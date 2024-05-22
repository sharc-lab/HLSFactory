import os
from pathlib import Path

from hlsfactory.datasets_builtin import (
    datasets_builder,
)
from hlsfactory.flow_vitis import (
    VitisHLSSynthFlow,
)
from hlsfactory.opt_dsl_frontend import OptDSLFrontend
from hlsfactory.utils import (
    DirSource,
    get_work_dir,
    remove_and_make_new_dir_if_exists,
)

### Run Setup Code ###

# Looks for a variable/key in an an .env file called HLSFACTORY_WORK_DIR
WORK_DIR_TOP = get_work_dir(dir_source=DirSource.ENVFILE)
WORK_DIR = WORK_DIR_TOP / "demo_regression_testing"
remove_and_make_new_dir_if_exists(WORK_DIR)

# Number of cores to run stuff in parallel
N_JOBS = 32
CPU_AFFINITY = list(range(N_JOBS))

### Dataset Setup Code ###

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

### OptDSL Frontend with Random Sampling ###

N_RANDOM_SAMPLES = 16
RANDOM_SAMPLE_SEED = 64

opt_dsl_frontend = OptDSLFrontend(
    WORK_DIR,
    random_sample=True,
    random_sample_num=N_RANDOM_SAMPLES,
    random_sample_seed=RANDOM_SAMPLE_SEED,
    log_execution_time=True,
)
datasets_post_frontend = (
    opt_dsl_frontend.execute_multiple_design_datasets_fine_grained_parallel(
        datasets,
        True,
        lambda x: f"{x}_post_frontend",
        n_jobs=N_JOBS,
        cpu_affinity=CPU_AFFINITY,
    )
)


### Tool Versions Setup for Comparison ###

VIVADO_PATHS = {
    "2021_1": Path(
        os.environ["HLSFACTORY_VIVADO_PATH_2021_1"],
    ),  # Path(".../Vivado/2021.1/<install_dir>")
    "2023_1": Path(
        os.environ["HLSFACTORY_VIVADO_PATH_2023_1"],
    ),  # Path(".../Vivado/2023.1/<install_dir>")
}

VITIS_HLS_PATHS = {
    "2021_1": Path(
        os.environ["HLSFACTORY_VITIS_HLS_PATH_2021_1"],
    ),  # Path(".../Vitis_HLS/2021.1/<install_dir>")
    "2023_1": Path(
        os.environ["HLSFACTORY_VITIS_HLS_PATH_2023_1"],
    ),  # Path(".../Vitis_HLS/2023.1/<install_dir>")
}

VITIS_HLS_BINS = {
    version: path / "bin" / "vitis_hls" for version, path in VITIS_HLS_PATHS.items()
}

### Setup Dataset Copies for Different Tool Versions ###

DATASET_VERSIONS = {
    year: {
        dataset_name: dataset.copy_and_rename(
            f"{dataset_name}_post_hls_synth__{year}",
            WORK_DIR,
        )
        for dataset_name, dataset in datasets_post_frontend.items()
    }
    for year, _ in VITIS_HLS_BINS.items()
}


### Execute HLS Synthesis Flows for Different Tool Versions on All Datasets ###

TIMEOUT_HLS_SYNTH = 60.0 * 5  # 5 minutes

for vitis_hls_version, datasets in DATASET_VERSIONS.items():
    vitis_hls_bin = VITIS_HLS_BINS[vitis_hls_version]
    vitis_hls_path = VITIS_HLS_PATHS[vitis_hls_version]
    vivado_path = VIVADO_PATHS[vitis_hls_version]
    toolflow_vitis_hls_synth = VitisHLSSynthFlow(
        vitis_hls_bin=str(vitis_hls_bin),
        env_var_xilinx_hls=str(vitis_hls_path),
        env_var_xilinx_vivado=str(vivado_path),
    )
    datasets_post_hls_synth = (
        toolflow_vitis_hls_synth.execute_multiple_design_datasets_fine_grained_parallel(
            datasets,
            False,
            lambda x: f"{x}_post_hls_synth__{vitis_hls_version}",
            n_jobs=N_JOBS,
            cpu_affinity=CPU_AFFINITY,
            timeout=TIMEOUT_HLS_SYNTH,
        )
    )
