import shutil
from collections.abc import Callable
from pathlib import Path
from unittest.mock import patch

import pytest

from hlsfactory.datasets_builtin import (
    DATASET_STR_MAP,
    T_dataset_builder,
    dataset_polybench_builder,
    dataset_vitis_examples_builder,
)
from hlsfactory.flow_vitis import (
    VitisHLSImplFlow,
    VitisHLSImplReportFlow,
    VitisHLSSynthFlow,
)
from hlsfactory.framework import (
    count_total_designs_in_dataset_collection,
)
from hlsfactory.opt_dsl_frontend import OptDSLFrontend
from hlsfactory.utils import (
    ToolPathsSource,
    get_tool_paths,
    get_work_dir,
    remove_and_make_new_dir_if_exists,
)

top_work_dir = get_work_dir()

PATH_VITIS_HLS, PATH_VIVADO = get_tool_paths(ToolPathsSource.ENVFILE)
BIN_VITIS_HLS = PATH_VITIS_HLS / "bin" / "vitis_hls"
BIN_VIVADO = PATH_VIVADO / "bin" / "vivado"

N_JOBS = 16
CPU_AFFINITY = list(range(N_JOBS))

TIMEOUT_HLS_SYNTH = 60.0 * 8  # 8 minutes
TIMEOUT_HLS_IMPL = 60.0 * 30  # 30 minutes


DATASETS = DATASET_STR_MAP
DATASETS_ITEMS = list(DATASETS.items())


@pytest.mark.parametrize("dataset", DATASETS_ITEMS, ids=lambda x: x[0])
def test_concrete_vitis_hls_flow_paramaterized(
    dataset: tuple[str, T_dataset_builder],
) -> None:
    dataset_name, dataset_builder = dataset
    work_dir = (
        top_work_dir / "test_flows" / f"test_concrete_vitis_hls_flow__{dataset_name}"
    )
    remove_and_make_new_dir_if_exists(work_dir)
    print(f"work_dir: {work_dir}")

    dataset_instance = dataset_builder(dataset_name, work_dir)

    datasets = {
        dataset_name: dataset_instance,
    }

    total_count = count_total_designs_in_dataset_collection(datasets)

    total_synth_time_estimation = (
        total_count * (TIMEOUT_HLS_SYNTH + TIMEOUT_HLS_IMPL) / N_JOBS
    )

    print(f"total_count: {total_count}")
    print(f"total_synth_time_estimation: {total_synth_time_estimation}")

    assert total_count == len(dataset_instance.designs)

    toolflow_vitis_hls_synth = VitisHLSSynthFlow(
        vitis_hls_bin=str(BIN_VITIS_HLS),
        env_var_xilinx_hls=str(PATH_VITIS_HLS),
        env_var_xilinx_vivado=str(PATH_VIVADO),
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

    assert datasets_post_hls_synth
