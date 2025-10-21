# End-to-End Xilinx Flow

This tutorial demonstrates an end-to-end workflow for dataset setup, frontend elaboration, HLS synthesis, implementation, and reporting using the Xilinx Vitis HLS and Vivado tools.

This demo can be found as a single script and as a Jupyter notebook in the `demos/demo_full_flow_xilinx` directory of the HLSFactory repository.

## Setup

First, we need to set up some basic configurations such as the working directory where designs will be stored and built, the number of cores to run jobs in parallel, and the paths to the Xilinx Vitis HLS and Vivado tools.

```python
from hlsfactory.utils import (
    DirSource,
    ToolPathsSource,
    get_tool_paths,
    get_work_dir,
    remove_and_make_new_dir_if_exists,
)

# Looks for a variable/key in an .env file called HLSFACTORY_WORK_DIR
WORK_DIR_TOP = get_work_dir(dir_source=DirSource.ENVFILE)
WORK_DIR = WORK_DIR_TOP / "demo_full_flow_xilinx"
remove_and_make_new_dir_if_exists(WORK_DIR)

# Number of cores to run jobs in parallel
N_JOBS = 32
CPU_AFFINITY = list(range(N_JOBS))

# Looks for variables/keys in an .env file called HLSFACTORY_VITIS_HLS_PATH and HLSFACTORY_VIVADO_PATH
VITIS_HLS_PATH, VIVADO_PATH = get_tool_paths(tool_paths_source=ToolPathsSource.ENVFILE)
VIVADO_BIN = VIVADO_PATH / "bin" / "vivado"
VITIS_HLS_BIN = VITIS_HLS_PATH / "bin" / "vitis_hls"
```

HLSFactory provides helper functions to set up the working directory and tool paths. The `get_work_dir` function looks for a variable/key in an `.env` file called `HLSFACTORY_WORK_DIR`. The `get_tool_paths` function looks for variables/keys in an `.env` file called `HLSFACTORY_VITIS_HLS_PATH` and `HLSFACTORY_VIVADO_PATH`. These helper functions can also search in other places for these variables; see the [](hlsfactory.utils) module API documentation for more information.

For this demo, please set the `HLSFACTORY_WORK_DIR`, `HLSFACTORY_VITIS_HLS_PATH`, and `HLSFACTORY_VIVADO_PATH` variables in a local `.env` file to the paths of your working directory where you want to store and build the designs, the path to the Xilinx Vitis HLS tool, and the path to the Xilinx Vivado tool. You can also modify the code to define these paths directly in the script as `pathlib.Path` objects.

## Dataset Setup

We will now set up the datasets using the `datasets_builder` function.

```python
from hlsfactory.datasets_builtin import datasets_builder
from hlsfactory.framework import count_total_designs_in_dataset_collection

datasets = datasets_builder(
    WORK_DIR,
    [
        "polybench",
        "machsuite",
        "chstone",
    ],
    dataset_labels=[
        "polybench_xilinx",
        "machsuite_xilinx",
        "chstone_xilinx",
    ],
)

total_count = count_total_designs_in_dataset_collection(datasets)
print(f"Total Designs: {total_count}")
```

The `datasets_builder` function creates a dataset collection with the specified datasets. The HLS source code for the designs in these datasets is built into the HLSFactory package itself, so no other external downloads or setup are needed to load these datasets. The `dataset_labels` argument is optional and can be used to assign user-specified names to the datasets rather than the default names. Datasets are loaded by a list of keys that correspond to the datasets in the `datasets_builtin` module. For more information about the datasets and the keys, see the [](../built_in_datasets.md) page as well as the [](hlsfactory.datasets_builtin) module API documentation.

In this demo, we load the Polybench, MachSuite, and CHStone datasets built into HLSFactory.

After this step, the working directory looks as follows:

```text
- WORK_DIR/
    - polybench_xilinx/
        - <design_0>/
            - <design files...>
        - <design_1>/
        - ...
    - machsuite_xilinx/
        - ...
    - chstone_xilinx/
        - ...
```

## Frontend for Design Space Elaboration

Next, we use the `OptDSLFrontend` for design space elaboration.

```python
from hlsfactory.opt_dsl_frontend import OptDSLFrontend

N_RANDOM_SAMPLES = 12
RANDOM_SAMPLE_SEED = 64

opt_dsl_frontend = OptDSLFrontend(
    WORK_DIR,
    random_sample=True,
    random_sample_num=N_RANDOM_SAMPLES,
    random_sample_seed=RANDOM_SAMPLE_SEED,
    log_execution_time=True,
)
datasets_post_frontend = opt_dsl_frontend.execute_multiple_design_datasets_fine_grained_parallel(
    datasets,
    True,
    lambda x: f"{x}__post_frontend",
    n_jobs=N_JOBS,
    cpu_affinity=CPU_AFFINITY,
)

total_count_post_frontend = count_total_designs_in_dataset_collection(
    datasets_post_frontend,
)

print(f"Total Designs post-frontend: {total_count_post_frontend}")
```

This step will take the designs in the datasets and generate many different variations of each design based on the specified design space. This is done by specifying what pragmas can be inserted into the design and the different possible values for pragma parameters.

For example, you can specify a design space to include three loop unroll pragmas for three different loops. For each pragma, you can set a range of values for the unroll factor. The `OptDSLFrontend` can then look at this design space, sample different combinations of enabling loop unrolling and the unrolling factor, and generate a new design for each combination. Currently, the `OptDSLFrontend` supports pipelining, loop unrolling, and array partitioning for Xilinx Vitis HLS and Intel HLS flows.

In most cases, enumerating designs for the entire design space can result in hundreds or thousands of design variations for a single design. For some users, this is overkill and may be limited by the amount of compute and time they have to run HLS synthesis and implementation. Therefore, the `OptDSLFrontend` has an option to sample a fixed random subset of the design space. This is done by setting the `random_sample` flag to `True` and specifying the number of random samples to take with the `random_sample_num` argument. The `random_sample_seed` argument is used to set the seed for the random number generator to ensure reproducibility.

Note that the built-in datasets have pre-defined design spaces that are used by the `OptDSLFrontend` to generate new designs. Not all built-in datasets have design spaces defined for them, but common HLS benchmarks like Polybench, MachSuite, and CHStone do have design spaces defined. Also, note that this step is not strictly necessary for HLS synthesis in the general HLSFactory framework. You can always run HLS synthesis and implementation on designs as they are or add prepared vendor-specific designs to the datasets.

The main motivation of a frontend like OptDSL is to expand and diversify the final generated dataset of HLS designs. In the end, this will result in a larger and more robust dataset for applications such as machine learning and benchmarking.

After this step, the working directory looks as follows:

```text
- WORK_DIR/
    - polybench_xilinx/
        - ...
    - machsuite_xilinx/
        - ...
    - chstone_xilinx/
        - ...
    - polybench_xilinx__post_frontend/
        - <design_0_sampled_0>/
            - <design files...>
        - <design_0_sampled_1>/
        - ...
        - <design_1_sampled_0>/
        - ...
    - machsuite_xilinx__post_frontend/
        - ...
    - chstone_xilinx__post_frontend/
        - ...
```

This is because the `OptDSLFrontend` was set up to copy the datasets and add a `__post_frontend` suffix to the dataset labels. The frontend then generates new designs in these copied datasets.

## HLS Synthesis and Implementation Flows

We proceed with the HLS synthesis and implementation flows.

```python
from hlsfactory.flow_vitis import (
    VitisHLSImplFlow,
    VitisHLSImplReportFlow,
    VitisHLSSynthFlow,
)

# Timeout for HLS synthesis and Vivado implementation
TIMEOUT_HLS_SYNTH = 60.0 * 8  # 8 minutes
TIMEOUT_HLS_IMPL = 60.0 * 30  # 30 minutes

total_time_estimation = (
    total_count_post_frontend * (TIMEOUT_HLS_SYNTH + TIMEOUT_HLS_IMPL) / N_JOBS
)
print(
    f"Estimated worst-case build time:\n{total_time_estimation} seconds\n{total_time_estimation / 60} minutes\n{total_time_estimation / 3600} hours",
)

toolflow_vitis_hls_synth = VitisHLSSynthFlow(
    vitis_hls_bin=str(VITIS_HLS_BIN),
    env_var_xilinx_hls=str(VITIS_HLS_PATH),
    env_var_xilinx_vivado=str(VIVADO_PATH),
)
datasets_post_hls_synth = toolflow_vitis_hls_synth.execute_multiple_design_datasets_fine_grained_parallel(
    datasets_post_frontend,
    False,
    n_jobs=N_JOBS,
    cpu_affinity=CPU_AFFINITY,
    timeout=TIMEOUT_HLS_SYNTH,
)

toolflow_vitis_hls_implementation = VitisHLSImplFlow(
    vitis_hls_bin=str(VITIS_HLS_BIN),
    env_var_xilinx_hls=str(VITIS_HLS_PATH),
    env_var_xilinx_vivado=str(VIVADO_PATH),
)
datasets_post_hls_implementation = toolflow_vitis_hls_implementation.execute_multiple_design_datasets_fine_grained_parallel(
    datasets_post_hls_synth,
    False,
    n_jobs=N_JOBS,
    cpu_affinity=CPU_AFFINITY,
    timeout=TIMEOUT_HLS_IMPL,
)
```

There are two main flow classes for the Xilinx Vitis HLS tool: `VitisHLSSynthFlow` and `VitisHLSImplFlow`.

The `VitisHLSSynthFlow` class is used to run HLS synthesis on a design. It looks for a user-defined entry point Tcl script in the top level of the design directory with the name `dataset_hls.tcl`. This script must be defined by the user to take a design, set up a Vitis HLS project, add the source files to the project, create a single solution, and run `csynth` on the solution successfully. The flow will call this entry point using Vitis HLS and then look for the resulting HLS project and solution files in the design directory.

The `VitisHLSImplFlow` class is used to run HLS implementation on a design. It works similarly to the synthesis flow with its own entry point Tcl script called `dataset_hls_ip_export.tcl`. This script must be defined by the user to open an existing project, open an existing synthesized solution, and then call the Vitis HLS `export_design` command with the argument `-flow impl` to export the synthesized design to Vivado and run the Vivado implementation flow.

In practice, we have already implemented these entry points and scripts for the built-in datasets. The `VitisHLSSynthFlow` and `VitisHLSImplFlow` classes will automatically run the correct entry point scripts for the built-in datasets.

If a user is loading their own design into an HLSFactory-based workflow, it is the user's responsibility to define these entry point scripts correctly to implement the expected functionality of the flow. However, we include some file and source code checks along with exceptions and error messages to help users debug their design integration with these flows.

We are further working on documenting how to use these entry point scripts and how to integrate new designs into the HLSFactory framework.

To make use of parallelism, all flows implement the `execute_multiple_design_datasets_fine_grained_parallel` function. This function accepts a dataset collection and returns a dataset collection. It allows for fine-grained parallelism across designs in different datasets. We recommend this as the best way to run compute-intensive flows in parallel.

Flows can accept a timeout argument to limit the amount of time a design can run. This can be useful because in some cases it is possible to create buggy designs using certain pragmas or pragma combinations that can cause HLS synthesis to run indefinitely. The timeout argument can be used to kill the HLS synthesis or implementation process if it runs for too long. The timeout is specified in seconds. When executing a flow on multiple designs and one of the designs times out, the output will be a dataset collection with all the designs that did not time out, excluding any design that did time out. The flows that time out also leave a sentinel file in the design directory to help with debugging.

## Vivado Reporting Flow

Finally, we run the Vivado reporting flow.

```python
toolflow_vitis_hls_impl_report = VitisHLSImplReportFlow(
    vitis_hls_bin=str(VITIS_HLS_BIN),
    vivado_bin=str(VIVADO_BIN),
    env_var_xilinx_hls=str(VITIS_HLS_PATH),
    env_var_xilinx_vivado=str(VIVADO_PATH),
)
toolflow_vitis_hls_impl_report.execute_multiple_design_datasets_fine_grained_parallel(
    datasets_post_hls_implementation,
    False,
    n_jobs=N_JOBS,
    cpu_affinity=CPU_AFFINITY,
)
```

This is a reporting flow that calls Vivado on an already implemented design and runs the reporting commands to report resource usage, timing information, and power estimations for an implemented design. The flow also has logic that extracts the data from the generated raw reports and processes it into a structured JSON file in the design directory.

## Generated Data

After running all the above code, the designs now reside on disk in the working directory. Each design has a synthesized HLS design from Vitis HLS along with an implemented design from Vivado.

When running the flows, HLSFactory will process and aggregate data into data files in the top-level directory of each design.

The `VitisHLSSynthFlow` generates a `data_design.json` file that contains metadata about the design parsed from Vitis HLS, including the name, part, target clock period, and version of the Vitis HLS tool used. The flow also parses the generated HLS report after synthesis and generates a `data_hls.json` file. This data includes clock period, estimated latency (best, average, and worst case), and the estimated resource usage (DSP, FF, LUT, BRAM, and URAM).

The `VitisHLSImplFlow` generates a `data_implementation.json` file that contains data from various Vivado reports of the fully implemented design. This includes estimated power (total, static, dynamic), resource usage (Total LUT, Logic LUT, LUTRAM, SRL, FF, BRAM, URAM, DSP), and timing information (WNS, TNS, WHS, THS, WPWS, TPWS, clock period, and clock frequency).

Additionally, any time a flow is run, its execution data is logged to a common `execution_time_data.json` file in the design directory. When a flow is run, it finds the existing `execution_time_data.json` file (or creates it if it does not exist) and appends the execution time of the current flow and the core ID the flow ran on to the file.

This data can be manually aggregated by the user, but HLSFactory also provides facilities to aggregate and process this data into data tables and archived datasets along with automatically extracting build artifacts. See the [](./data_agg.md) tutorial to explore these features.
