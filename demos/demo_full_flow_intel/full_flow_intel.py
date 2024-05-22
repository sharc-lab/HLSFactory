from hlsfactory.datasets_builtin import (
    datasets_builder,
)
from hlsfactory.flow_intel import IntelHLSSynthFlow, IntelQuartusImplFlow
from hlsfactory.opt_dsl_frontend_intel import OptDSLFrontendIntel
from hlsfactory.utils import (
    DirSource,
    get_work_dir,
    remove_and_make_new_dir_if_exists,
)

### Demo Setup Code ###

# Looks for a variable/key in an an .env file called HLSFACTORY_WORK_DIR
WORK_DIR_TOP = get_work_dir(dir_source=DirSource.ENVFILE)
WORK_DIR = WORK_DIR_TOP / "demo_full_flow_intel"
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
        "polybench_intel",
        "machsuite_intel",
        "chstone_intel",
    ],
)

### OptDSL Frontend ###

N_RANDOM_SAMPLES = 12

opt_dsl_frontend_intel = OptDSLFrontendIntel(
    WORK_DIR,
    random_sample=True,
    random_sample_num=N_RANDOM_SAMPLES,
)

designs_after_frontend = (
    opt_dsl_frontend_intel.execute_multiple_design_datasets_fine_grained_parallel(
        datasets,
        True,
        lambda x: f"{x}__post_frontend",
        n_jobs=N_JOBS,
        cpu_affinity=CPU_AFFINITY,
    )
)

### HLS Synthesis and Implementation ###

toolflow_intel_hls_synth = IntelHLSSynthFlow(
    ipp_bin=None,  # Will automatically find the i++ binary path from the PATH variable
    arch="1ST110EN1F43E1VG",  # Stratix 10
    clock="10ns",
)
toolflow_intel_impl_synth = IntelQuartusImplFlow(
    quartus_bin=None,  # Will automatically find the quartus_sh binary path from the PATH variable
)

datasets_post_hls_synth = (
    toolflow_intel_hls_synth.execute_multiple_design_datasets_fine_grained_parallel(
        designs_after_frontend,
        False,
        n_jobs=N_JOBS,
        cpu_affinity=CPU_AFFINITY,
    )
)

datasets_post_hls_implementation = (
    toolflow_intel_impl_synth.execute_multiple_design_datasets_fine_grained_parallel(
        datasets_post_hls_synth,
        False,
        n_jobs=N_JOBS,
        cpu_affinity=CPU_AFFINITY,
    )
)
