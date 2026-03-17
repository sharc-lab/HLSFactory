#!/bin/bash
# Local DSE Environment Configuration

detect_workspace() {
    if [[ -n "$WORKSPACE" && -d "$WORKSPACE" ]]; then
        return 0
    fi

    local script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    if [[ -f "$script_dir/local_env.sh" && -d "$script_dir/database" ]]; then
        export WORKSPACE="$script_dir"
        return 0
    fi

    export WORKSPACE="/home/cy/github/DSE-HLS-FFT"

    if [[ ! -d "$WORKSPACE" ]]; then
        echo "❌ Error: WORKSPACE directory not found: $WORKSPACE"
        return 1
    fi
}

detect_workspace || exit 1
echo "🔧 Using workspace: $WORKSPACE"

# Xilinx tools (local paths)
export TARGET_PART_NUM=xcu200-fsgd2104-2-e
export TARGET_FREQ=250

# Benchmark name
if [[ -z "$BENCHMARK_NAME" ]]; then
    export BENCHMARK_NAME="3mm"
fi

# Local paths
export BUILD_LOC=~/local_scratch/run-${BENCHMARK_NAME}

# DSE framework paths
export BASE_DATABASE_PATH="${WORKSPACE}/database/hlsyn-vivado-target"
export BASE_OUTPUT_PATH="${WORKSPACE}/dse_levelheap_done"
export RESUM_FROM_PATH="${WORKSPACE}/dse_levelheap_done"

export XILINX_VITIS=/tools/Xilinx/2025.1/Vitis
source $XILINX_VITIS/settings64.sh
