# OptDSL

OptDSL (Optimization Domain Specific Language) is a design space exploration frontend in HLSFactory that automatically generates multiple design variations by applying different HLS optimization directives to a base design. It serves as a powerful tool for systematically exploring the design space of HLS optimizations.

## Overview

OptDSL enables users to:

- **Automate design space exploration** by generating multiple design variants with different optimization parameters
- **Apply HLS optimization directives** systematically across parameter ranges
- **Support multiple HLS tools** including Xilinx Vitis HLS and Intel HLS
- **Generate comprehensive datasets** for HLS research and benchmarking
- **Reduce manual effort** in exploring optimization combinations

## OptDSL Versions

HLSFactory includes two versions of OptDSL with different syntax approaches:

### OptDSL v1 (Production Ready)

- **Location**: `hlsfactory/opt_dsl_frontend.py`
- **Syntax**: Custom DSL with template-based approach
- **Status**: Mature and fully functional
- **Support**: Complete Xilinx and Intel tool support

### OptDSL v2 (Under Development)

- **Location**: `hlsfactory/opt_dsl_v2/` and `hlsfactory/opt_dsl_frontend_v2.py`
- **Syntax**: Pythonic syntax with function calls
- **Status**: Work in progress (limited functionality)
- **Support**: Basic optimization directives

### OptDSL v3 (Under Even More Development)

No details available yet.

## OptDSL v1 Syntax

### Template Structure

OptDSL v1 uses `opt_template.tcl` files with a custom domain-specific syntax:

```tcl
# Array partitioning specification
array_partition,NUM_SPECS,[FACTOR_LIST],TYPE
set_directive_array_partition -type [type] -factor [factor] -dim DIM "FUNCTION" ARRAY

# Loop optimization specification  
loop_opt,NUM_LOOP_SPECS,NUM_STATIC_SPECS
LOOP_ID,LOOP_NAME,pipeline,unroll,[FACTOR_LIST]
set_directive_unroll -factor [factor] FUNCTION/[name]
set_directive_pipeline FUNCTION/[name]
```

### Array Partitioning

Array partitioning optimizes memory access patterns:

```tcl
# Partition array A with different factors and types
array_partition,1,[1 2 4 8],cyclic
set_directive_array_partition -type [type] -factor [factor] -dim 2 "atax" A
```

**Parameters**:

- **Factors**: `[1 2 4 8 16 32 64]` - Partition factors to explore
- **Types**: `cyclic`, `block`, `complete` - Partitioning strategies
- **Dimension**: Target dimension for partitioning (1, 2, etc.)

### Loop Optimization

Loop optimizations control pipelining and unrolling:

```tcl
# Configure 3 loop optimizations, 2 static directives
loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]  
2,lp3,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] atax/[name]
set_directive_pipeline atax/[name]
```

**Parameters**:

- **Pipeline**: Enable loop pipelining
- **Unroll factors**: `[1 2 4 8 16]` - Unroll factors to explore
- **Loop names**: Target loop labels from HLS synthesis

### Example Template

From `polybench/atax/opt_template.tcl`:

```tcl
# Array partitioning for matrices
array_partition,7,[1 2 4 8],cyclic
set_directive_array_partition -type [type] -factor [factor] -dim 2 "atax" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "atax" buff_A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "atax" x
set_directive_array_partition -type [type] -factor [factor] -dim 1 "atax" y
set_directive_array_partition -type [type] -factor [factor] -dim 1 "atax" buff_y
set_directive_array_partition -type [type] -factor [factor] -dim 1 "atax" tmp1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "atax" tmp2

# Loop optimizations
loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] atax/[name]
set_directive_pipeline atax/[name]
```

## OptDSL v2 Syntax (Pythonic)

OptDSL v2 uses a more intuitive Python-like syntax:

```python
# Array partitioning with loops
for factor in [1, 2, 4, 8]:
    for partition_type in ["cyclic", "block"]:
        partition("A", "atax", partition_type, factor, 2)
        partition("buff_A", "atax", partition_type, factor, 2)
        partition("tmp1", "atax", partition_type, factor, 1)
    
    # Loop optimizations
    unroll("lprd_2", "atax", factor)
    unroll("lpwr_1", "atax", factor)

# Pipeline loops
pipeline("lprd_2", "atax")
pipeline("lpwr_1", "atax")
```

**Functions**:

- `partition(array, function, type, factor, dimension)`
- `unroll(loop, function, factor)`
- `pipeline(loop, function)`

## Usage Examples

### Basic OptDSL Execution

```python
from hlsfactory.opt_dsl_frontend import OptDSLFrontend

# Create OptDSL frontend
opt_dsl_frontend = OptDSLFrontend(
    work_dir="/path/to/work_dir",
    random_sample=True,
    random_sample_num=16,
    random_sample_seed=42,
    log_execution_time=True
)

# Execute on datasets
datasets_post_frontend = opt_dsl_frontend.execute_multiple_design_datasets_fine_grained_parallel(
    datasets,
    copy_dataset=True,
    lambda x: f"{x}__post_frontend",
    n_jobs=32,
    cpu_affinity=list(range(32))
)
```

### Intel HLS Support

```python
from hlsfactory.opt_dsl_frontend_intel import OptDSLFrontendIntel

# Intel-specific OptDSL
opt_dsl_frontend_intel = OptDSLFrontendIntel(
    work_dir="/path/to/work_dir",
    random_sample=True,
    random_sample_num=12,
    random_sample_seed=64
)

# Execute with Intel HLS
datasets_intel = opt_dsl_frontend_intel.execute_multiple_design_datasets_fine_grained_parallel(
    datasets,
    copy_dataset=True,
    n_jobs=16
)
```

### Configuration Options

```python
# Full configuration example
opt_dsl_frontend = OptDSLFrontend(
    work_dir="/path/to/work",
    
    # Random sampling configuration
    random_sample=True,                    # Enable random sampling
    random_sample_num=50,                  # Number of samples
    random_sample_seed=42,                 # Reproducible sampling
    
    # Performance tracking
    log_execution_time=True,               # Log execution times
)
```



## Design Space Exploration

### Automatic Parameter Generation

OptDSL automatically generates all combinations of optimization parameters:

```python
# For array partitioning with factors [1,2,4] and types [cyclic,block]
# OptDSL generates 6 combinations:
# (1, cyclic), (1, block), (2, cyclic), (2, block), (4, cyclic), (4, block)
```

### Random Sampling

Large design spaces can be reduced using random sampling:

```python
# Without sampling: 1000+ design points
# With sampling: 50 design points (configurable)
opt_dsl_frontend = OptDSLFrontend(
    work_dir,
    random_sample=True,
    random_sample_num=50,
    random_sample_seed=42  # Reproducible results
)
```

### Design Naming

Generated designs are named using MD5 hashes of optimization parameters:

```
original_design_name + "_" + MD5_hash_of_parameters
```

Example: `atax_a1b2c3d4e5f6`

## Required Files

For OptDSL to work with a design, the following files must be present in a design directory:

- **`opt_template.tcl`**: OptDSL specification and directive templates
