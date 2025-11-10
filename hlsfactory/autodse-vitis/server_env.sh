# ============================================================================
# DSE Environment Configuration
# ============================================================================

# Auto-detect workspace location (DSE-HLS-FFT repository)
detect_workspace() {
    # If WORKSPACE is already set, use it
    if [[ -n "$WORKSPACE" && -d "$WORKSPACE" ]]; then
        return 0
    fi
    
    # Try to detect from script location
    local script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    if [[ -f "$script_dir/server_env.sh" && -d "$script_dir/database" ]]; then
        export WORKSPACE="$script_dir"
        return 0
    fi
    
    # Fallback to hardcoded path
    export WORKSPACE="/home/cy/github/DSE-HLS-FFT"
    
    # Verify workspace exists
    if [[ ! -d "$WORKSPACE" ]]; then
        echo "❌ Error: WORKSPACE directory not found: $WORKSPACE"
        echo "   Please set WORKSPACE environment variable to DSE-HLS-FFT repository location"
        return 1
    fi
}

# Detect workspace location
detect_workspace || exit 1

echo "🔧 Using workspace: $WORKSPACE"

# Xilinx tools configuration
export XILINX_INSTALL_PATH=/mnt/software/xilinx # typically it is /tools/Xilinx, under which contains Vitis/Vivado/Vitis_HLS ...
export TARGET_PART_NUM=xcu200-fsgd2104-2-e
export TARGET_FREQ=250

# Dynamic benchmark name - can be overridden by calling script
if [[ -z "$BENCHMARK_NAME" ]]; then
    export BENCHMARK_NAME="3mm"  # Default fallback
fi

# Workspace-based path configuration
export BUILD_LOC=~/scratch/run-${BENCHMARK_NAME}              # Temporary scratch workspace

# DSE framework paths (derived from WORKSPACE)
export BASE_DATABASE_PATH="${WORKSPACE}/database/hlsyn-vivado-target"
export BASE_OUTPUT_PATH="${WORKSPACE}/dse_levelheap_done"
export RESUM_FROM_PATH="${WORKSPACE}/dse_levelheap_done"

# export XILINX_VITIS=$XILINX_INSTALL_PATH/Vitis/2024.2
export XILINX_VITIS=$XILINX_INSTALL_PATH/2025.1/Vitis
source $XILINX_VITIS/settings64.sh

