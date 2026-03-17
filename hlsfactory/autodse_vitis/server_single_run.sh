#!/bin/bash
### You can also run run.sh directly after logging into the server in interactive mode using: srun -c32 --mem=180G --pty bash

# Python environment setup
# Create virtual environment if needed:
# python -m venv ~/envs/hlsdse
# pip install --upgrade --no-cache-dir "clang==16.0.6"

# LibClang configuration
export LIBCLANG_FILE=/mnt/software/spack/opt/spack/linux-ubuntu22.04-x86_64_v3/gcc-12.1.0/llvm-16.0.6-xmoco2xncbbs7refq3asjtrc6kua5zka/lib/libclang.so
export LD_LIBRARY_PATH=$(dirname "$LIBCLANG_FILE"):$LD_LIBRARY_PATH

# Activate Python virtual environment
# Update this path to match your virtual environment location
source /home/cy/playground/19-dse-hls/envs/hlsdse/bin/activate || {
    echo "❌ Failed to activate Python virtual environment"
    echo "   Please create virtual environment: python -m venv ~/envs/hlsdse"
    echo "   And install dependencies: pip install clang==16.0.6"
    exit 1
}

# Load DSE workspace configuration
source server_env.sh 

# Ensure the main build directory exists
# mkdir -p "$WORKSPACE/build"
echo "🔧 Removing: $BUILD_LOC"
rm -rf "$BUILD_LOC/"
# mkdir -p "$BUILD_LOC/"

echo "🔧 Copying essential files to $BUILD_LOC (src/, database/, *.sh)"
mkdir -p "$BUILD_LOC"
cp -R "$WORKSPACE/src" "$BUILD_LOC/"
echo "✅  Finished copying src/ to $BUILD_LOC"
cp -R "$WORKSPACE/database" "$BUILD_LOC/"
echo "✅  Finished copying database/ to $BUILD_LOC"
cp "$WORKSPACE"/*.sh "$BUILD_LOC/" 2>/dev/null || true
echo "✅  Finished copying *.sh to $BUILD_LOC"


# Calculate benchmark-specific paths (using environment variables from server_env.sh)
TEMPLATE_DIR="${BASE_DATABASE_PATH}/${BENCHMARK_NAME}"
OUTPUT_DIR="${BASE_OUTPUT_PATH}/${BENCHMARK_NAME}"
TEMP_OUTPUT_DIR="${BUILD_LOC}/dse_output/${BENCHMARK_NAME}"
RESUM_FROM_DIR="${RESUM_FROM_PATH}/${BENCHMARK_NAME}"


echo "🔧 Running DSE for benchmark: $BENCHMARK_NAME"
echo "   Template: $TEMPLATE_DIR"
echo "   Temp Output (BUILD_LOC): $TEMP_OUTPUT_DIR"
echo "   Final Output: $OUTPUT_DIR"

resume=true
max_iterations=None   # Maximum number of iterations (use None for time-based exploration)
hours=70           # Exploration time in hours for time-based exploratio
timeout=15          # Synthesis timeout in minutes

# Create temporary output directory in BUILD_LOC
mkdir -p "$TEMP_OUTPUT_DIR"

# Resume functionality: copy pragma_state.pkl and final_trace.json if resume is true
if [ "$resume" = true ]; then
    echo "🔄 Resume mode enabled: copying state files from $RESUM_FROM_DIR to $TEMP_OUTPUT_DIR"
    if [ -f "$RESUM_FROM_DIR/pragma_state.pkl" ]; then
        cp "$RESUM_FROM_DIR/pragma_state.pkl" "$TEMP_OUTPUT_DIR/"
        echo "   ✅ Copied pragma_state.pkl"
    else
        echo "   ⚠️ No pragma_state.pkl found in $RESUM_FROM_DIR, starting fresh"
    fi

    if [ -f "$RESUM_FROM_DIR/final_trace.json" ]; then
        cp "$RESUM_FROM_DIR/final_trace.json" "$TEMP_OUTPUT_DIR/"
        echo "   ✅ Copied final_trace.json"
    else
        echo "   ⚠️ No final_trace.json found, will create new one"
    fi
fi

cd "$BUILD_LOC/src"

# Function to create and copy zip of work* directories
create_and_copy_work_zip() {
    cd "$TEMP_OUTPUT_DIR"

    if ! compgen -G "work*" > /dev/null; then
        echo "⚠️ No work* directories found to zip"
        cd "$BUILD_LOC/src"
        return
    fi

    # Find the highest work directory number to use in zip name
    last_work_num=$(find . -maxdepth 1 -type d -name "work_*" \
        | sed -E 's#./work_([0-9]+)$#\1#' | sort -n | tail -n1)

    if [ -z "$last_work_num" ]; then
        echo "⚠️ Could not determine work directory numbers"
        cd "$BUILD_LOC/src"
        return
    fi

    ZIP_NAME="work_until_${last_work_num}.zip"
    echo "📦 Creating ${ZIP_NAME} from work* directories (up to work_${last_work_num})..."

    # Process each work directory for selective copying
    for work_dir in work_*; do
        if [[ -d "$work_dir" ]]; then
            echo "📁 Processing $work_dir for zip..."

            # Handle project directory selectively - copy only specific report files
            if [[ -d "$work_dir/project/solution/syn/report" ]]; then
                mkdir -p "$work_dir/logs"
                cp "$work_dir/project/solution/syn/report/csynth.rpt" "$work_dir/logs/csynth.rpt" 2>/dev/null || true
                cp "$work_dir/project/solution/syn/report/csynth.xml" "$work_dir/logs/csynth.xml" 2>/dev/null || true
            fi

            # Remove the entire project directory to save space
            rm -rf "$work_dir/project"
        fi
    done

    # Create the zip file with all work* directories
    zip -r "$ZIP_NAME" work_* -q
    echo "✅ Created ${ZIP_NAME}"

    # Copy zip to output directory
    mkdir -p "$OUTPUT_DIR"
    cp "$ZIP_NAME" "$OUTPUT_DIR/"
    echo "✅ Copied ${ZIP_NAME} to $OUTPUT_DIR"

    # Clean up work* directories and zip from temp to free memory and disk space
    echo "🧹 Cleaning up work* directories from temp to free memory..."
    rm -rf work_*
    rm -f "$ZIP_NAME"
    echo "✅ Cleaned up temp work directories and zip file"

    cd "$BUILD_LOC/src"
}

# Trap handler to ensure cleanup on exit or termination (SLURM signals)
cleanup() {
    echo "🧹 Cleaning up background processes..."
    kill $SNAPSHOT_PID 2>/dev/null || true
    wait $SNAPSHOT_PID 2>/dev/null || true
    kill $ZIP_BACKUP_PID 2>/dev/null || true
    wait $ZIP_BACKUP_PID 2>/dev/null || true
    # Don't kill MAIN_PID here as it might be finishing naturally
}
trap cleanup EXIT INT TERM

# Run the main DSE process in background
python test_dse_LevelHeap_DFS.py "$TEMPLATE_DIR" "$TEMP_OUTPUT_DIR" --resume $resume --max-iterations $max_iterations --hours $hours --timeout $timeout &
MAIN_PID=$!

# Determine starting hour counter from existing history
hour_counter=1
if [ -d "$OUTPUT_DIR/history" ]; then
    last_hour=$(find "$OUTPUT_DIR/history" -maxdepth 1 -type d -name "hour*" \
        | sed -E 's#.*/hour([0-9]+)$#\1#' | sort -n | tail -n1)
    if [ -n "$last_hour" ]; then
        hour_counter=$(( last_hour + 1 ))
    fi
fi
echo "⏱️ Resuming snapshots from hour${hour_counter}"

# Start hourly snapshot background process
(
    while true; do
        sleep 3600  # Wait 1 hour

        # Check if main process is still running
        if ! kill -0 $MAIN_PID 2>/dev/null; then
            echo "📸 Main DSE process completed, stopping snapshots"
            break
        fi

        # Create history directory for this hour (save directly to OUTPUT_DIR)
        HISTORY_DIR="$OUTPUT_DIR/history/hour${hour_counter}"
        mkdir -p "$HISTORY_DIR"

        echo "📸 [$(date '+%Y-%m-%d %H:%M:%S')] Saving hourly snapshot to $OUTPUT_DIR/history/hour${hour_counter}/"

        # Copy key files if they exist
        [ -f "$TEMP_OUTPUT_DIR/explore.log" ] && cp "$TEMP_OUTPUT_DIR/explore.log" "$HISTORY_DIR/"
        [ -f "$TEMP_OUTPUT_DIR/final_trace.json" ] && cp "$TEMP_OUTPUT_DIR/final_trace.json" "$HISTORY_DIR/"
        [ -f "$TEMP_OUTPUT_DIR/levelheap_dse_trace.log" ] && cp "$TEMP_OUTPUT_DIR/levelheap_dse_trace.log" "$HISTORY_DIR/"
        [ -f "$TEMP_OUTPUT_DIR/pragma_state.pkl" ] && cp "$TEMP_OUTPUT_DIR/pragma_state.pkl" "$HISTORY_DIR/"
        [ -f "$TEMP_OUTPUT_DIR/pragma_state.txt" ] && cp "$TEMP_OUTPUT_DIR/pragma_state.txt" "$HISTORY_DIR/"

        ((hour_counter++))
    done
) &
SNAPSHOT_PID=$!

# Start 4-hour zip backup background process
(
    while true; do
        sleep 14400  # Wait 4 hours (4 * 3600 seconds)

        # Check if main process is still running
        if ! kill -0 $MAIN_PID 2>/dev/null; then
            echo "📦 Main DSE process completed, stopping periodic zip backups"
            break
        fi

        echo "📦 [$(date '+%Y-%m-%d %H:%M:%S')] Starting 4-hour zip backup..."
        create_and_copy_work_zip
    done
) &
ZIP_BACKUP_PID=$!

# Wait for main process to complete
wait $MAIN_PID
MAIN_EXIT_CODE=$?

# Ensure background processes are terminated
kill $SNAPSHOT_PID 2>/dev/null || true
wait $SNAPSHOT_PID 2>/dev/null || true
kill $ZIP_BACKUP_PID 2>/dev/null || true
wait $ZIP_BACKUP_PID 2>/dev/null || true

echo "✅ Main DSE process finished with exit code: $MAIN_EXIT_CODE"

# Copy results from temporary location back to main workspace
echo "📂 Copying final results from $TEMP_OUTPUT_DIR to $OUTPUT_DIR"
mkdir -p "$(dirname "$OUTPUT_DIR")"
mkdir -p "$OUTPUT_DIR"

# Create final zip for any remaining work* directories
echo "📦 Creating final zip for any remaining work* directories..."
create_and_copy_work_zip

# Copy any other files/directories that are not variant_* or work*
find "$TEMP_OUTPUT_DIR" -maxdepth 1 ! -name "variant_*" ! -name "work*" ! -path "$TEMP_OUTPUT_DIR" -exec cp -R {} "$OUTPUT_DIR/" \;

echo "✅ Results copied successfully to $OUTPUT_DIR"

rm -rf "$BUILD_LOC/"

# Finished
echo "All configurations built successfully."

