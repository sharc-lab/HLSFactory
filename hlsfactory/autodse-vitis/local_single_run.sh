#!/bin/bash

# LibClang (using libclang-18 available on system)
export LIBCLANG_FILE=/usr/lib/x86_64-linux-gnu/libclang-18.so.18
export LD_LIBRARY_PATH=$(dirname "$LIBCLANG_FILE"):$LD_LIBRARY_PATH

# Activate Python venv (update path)
source ~/envs/hlsdse/bin/activate || {
    echo "❌ Failed to activate venv. Create with: python -m venv ~/envs/hlsdse"
    exit 1
}

# Load local environment
source local_env.sh

# Setup build location
echo "🔧 Removing: $BUILD_LOC"
rm -rf "$BUILD_LOC/"
echo "🔧 Copying essential files to $BUILD_LOC"
mkdir -p "$BUILD_LOC"
cp -R "$WORKSPACE/src" "$BUILD_LOC/"
cp -R "$WORKSPACE/database" "$BUILD_LOC/"
cp "$WORKSPACE"/*.sh "$BUILD_LOC/" 2>/dev/null || true

# Calculate paths
TEMPLATE_DIR="${BASE_DATABASE_PATH}/${BENCHMARK_NAME}"
OUTPUT_DIR="${BASE_OUTPUT_PATH}/${BENCHMARK_NAME}"
TEMP_OUTPUT_DIR="${BUILD_LOC}/dse_output/${BENCHMARK_NAME}"
RESUM_FROM_DIR="${RESUM_FROM_PATH}/${BENCHMARK_NAME}"

echo "🔧 Running DSE for benchmark: $BENCHMARK_NAME"
echo "   Template: $TEMPLATE_DIR"
echo "   Temp Output: $TEMP_OUTPUT_DIR"
echo "   Final Output: $OUTPUT_DIR"

resume=true
max_iterations=3
hours=70
timeout=15

mkdir -p "$TEMP_OUTPUT_DIR"

# Resume: copy state files
if [ "$resume" = true ]; then
    echo "🔄 Resume mode enabled"
    [ -f "$RESUM_FROM_DIR/pragma_state.pkl" ] && cp "$RESUM_FROM_DIR/pragma_state.pkl" "$TEMP_OUTPUT_DIR/" && echo "   ✅ Copied pragma_state.pkl"
    [ -f "$RESUM_FROM_DIR/final_trace.json" ] && cp "$RESUM_FROM_DIR/final_trace.json" "$TEMP_OUTPUT_DIR/" && echo "   ✅ Copied final_trace.json"
fi

cd "$BUILD_LOC/src"

# Cleanup trap
cleanup() {
    echo "🧹 Cleaning up..."
    kill $SNAPSHOT_PID 2>/dev/null || true
    wait $SNAPSHOT_PID 2>/dev/null || true
}
trap cleanup EXIT INT TERM

# Run DSE
python test_dse_LevelHeap_DFS.py "$TEMPLATE_DIR" "$TEMP_OUTPUT_DIR" --resume $resume --max-iterations $max_iterations --hours $hours --timeout $timeout &
MAIN_PID=$!

# Hourly snapshots
hour_counter=1
if [ -d "$OUTPUT_DIR/history" ]; then
    last_hour=$(find "$OUTPUT_DIR/history" -maxdepth 1 -type d -name "hour*" | sed -E 's#.*/hour([0-9]+)$#\1#' | sort -n | tail -n1)
    [ -n "$last_hour" ] && hour_counter=$(( last_hour + 1 ))
fi
echo "⏱️ Resuming snapshots from hour${hour_counter}"

(
    while true; do
        sleep 3600
        if ! kill -0 $MAIN_PID 2>/dev/null; then
            echo "📸 Main process completed"
            break
        fi
        HISTORY_DIR="$OUTPUT_DIR/history/hour${hour_counter}"
        mkdir -p "$HISTORY_DIR"
        echo "📸 [$(date '+%Y-%m-%d %H:%M:%S')] Snapshot to hour${hour_counter}"
        [ -f "$TEMP_OUTPUT_DIR/explore.log" ] && cp "$TEMP_OUTPUT_DIR/explore.log" "$HISTORY_DIR/"
        [ -f "$TEMP_OUTPUT_DIR/final_trace.json" ] && cp "$TEMP_OUTPUT_DIR/final_trace.json" "$HISTORY_DIR/"
        [ -f "$TEMP_OUTPUT_DIR/levelheap_dse_trace.log" ] && cp "$TEMP_OUTPUT_DIR/levelheap_dse_trace.log" "$HISTORY_DIR/"
        [ -f "$TEMP_OUTPUT_DIR/pragma_state.pkl" ] && cp "$TEMP_OUTPUT_DIR/pragma_state.pkl" "$HISTORY_DIR/"
        [ -f "$TEMP_OUTPUT_DIR/pragma_state.txt" ] && cp "$TEMP_OUTPUT_DIR/pragma_state.txt" "$HISTORY_DIR/"
        ((hour_counter++))
    done
) &
SNAPSHOT_PID=$!

wait $MAIN_PID
MAIN_EXIT_CODE=$?

kill $SNAPSHOT_PID 2>/dev/null || true
wait $SNAPSHOT_PID 2>/dev/null || true

echo "✅ Main DSE finished with exit code: $MAIN_EXIT_CODE"

# Copy results
echo "📂 Copying results from $TEMP_OUTPUT_DIR to $OUTPUT_DIR"
mkdir -p "$OUTPUT_DIR"

for work_dir in "$TEMP_OUTPUT_DIR"/work*; do
    if [[ -d "$work_dir" ]]; then
        work_name=$(basename "$work_dir")
        echo "📁 Copying $work_name..."
        mkdir -p "$OUTPUT_DIR/$work_name"
        find "$work_dir" -maxdepth 1 -type f -exec cp {} "$OUTPUT_DIR/$work_name/" \;
        find "$work_dir" -maxdepth 1 -type d ! -name "project" ! -path "$work_dir" -exec cp -R {} "$OUTPUT_DIR/$work_name/" \;
        if [[ -d "$work_dir/project/solution/syn/report" ]]; then
            cp "$work_dir/project/solution/syn/report/csynth.rpt" "$OUTPUT_DIR/$work_name/logs/csynth.rpt"
            cp "$work_dir/project/solution/syn/report/csynth.xml" "$OUTPUT_DIR/$work_name/logs/csynth.xml"
        fi
    fi
done

find "$TEMP_OUTPUT_DIR" -maxdepth 1 ! -name "variant_*" ! -name "work*" ! -path "$TEMP_OUTPUT_DIR" -exec cp -R {} "$OUTPUT_DIR/" \;

echo "✅ Results copied to $OUTPUT_DIR"

rm -rf "$BUILD_LOC/"

echo "All configurations built successfully."
