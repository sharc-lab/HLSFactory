import json
from pathlib import Path

import pytest

from hlsfactory.datasets_builtin import (
    DATASET_STR_MAP,
    check_dataset_dir_exists,
    datasets_all_builder,
    datasets_builder,
)
from hlsfactory.utils import (
    get_work_dir,
    log_execution_time_to_file,
    remove_and_make_new_dir_if_exists,
)


def test_load_builtin_datasets_all() -> None:
    top_work_dir = get_work_dir()
    work_dir = top_work_dir / "test_load_builtin_datasets_all"
    remove_and_make_new_dir_if_exists(work_dir)
    datasets = datasets_all_builder(work_dir)
    assert str(datasets)
    assert datasets


def test_load_datasets_from_str() -> None:
    top_work_dir = get_work_dir()
    work_dir = top_work_dir / "test_load_datasets_from_str"
    remove_and_make_new_dir_if_exists(work_dir)
    for dataset_str in DATASET_STR_MAP:
        datasets = datasets_builder(work_dir, [dataset_str])
        assert str(datasets)
        assert datasets

    with pytest.raises(ValueError, match="non_existent_dataset"):
        datasets = datasets_builder(work_dir, ["non_existent_dataset"])


def test_load_datasets_with_labels() -> None:
    top_work_dir = get_work_dir()
    work_dir = top_work_dir / "test_load_datasets_with_labels"
    remove_and_make_new_dir_if_exists(work_dir)

    dataset_names = list(DATASET_STR_MAP.keys())
    dataset_labels = [f"{dset}_labeled" for dset in dataset_names]

    datasets = datasets_builder(work_dir, dataset_names, dataset_labels)

    assert str(datasets)
    assert datasets


def test_check_dataset_dir_exists() -> None:
    top_work_dir = get_work_dir()
    work_dir = top_work_dir / "test_load_datasets_from_str"
    remove_and_make_new_dir_if_exists(work_dir)
    with pytest.raises(FileNotFoundError, match="non_existent_dataset"):
        check_dataset_dir_exists(
            work_dir / "non_existent_dataset",
        )
