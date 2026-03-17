#!/bin/bash

# ============================================================================
# Multi-Benchmark DSE Launch Script
# ============================================================================
# This script launches multiple DSE experiments for different benchmarks
# Usage: bash server_multi_launch.sh

BENCHMARKS=(
    "2mm" 
    "3mm"
    "adi"
    "aes"
    "atax"
    "atax-medium"
    "bicg"
    "bicg-large"
    "bicg-medium"
    "correlation"
    "covariance"
    "doitgen"
    "doitgen-red"
    "fdtd-2d"
    "fdtd-2d-large"
    "gemm-blocked"
    "gemm-ncubed"
    "gemm-p"
    "gemm-p-large"
    "gemver"
    "gemver-medium"
    "gesummv"
    "gesummv-medium"
    "heat-3d"
    "jacobi-1d"
    "jacobi-2d"
    "md"
    "mvt"
    "mvt-medium"
    "nw"
    "seidel-2d"
    "spmv-crs"
    "spmv-ellpack"
    "stencil-3d"
    "stencil_stencil2d"
    "symm"
    "symm-opt"
    "symm-opt-medium"
    "syr2k"
    "syrk"
    "trmm" 
    "trmm-opt"
)



# Load environment configuration
source server_env.sh

# Configuration (using environment variables from server_env.sh)
# BASE_DATABASE_PATH and BASE_OUTPUT_PATH are now set in server_env.sh
CURRENT_DIR=$(pwd)

# Function to check if benchmark exists
check_benchmark_exists() {
    local benchmark=$1
    local benchmark_path="$BASE_DATABASE_PATH/$benchmark"
    
    if [[ ! -d "$benchmark_path" ]]; then
        echo "❌ Error: Benchmark '$benchmark' not found at $benchmark_path"
        echo "Available benchmarks:"
        ls "$BASE_DATABASE_PATH" 2>/dev/null | grep -v "llvmpass" || echo "No benchmarks found"
        return 1
    fi
    return 0
}

# Function to create benchmark-specific single run script
create_benchmark_run_script() {
    local benchmark=$1
    local script_path="$CURRENT_DIR/server_single_run_${benchmark}.sh"
    local template_path="$CURRENT_DIR/server_single_run.sh"
    
    echo "📝 Creating benchmark-specific run script: $script_path"
    
    # Copy the template and modify for this benchmark
    cp "$template_path" "$script_path"
    
    # Add benchmark name export at the beginning (after shebang)
    sed -i '2i export BENCHMARK_NAME="'$benchmark'"' "$script_path"
    
    echo "✅ Created $script_path"
}

# Function to create benchmark-specific batch script  
create_benchmark_batch_script() {
    local benchmark=$1
    local script_path="$CURRENT_DIR/server_submit_${benchmark}.batch"
    local template_path="$CURRENT_DIR/server_submit.batch"
    
    echo "📝 Creating benchmark-specific batch script: $script_path"
    
    # Copy the template and modify job name and script call
    cp "$template_path" "$script_path"
    
    # Update job name and script call
    sed -i "s|#SBATCH --job-name=hls-dse|#SBATCH --job-name=dse-${benchmark}|g" "$script_path"
    sed -i "s|bash server_single_run.sh|bash server_single_run_${benchmark}.sh|g" "$script_path"
    
    echo "✅ Created $script_path"
}

# Function to submit job for a benchmark
submit_benchmark_job() {
    local benchmark=$1
    local batch_script="server_submit_${benchmark}.batch"
    
    echo "🚀 Submitting job for benchmark: $benchmark"
    
    if [[ -f "$batch_script" ]]; then
        local job_id=$(sbatch "$batch_script" 2>&1)
        if [[ $? -eq 0 ]]; then
            echo "✅ Job submitted successfully for $benchmark: $job_id"
            echo "$job_id" >> submitted_jobs.log
        else
            echo "❌ Failed to submit job for $benchmark: $job_id"
        fi
    else
        echo "❌ Batch script not found: $batch_script"
    fi
}

# Main execution
echo "🚀 Starting Multi-Benchmark DSE Launch"
echo "======================================"
echo "Benchmarks to run: ${BENCHMARKS[*]}"
echo "Total benchmarks: ${#BENCHMARKS[@]}"
echo ""
echo "📝 Using argument-based approach:"
echo "   - Single test script with command-line arguments"
echo "   - Template and output paths passed as arguments"
echo "   - No need to generate separate test scripts"
echo ""

# Clear previous job log
> submitted_jobs.log

# Process each benchmark
for benchmark in "${BENCHMARKS[@]}"; do
    echo ""
    echo "🔧 Processing benchmark: $benchmark"
    echo "-----------------------------------"
    
    # Check if benchmark exists
    if ! check_benchmark_exists "$benchmark"; then
        echo "⏭️ Skipping $benchmark"
        continue
    fi
    
    # Create benchmark-specific scripts (only run and batch scripts needed)
    create_benchmark_run_script "$benchmark"
    create_benchmark_batch_script "$benchmark"
    
    # Submit the job
    submit_benchmark_job "$benchmark"
    
    # Small delay between submissions
    sleep 2
done

echo ""
echo "🎉 Multi-launch completed!"
echo "========================="
echo "Submitted jobs:"
cat submitted_jobs.log 2>/dev/null || echo "No jobs were submitted"

echo ""
echo "📊 Monitor jobs with:"
echo "  squeue -u \$USER"
echo "  bash server_monitor.sh"
echo ""
echo "📂 Results will be saved to:"
echo "  $BASE_OUTPUT_PATH/<benchmark>/levelheap_dfs_test/"
echo ""
echo "🧹 Cleanup benchmark-specific scripts:"
echo "  rm -f server_single_run_*.sh server_submit_*.batch"