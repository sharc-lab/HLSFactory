import sys
from pathlib import Path

from hlsfactory.data_packaging import DataAggregatorXilinx
from hlsfactory.framework import DesignDataset
from hlsfactory.utils import get_work_dir, remove_and_make_new_dir_if_exists

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
