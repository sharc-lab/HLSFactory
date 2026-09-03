# import sys
# from pathlib import Path

# from hlsfactory.data_packaging import DataAggregatorXilinx
# from hlsfactory.framework import DesignDataset
# from hlsfactory.utils import get_work_dir, remove_and_make_new_dir_if_exists

# test_data_dir = Path(
#     "/usr/scratch/skaram7/hlsdataset_workdir_design_space_v2/polybench_xilinx__post_frontend",
# )

# dataset = DesignDataset.from_dir("polybench_xilinx__post_frontend", test_data_dir)


# def test_data_aggregation() -> None:
#     top_work_dir = get_work_dir()
#     work_dir = top_work_dir / "aggr_tests"
#     remove_and_make_new_dir_if_exists(work_dir)

#     xilinx_aggregator = DataAggregatorXilinx()

#     data = xilinx_aggregator.gather_multiple_designs(dataset.designs, n_jobs=16)
#     output_archive_fp = work_dir / "data.zip"
#     xilinx_aggregator.aggregated_data_to_archive(
#         data,
#         output_archive_fp,
#     )
#     print(output_archive_fp)


from pathlib import Path

from hlsfactory.data_packaging import DataAggregatorXilinx
from hlsfactory.framework import Design
from hlsfactory.utils import (
    ExecutionDataStatus,
    update_execution_data_with_flow_results,
)


def test_gather_execution_data_uses_typed_schema(tmp_path: Path) -> None:
    design = Design("test_design", tmp_path)
    update_execution_data_with_flow_results(
        design_dir=tmp_path,
        flow_name="VitisHLSSynthFlow",
        status=ExecutionDataStatus.SUCCESS,
        t_start=1.0,
        t_end=3.0,
        core=2,
    )

    assert DataAggregatorXilinx().gather_execution_data(design) == {
        "VitisHLSSynthFlow": {
            "status": "success",
            "t_start": 1.0,
            "t_end": 3.0,
            "dt": 2.0,
            "core": 2,
            "error_message": None,
        }
    }


def test_failed_execution_data_prevents_artifact_collection(tmp_path: Path) -> None:
    design = Design("test_design", tmp_path)
    update_execution_data_with_flow_results(
        design_dir=tmp_path,
        flow_name="VitisHLSSynthFlow",
        status=ExecutionDataStatus.ERROR,
        t_start=1.0,
        t_end=2.0,
        error_message="Synthesis failed",
    )

    assert DataAggregatorXilinx().gather_hls_synthesis_artifacts_data(design) == {}
