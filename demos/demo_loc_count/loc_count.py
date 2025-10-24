import json
from pathlib import Path

from hlsfactory.datasets_builtin import (
    datasets_builder,
)
from hlsfactory.flow_vitis import (
    VitisHLSImplFlow,
    VitisHLSImplReportFlow,
    VitisHLSSynthFlow,
)
from hlsfactory.framework import (
    count_total_designs_in_dataset_collection,
)
from hlsfactory.loc_flow import LOCCounterFlow
from hlsfactory.opt_dsl_frontend import OptDSLFrontend
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
WORK_DIR = WORK_DIR_TOP / "demo_loc_count"
remove_and_make_new_dir_if_exists(WORK_DIR)

print(f"Demo LOC Count - Work dir is {WORK_DIR}")

# Number of cores to run stuff in parallel
N_JOBS = 32
CPU_AFFINITY = list(range(N_JOBS))


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


flow_loc = LOCCounterFlow(
    log_execution_time=True,
)

datasets_post_flow_loc = flow_loc.execute_multiple_design_datasets_naive_parallel(
    datasets,
    False,
    n_jobs=1,
)

# for each dataset go into each deisgn and look for the data_loc_count.json file and count up the total loc for each dataset
dataset_loc_count_map = {}
for dataset_name, dataset in datasets_post_flow_loc.items():
    for design in dataset.designs:
        design_dir = design.dir
        json_path = design_dir / "data_loc_count.json"
        if not json_path.exists():
            raise RuntimeError(
                f"Expected LOC count json file at {json_path} but it does not exist"
            )
        json_txt = json_path.read_text()
        data = json.loads(json_txt)
        loc_count_total = data["loc_count_total"]
        if dataset_name not in dataset_loc_count_map:
            dataset_loc_count_map[dataset_name] = 0
        dataset_loc_count_map[dataset_name] += loc_count_total

print("Lines of Code (LOC) count per dataset:")
for dataset_name, loc_count_total in dataset_loc_count_map.items():
    print(f"{dataset_name}: {loc_count_total} LOC")
