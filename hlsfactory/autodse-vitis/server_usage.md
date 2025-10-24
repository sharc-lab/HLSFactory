# 🚀 DSE Server Usage Guide

This guide provides comprehensive instructions for running High-Level Synthesis Design Space Exploration (HLS-DSE) on server clusters using SLURM.

## 📁 File Overview

### Core Scripts
- **`server_env.sh`** - Environment configuration (Xilinx tools, paths, benchmark names)
- **`server_single_run.sh`** - Single benchmark DSE execution script
- **`server_submit.batch`** - SLURM batch job template
- **`server_multi_launch.sh`** - Multi-benchmark launcher (main script)
- **`server_monitor.sh`** - Job monitoring and progress dashboard

### Generated Scripts (Auto-created)
- **`test_dse_<benchmark>_LevelHeap_DFS.py`** - Benchmark-specific test scripts
- **`server_single_run_<benchmark>.sh`** - Benchmark-specific run scripts
- **`server_submit_<benchmark>.batch`** - Benchmark-specific SLURM scripts

## 🎯 Quick Start

### 1. Single Benchmark Run
```bash
# Run DSE for the default benchmark (currently syr2k)
sbatch server_submit.batch

# Or run directly with custom benchmark/paths
cd 0_src
python test_dse_LevelHeap_DFS.py database/vitishlssyn/3mm 0_work_dir/dse_test/3mm

# Or with additional options
python test_dse_LevelHeap_DFS.py \
    database/vitishlssyn/3mm \
    0_work_dir/dse_test/3mm \
    --hours 12 \
    --timeout 15 \
    --resume
```

### 2. Multi-Benchmark Run
```bash
# Run DSE for multiple benchmarks simultaneously
bash server_multi_launch.sh
```

### 3. Monitor Progress
```bash
# Interactive monitoring dashboard
bash server_monitor.sh

# Command-line monitoring
bash server_monitor.sh all
```

## 🔧 Detailed Usage

### Environment Configuration (`server_env.sh`)

This script sets up the execution environment with dynamic benchmark support:

```bash
# Environment variables (auto-configured)
export XILINX_INSTALL_PATH=/mnt/software/xilinx
export TARGET_PART_NUM=xcu200-fsgd2104-2-e
export TARGET_FREQ=250

# Dynamic benchmark paths (workspace-based)
export BENCHMARK_NAME="3mm"  # Default, can be overridden
export BUILD_LOC=~/scratch/run-${BENCHMARK_NAME}
```

**Key Features:**
- ✅ **Dynamic Benchmark Names**: Automatically adapts paths based on `$BENCHMARK_NAME`
- ✅ **Xilinx Tool Setup**: Configures Vitis HLS 2024.2 environment
- ✅ **Target Hardware**: Pre-configured for `xcu200-fsgd2104-2-e` FPGA

### Single Benchmark Execution (`server_single_run.sh`)

Executes DSE for a single benchmark:

```bash
#!/bin/bash
export BENCHMARK_NAME="benchmark_name"  # Set by multi-launch script

# Environment setup
export LIBCLANG_FILE=/mnt/software/spack/.../libclang.so
source /home/cy/playground/19-dse-hls/envs/hlsdse/bin/activate
source server_env.sh

# Execute DSE
cd "$BUILD_LOC/0_src"
python test_dse_benchmark_name_LevelHeap_DFS.py

# Copy results back to main workspace
cp -R "$BUILD_LOC/0_work_dir" "$WORKSPACE/0_work_dir_done"
```

**Process Flow:**
1. **Environment Setup**: Activates Python venv, sources Xilinx tools
2. **Workspace Preparation**: Copies source to scratch space
3. **DSE Execution**: Runs LevelHeap DFS algorithm
4. **Results Collection**: Copies results back to persistent storage

### SLURM Job Configuration (`server_submit.batch`)

SLURM batch job template:

```bash
#!/bin/bash
#SBATCH --job-name=hls-dse
#SBATCH -o %x-%A_%a.out
#SBATCH --cpus-per-task=8
#SBATCH --mem 45G
#SBATCH -p batch

bash server_single_run.sh
```

**Resource Requirements:**
- **CPUs**: 8 cores per job
- **Memory**: 45GB RAM
- **Queue**: batch partition
- **Output**: Job-specific log files

### Multi-Benchmark Launcher (`server_multi_launch.sh`)

Orchestrates multiple DSE experiments:

**Configuration Section:**
```bash
# Benchmark array - edit this to customize runs
BENCHMARKS=(
    "3mm"           # Matrix multiplication
    "syr2k"         # Symmetric rank-2k update  
    "atax-medium"   # Matrix transpose and vector multiplication
    "covariance"    # Covariance matrix computation
    "gemm-p"        # General matrix multiplication
    "jacobi-2d"     # 2D Jacobi stencil
    "trmm-opt"      # Triangular matrix multiplication (optimized)
)
```

**Process Flow:**
1. **Validation**: Checks if benchmarks exist in `database/vitishlssyn/`
2. **Script Generation**: Creates benchmark-specific scripts for each benchmark
3. **Job Submission**: Submits separate SLURM jobs via `sbatch`
4. **Logging**: Records all submitted job IDs

**Generated Files Per Benchmark:**
```bash
test_dse_3mm_LevelHeap_DFS.py       # 3mm-specific test script
server_single_run_3mm.sh            # 3mm-specific run script  
server_submit_3mm.batch             # 3mm-specific SLURM script
```

### Monitoring Dashboard (`server_monitor.sh`)

Comprehensive monitoring tool with interactive menu:

**Features:**
- 🔍 **Job Status**: Shows running/queued SLURM jobs
- ✅ **Completion Tracking**: Lists recently completed jobs  
- 📂 **Output Monitoring**: Shows disk usage and directory status
- 📊 **Progress Tracking**: Counts iterations and shows last activity
- 🧹 **Cleanup Options**: Provides cleanup commands

**Command-Line Usage:**
```bash
bash server_monitor.sh status      # Show job status
bash server_monitor.sh completed   # Show completed jobs
bash server_monitor.sh output      # Show output directory status
bash server_monitor.sh progress    # Show exploration progress
bash server_monitor.sh all         # Show everything
```

## 📂 Directory Structure

### Input (Templates)
```
database/vitishlssyn/
├── 3mm/                    # 3x3 matrix multiplication
│   ├── 3mm.cpp
│   ├── 3mm.h
│   ├── 3mm.json           # Pragma options
│   └── project.tcl
├── syr2k/                  # Symmetric rank-2k update
├── atax-medium/           # Matrix transpose (medium size)
└── ...
```

### Output (Results)
```
0_work_dir/dse_levelheap_test/
├── 3mm/
│   └── levelheap_dfs_test/
│       ├── explore.log                 # Main exploration log
│       ├── levelheap_dse_trace.log    # Algorithm trace
│       ├── final_trace.json           # Machine-readable results
│       ├── pragma_state.pkl           # Checkpoint data
│       ├── variant_N/                 # Generated design variants
│       └── work_N/                    # Synthesis workspaces
├── syr2k/
│   └── levelheap_dfs_test/
└── ...
```

### Temporary Files
```
~/scratch/
├── run-3mm/              # Temporary workspace for 3mm
├── run-syr2k/            # Temporary workspace for syr2k  
└── ...
```

## ⚙️ Configuration Options

### DSE Parameters (`test_dse_LevelHeap_DFS.py`)

Key parameters you can modify:

```python
# Exploration control
MAX_ITERATIONS = 3              # Fixed iteration limit
MAX_ITERATIONS = None           # Enable time-based exploration
EXPLORATION_HOURS = 24          # Run for 24 hours

# Synthesis settings
TIMEOUT_MINUTES = 10            # Per-synthesis timeout
RESUME_FROM_CHECKPOINT = False  # Resume from previous run

# Objective function (in the script)
objective = ResourceConstrainedLatencyObjective(
    max_dsp=5130,      # DSP resource limit
    max_bram=3240,     # BRAM resource limit  
    max_lut=886680,    # LUT resource limit
    max_ff=1773360,    # Flip-flop resource limit
    max_uram=720,      # UltraRAM resource limit
    top_module="kernel_name"  # Top-level module name
)
```

### Hardware Target (`server_env.sh`)

```bash
export TARGET_PART_NUM=xcu200-fsgd2104-2-e  # Xilinx U200 FPGA
export TARGET_FREQ=250                       # 250 MHz target frequency
```

### SLURM Resources (`server_submit.batch`)

```bash
#SBATCH --cpus-per-task=8    # CPU cores per job
#SBATCH --mem 45G            # Memory per job  
#SBATCH -p batch             # Queue/partition
#SBATCH -t 48:00:00          # Optional: time limit
```

## 📊 Monitoring and Results

### Real-Time Monitoring

**Check Running Jobs:**
```bash
squeue -u $USER
```

**Monitor Specific Job:**
```bash
tail -f hls-dse-JOBID.out
```

**Check Progress:**
```bash
bash server_monitor.sh progress
```

### Result Analysis

**Key Output Files:**
- **`explore.log`**: Human-readable exploration log
- **`final_trace.json`**: Complete results in JSON format
- **`levelheap_dse_trace.log`**: Detailed algorithm execution trace
- **`pragma_state.pkl`**: Checkpoint for resuming exploration

**Success Metrics:**
- ✅ **Successful Designs**: Meet resource constraints
- ❌ **Failed Designs**: Exceed resource limits or synthesis failures  
- 🏆 **Best Design**: Lowest latency within constraints

### Example Results Summary
```
📊 LEVELHEAP DFS RESULTS ANALYSIS
=====================================
📋 Iteration Summary:
  ✅ Iteration  0: 5604.0 (latency: 5604.0)
  ❌ Iteration  1: INFEASIBLE - Resource violations: ['DSP: 6797 > 5130']
  ✅ Iteration  2: 4321.0 (latency: 4321.0)

📈 Summary:
  Total iterations: 3
  Successful synthesis: 3  
  Failed synthesis: 0
  Feasible designs: 2
  Infeasible designs: 1
  Success rate: 100.0%
  Feasible rate: 66.7%

🏆 Best Feasible Result (Iteration 2):
  Latency: 4321.0
  Resources: DSP=4892, BRAM=2156, LUT=325467
```

## 🧹 Cleanup and Maintenance

### Clean Up Generated Scripts
```bash
# Remove benchmark-specific scripts
rm -f server_single_run_*.sh server_submit_*.batch test_dse_*_LevelHeap_DFS.py
```

### Clean Up SLURM Output Files
```bash
# Remove job output files
rm -f hls-dse-*.out dse-*.out
```

### Clean Up Work Directories
```bash
# Remove temporary scratch directories (be careful!)
rm -rf ~/scratch/run-*
```

### Archive Results
```bash
# Archive completed results
tar -czf dse_results_$(date +%Y%m%d).tar.gz 0_work_dir/dse_levelheap_test/
```

## 🚨 Troubleshooting

### Common Issues

**1. Job Fails Immediately**
```bash
# Check SLURM output file
cat hls-dse-JOBID.out

# Common causes:
# - Missing Xilinx tools: Check XILINX_INSTALL_PATH
# - Python environment: Check virtual environment activation
# - Missing benchmark: Check database/vitishlssyn/<benchmark>/
```

**2. Synthesis Timeouts**
```bash
# Increase timeout in test script
TIMEOUT_MINUTES = 20  # Increase from default 10 minutes
```

**3. Resource Constraint Violations**
```bash
# Adjust resource limits in objective function
objective = ResourceConstrainedLatencyObjective(
    max_dsp=6000,      # Increase limits
    max_bram=4000,
    # ... other resources
)
```

**4. Out of Memory Errors**
```bash
# Increase SLURM memory allocation
#SBATCH --mem 64G     # Increase from 45G
```

### Debug Commands

**Check Environment:**
```bash
echo $XILINX_VITIS
echo $BUILD_LOC  
echo $BENCHMARK_NAME
```

**Test Xilinx Tools:**
```bash
source server_env.sh
which vitis_hls
vitis_hls -version
```

**Check Disk Space:**
```bash
df -h ~/scratch/
df -h 0_work_dir/
```

## 🔗 Advanced Usage

### Custom Benchmarks

1. **Add Benchmark Directory:**
   ```bash
   mkdir database/vitishlssyn/my_benchmark/
   # Add source files and project.tcl
   ```

2. **Update Benchmark Array:**
   ```bash
   # In server_multi_launch.sh
   BENCHMARKS=("my_benchmark" "3mm" "syr2k")
   ```

### Parallel Parameter Sweeps

Run same benchmark with different parameters:
```bash
# Create multiple test scripts with different configurations
# Modify MAX_ITERATIONS, EXPLORATION_HOURS, resource limits, etc.
```

### Integration with External Tools

**Export Results to CSV:**
```python
# Add to analysis scripts
import json, csv
with open('final_trace.json') as f:
    data = json.load(f)
# Convert to CSV format
```

**Integration with Plotting Tools:**
```bash
# Generate plots from results
python analysis/plot_results.py final_trace.json
```

---

## 📞 Support

For issues and questions:
1. Check the troubleshooting section above
2. Examine SLURM output files (`*.out`)
3. Use the monitoring dashboard: `bash server_monitor.sh`
4. Check log files in output directories

**Happy exploring! 🚀**