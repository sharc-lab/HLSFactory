from pathlib import Path

from hlsfactory.data_packaging import DataAggregatorXilinx
from hlsfactory.framework import DesignDataset
from hlsfactory.utils import get_work_dir, remove_and_make_new_dir_if_exists

CURRENT_DIR = Path(__file__).parent
DATA_DIR = CURRENT_DIR / "data"
if not DATA_DIR.exists():
    DATA_DIR.mkdir()

test_data_dir = Path(
    "/usr/scratch/skaram7/hlsdataset_workdir_design_space_v2/",
)

dataset_polybench = DesignDataset.from_dir(
    "polybench_xilinx__post_frontend",
    test_data_dir / "polybench_xilinx__post_frontend",
)
dataset_machsuite = DesignDataset.from_dir(
    "machsuite_xilinx__post_frontend",
    test_data_dir / "machsuite_xilinx__post_frontend",
)
dataset_chstone = DesignDataset.from_dir(
    "chstone_xilinx__post_frontend",
    test_data_dir / "chstone_xilinx__post_frontend",
)

designs = (
    dataset_polybench.designs + dataset_machsuite.designs + dataset_chstone.designs
)

xilinx_aggregator = DataAggregatorXilinx()

data = xilinx_aggregator.gather_multiple_designs(designs, n_jobs=32)
output_archive_fp = DATA_DIR / "data.zip"
xilinx_aggregator.aggregated_data_to_archive(
    data,
    output_archive_fp,
)
print(output_archive_fp)
