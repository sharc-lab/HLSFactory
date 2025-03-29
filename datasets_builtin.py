import shutil
from pathlib import Path

from hlsfactory.framework import DesignDataset, DesignDatasetCollection

CURRENT_FP = Path(__file__).resolve()
CURRENT_DIR = CURRENT_FP.parent

HLS_DATASET_DIR = CURRENT_DIR / "hls_dataset_sources"

DIR_DATASET_POLYBENCH = HLS_DATASET_DIR / "polybench"
DIR_DATASET_MACHSUITE = HLS_DATASET_DIR / "machsuite"
DIR_DATASET_CHSTONE = HLS_DATASET_DIR / "chstone"
DIR_DATASET_PP4FPGAS = HLS_DATASET_DIR / "pp4fpgas"
DIR_DATASET_VITIS_EXAMPLES = HLS_DATASET_DIR / "vitis_examples"
DIR_DATASET_ACCELERATORS = HLS_DATASET_DIR / "accelerators"
DIR_DATASET_Ayush_Saxena_ECE_8893_SPRING_26 = HLS_DATASET_DIR / "ayush_saxena_ece_8893_spring_26"

DIR_ALL = [
    DIR_DATASET_POLYBENCH,
    DIR_DATASET_MACHSUITE,
    DIR_DATASET_CHSTONE,
    DIR_DATASET_PP4FPGAS,
    DIR_DATASET_VITIS_EXAMPLES,
    DIR_DATASET_ACCELERATORS,
]


def check_dataset_dir_exists(dir_path: Path) -> None:
    if not dir_path.exists():
        msg = f"Dataset directory not found: {dir_path}"
        raise FileNotFoundError(msg)


def dataset_polybench_builder(name: str, work_dir: Path) -> DesignDataset:
    check_dataset_dir_exists(DIR_DATASET_POLYBENCH)
    new_dir = work_dir / name
    shutil.copytree(DIR_DATASET_POLYBENCH, new_dir)
    return DesignDataset.from_dir(name, new_dir)


def dataset_machsuite_builder(name: str, work_dir: Path) -> DesignDataset:
    check_dataset_dir_exists(DIR_DATASET_MACHSUITE)
    new_dir = work_dir / name
    shutil.copytree(DIR_DATASET_MACHSUITE, new_dir)
    return DesignDataset.from_dir(
        name,
        new_dir,
        exclude_dir_filter=lambda dir_path: dir_path.name == "common",
    )


def dataset_chstone_builder(name: str, work_dir: Path) -> DesignDataset:
    check_dataset_dir_exists(DIR_DATASET_CHSTONE)
    new_dir = work_dir / name
    shutil.copytree(DIR_DATASET_CHSTONE, new_dir)
    return DesignDataset.from_dir(name, new_dir)


def dataset_pp4fpgas_builder(name: str, work_dir: Path) -> DesignDataset:
    check_dataset_dir_exists(DIR_DATASET_PP4FPGAS)
    new_dir = work_dir / name
    shutil.copytree(DIR_DATASET_PP4FPGAS, new_dir)
    return DesignDataset.from_dir(name, new_dir)


def dataset_vitis_examples_builder(name: str, work_dir: Path) -> DesignDataset:
    check_dataset_dir_exists(DIR_DATASET_VITIS_EXAMPLES)
    new_dir = work_dir / name
    shutil.copytree(DIR_DATASET_VITIS_EXAMPLES, new_dir)
    return DesignDataset.from_dir(name, new_dir)


def dataset_accelerators_builder(name: str, work_dir: Path) -> DesignDataset:
    check_dataset_dir_exists(DIR_DATASET_ACCELERATORS)
    new_dir = work_dir / name
    shutil.copytree(DIR_DATASET_ACCELERATORS, new_dir)
    return DesignDataset.from_dir(name, new_dir)

def dataset_dataset_ayush_saxena_ece_8893_spring_26_builder_builder(name: str, work_dir: Path) -> DesignDataset:
    check_dataset_dir_exists(DIR_DATASET_Ayush_Saxena_ECE_8893_SPRING_26)
    new_dir = work_dir / name
    shutil.copytree(DIR_DATASET_ACCELERATORS, new_dir)
    return DesignDataset.from_dir(name, new_dir)


DATASET_STR_MAP = {
    "polybench": dataset_polybench_builder,
    "machsuite": dataset_machsuite_builder,
    "chstone": dataset_chstone_builder,
    "pp4fpgas": dataset_pp4fpgas_builder,
    "vitis_examples": dataset_vitis_examples_builder,
    "accelerators": dataset_accelerators_builder,
    "ayush_saxena_ece_8893_spring_26": dataset_dataset_ayush_saxena_ece_8893_spring_26_builder_builder
}


def datasets_builder(
    work_dir: Path,
    dataset_names: list[str],
    dataset_labels: list[str] | None = None,
) -> DesignDatasetCollection:
    datasets = {}
    if dataset_labels is None:
        dataset_labels = dataset_names

    assert len(dataset_names) == len(dataset_labels)
    for dataset_name, dataset_label in zip(
        dataset_names,
        dataset_labels,
        strict=True,
    ):
        dataset_normalized = dataset_name.strip().lower()
        if dataset_normalized not in DATASET_STR_MAP:
            raise ValueError(
                f"Dataset name not recognized: {dataset_name} "
                f"(normalized: {dataset_normalized})",
            )
        dataset_builder = DATASET_STR_MAP[dataset_normalized]
        dataset = dataset_builder(dataset_label, work_dir)
        datasets[dataset_label] = dataset
    return datasets


def datasets_all_builder(work_dir: Path) -> DesignDatasetCollection:
    for dir_path in DIR_ALL:
        check_dataset_dir_exists(dir_path)
    dataset_polybench = dataset_polybench_builder("polybench", work_dir)
    dataset_machsuite = dataset_machsuite_builder("machsuite", work_dir)
    dataset_chstone = dataset_chstone_builder("chstone", work_dir)
    dataset_pp4fpgas = dataset_pp4fpgas_builder("pp4fpgas", work_dir)
    dataset_vitis_examples = dataset_vitis_examples_builder("vitis_examples", work_dir)
    dataset_accelerators = dataset_accelerators_builder("accelerators", work_dir)
    return {
        dataset_polybench.name: dataset_polybench,
        dataset_machsuite.name: dataset_machsuite,
        dataset_chstone.name: dataset_chstone,
        dataset_pp4fpgas.name: dataset_pp4fpgas,
        dataset_vitis_examples.name: dataset_vitis_examples,
        dataset_accelerators.name: dataset_accelerators,
    }
