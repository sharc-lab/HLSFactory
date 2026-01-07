# HLS-DSE Execution Guide

This guide introduces the scripts for running High-Level Synthesis Design Space Exploration (HLS-DSE) on both local machines and SLURM clusters.

## Entry Points

**For local machines:**
```bash
./local_multi_launch.sh
```

**For SLURM machines:**
```bash
./server_multi_launch.sh
```

## Script Overview

### Local Execution Scripts

#### `local_env.sh`
Environment configuration for local execution. Sets up:
- Xilinx tool paths and versions
- Target FPGA part number and frequency
- Benchmark-specific workspace paths

#### `local_single_run.sh`
Executes DSE for a single benchmark on the local machine. Handles:
- Environment setup (Python venv, Xilinx tools)
- Workspace preparation
- DSE execution
- Results collection

#### `local_multi_launch.sh`
**Main entry point for local machines.** Orchestrates multiple DSE experiments by:
- Validating benchmark directories
- Generating benchmark-specific test scripts
- Launching DSE runs for each benchmark sequentially or in parallel
- Logging execution status

### Server/SLURM Execution Scripts

#### `server_env.sh`
Environment configuration for SLURM cluster execution. Configures:
- Xilinx installation paths
- Target hardware specifications
- Dynamic benchmark-based workspace paths

#### `server_single_run.sh`
Executes DSE for a single benchmark as a SLURM job. Performs:
- Environment initialization
- Workspace setup in scratch space
- DSE execution
- Results transfer back to persistent storage

#### `server_submit.batch`
SLURM batch job template that defines:
- Resource requirements (CPUs, memory)
- Job naming and output logging
- Queue/partition selection
- Execution of `server_single_run.sh`

#### `server_multi_launch.sh`
**Main entry point for SLURM machines.** Manages multiple DSE experiments by:
- Validating benchmark availability
- Generating benchmark-specific scripts (`test_dse_*.py`, `server_single_run_*.sh`, `server_submit_*.batch`)
- Submitting jobs to SLURM via `sbatch`
- Recording submitted job IDs

## Usage Examples

### Running on Local Machine

```bash
# Run DSE for all configured benchmarks
./local_multi_launch.sh
```

### Running on SLURM Cluster

```bash
# Submit DSE jobs for all configured benchmarks
./server_multi_launch.sh

# Monitor job status
squeue -u $USER

```

## Configuration

Both local and server scripts use benchmark arrays that can be customized:

```bash
# Edit the BENCHMARKS array in local_multi_launch.sh or server_multi_launch.sh
BENCHMARKS=(
    "3mm"
    "syr2k"
    "atax-medium"
    "covariance"
    "gemm-p"
    "jacobi-2d"
    "trmm-opt"
)
```

## Generated Files

The multi-launch scripts automatically generate benchmark-specific files:

**Server execution:**
- `server_single_run_<benchmark>.sh` - Benchmark-specific run script
- `server_submit_<benchmark>.batch` - Benchmark-specific SLURM batch file
