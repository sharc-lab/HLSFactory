import shutil

from hlsfactory.datasets_builtin import datasets_builder
from hlsfactory.flow_lightningsim import LightningSimFlow
from hlsfactory.flow_vitis import VitisHLSCosimFlow, VitisHLSSynthFlow
from hlsfactory.framework import DesignDataset, count_total_designs_in_dataset_collection
from hlsfactory.utils import (
    DirSource,
    ToolPathsSource,
    get_tool_paths,
    get_work_dir,
    remove_and_make_new_dir_if_exists,
)

### Run Setup Code ###

# Looks for a variable/key in an an .env file called HLSFACTORY_WORK_DIR
WORK_DIR_TOP = get_work_dir(dir_source=DirSource.ENVFILE)
WORK_DIR = WORK_DIR_TOP / "demo_sim"
remove_and_make_new_dir_if_exists(WORK_DIR)

# Number of cores to run stuff in parallel
N_JOBS = 32
CPU_AFFINITY = list(range(N_JOBS))

# Looks for variables/keys in an an .env file called HLSFACTORY_VITIS_HLS_PATH and HLSFACTORY_VIVADO_PATH
VITIS_HLS_PATH, VIVADO_PATH = get_tool_paths(tool_paths_source=ToolPathsSource.ENVFILE)
VITIS_HLS_BIN = VITIS_HLS_PATH / "bin" / "vitis_hls"


### Dataset Setup Code ###

# Case Study 7: Comparing Estimated (C-Synth) and Actual (Co-Sim) design
# latency against LightningSim's simulated latency, using the Stream-HLS
# designs (each of which already has csynth/csim/cosim/lightningsim flow
# configs set up in its hlsfactory.toml).
datasets_all_opt_levels = datasets_builder(
    WORK_DIR,
    ["stream_hls"],
    dataset_labels=["stream_hls"],
)

# Only the opt5 variant of each design is used for this case study.
dataset_stream_hls_all = datasets_all_opt_levels["stream_hls"]
opt5_designs = [
    design for design in dataset_stream_hls_all.designs if design.name.endswith("__opt5")
]
non_opt5_designs = [
    design
    for design in dataset_stream_hls_all.designs
    if not design.name.endswith("__opt5")
]
for design in non_opt5_designs:
    shutil.rmtree(design.dir)

datasets = {
    "stream_hls": DesignDataset(
        "stream_hls",
        dataset_stream_hls_all.dataset_dir,
        opt5_designs,
    ),
}

total_count = count_total_designs_in_dataset_collection(datasets)
print(f"Total Designs: {total_count}")


### C Synthesis Flow ###
# Produces data_hls.json (synthesis-estimated latency) and data_design.json
# for every design, and creates the solution directory LightningSim needs.

TIMEOUT_HLS_SYNTH = 60.0 * 15  # 15 minutes

toolflow_vitis_hls_synth = VitisHLSSynthFlow(
    vitis_hls_bin=str(VITIS_HLS_BIN),
    env_var_xilinx_hls=str(VITIS_HLS_PATH),
    env_var_xilinx_vivado=str(VIVADO_PATH),
)
datasets_post_hls_synth = (
    toolflow_vitis_hls_synth.execute_multiple_design_datasets_fine_grained_parallel(
        datasets,
        False,
        n_jobs=N_JOBS,
        cpu_affinity=CPU_AFFINITY,
        timeout=TIMEOUT_HLS_SYNTH,
    )
)


### C/RTL Co-Simulation Flow ###
# Produces data_cosim.json with the ground-truth, cycle-accurate latency
# for every design (used as the baseline the other two columns are compared
# against in the case study table).

TIMEOUT_HLS_COSIM = 60.0 * 60  # 1 hour

toolflow_vitis_hls_cosim = VitisHLSCosimFlow(
    vitis_hls_bin=str(VITIS_HLS_BIN),
    env_var_xilinx_hls=str(VITIS_HLS_PATH),
    env_var_xilinx_vivado=str(VIVADO_PATH),
)
datasets_post_cosim = (
    toolflow_vitis_hls_cosim.execute_multiple_design_datasets_fine_grained_parallel(
        datasets_post_hls_synth,
        False,
        n_jobs=N_JOBS,
        cpu_affinity=CPU_AFFINITY,
        timeout=TIMEOUT_HLS_COSIM,
    )
)


### LightningSim Baseline Simulation Flow ###
# Produces data_lightningsim.json with LightningSim's simulated latency for
# each design's default (as-synthesized) FIFO sizing.

toolflow_lightningsim = LightningSimFlow()
datasets_post_lightningsim = (
    toolflow_lightningsim.execute_multiple_design_datasets_fine_grained_parallel(
        datasets_post_cosim,
        False,
        n_jobs=N_JOBS,
        cpu_affinity=CPU_AFFINITY,
    )
)

total_count_post_lightningsim = count_total_designs_in_dataset_collection(
    datasets_post_lightningsim,
)
print(f"Total Designs: {total_count}")
print(f"Total Designs post-lightningsim: {total_count_post_lightningsim}")
