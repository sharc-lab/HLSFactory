import json
import os
import subprocess
import unittest
from dataclasses import dataclass
from pathlib import Path
from unittest.mock import mock_open, patch

import pytest

from hlsfactory.utils import (
    CallToolResult,
    DirSource,
    ToolPathsSource,
    call_tool,
    find_bin_path,
    get_tool_paths,
    get_work_dir,
    log_execution_time_to_file,
    remove_and_make_new_dir_if_exists,
    serialize_methods_for_dataclass,
    timeout_not_supported,
)

top_work_dir = get_work_dir()
work_dir = top_work_dir / "test_call_tool"
remove_and_make_new_dir_if_exists(work_dir)


def test_find_bin_path() -> None:
    bin_path_str = find_bin_path("echo")
    assert bin_path_str
    assert Path(bin_path_str).exists()

    with pytest.raises(RuntimeError, match="Could not find"):
        find_bin_path("non_existent_tool_that_definitely_does_not_exist")


def test_serialize_methods_for_dataclass() -> None:
    @serialize_methods_for_dataclass
    @dataclass
    class TestDataclass:
        a: int
        b: str

    test = TestDataclass(1, "test")
    json_path = work_dir / "test.json"
    yaml_path = work_dir / "test.yaml"

    test.to_json(json_path)
    test.to_yaml(yaml_path)

    test_from_json = TestDataclass.from_json(json_path)
    test_from_yaml = TestDataclass.from_yaml(yaml_path)

    assert test == test_from_json
    assert test == test_from_yaml

    with pytest.raises(TypeError, match="Decorated class must be a dataclass."):

        @serialize_methods_for_dataclass
        class TestNormalClass:
            pass


def test_timeout_not_supported() -> None:
    with pytest.raises(
        RuntimeError,
        match="Timeout not supported for the current flow:",
    ):
        timeout_not_supported("test_flow")


def test_get_work_dir_envfile() -> None:
    # patch this function dotenv.dotenv_values()
    with patch(
        "dotenv.dotenv_values",
        return_value={"HLSFACTORY_WORK_DIR": "/non_existent_dir/my_work_dir"},
    ):
        assert get_work_dir(dir_source=DirSource.ENVFILE)

    with (
        patch(
            "dotenv.dotenv_values",
            return_value={},
        ),
        pytest.raises(ValueError, match="HLSFACTORY_WORK_DIR not in .env file"),
    ):
        get_work_dir(dir_source=DirSource.ENVFILE)


def test_get_work_dir_env() -> None:
    os.environ["HLSFACTORY_WORK_DIR"] = "/non_existent_dir/my_work_dir"
    assert get_work_dir(dir_source=DirSource.ENV)
    del os.environ["HLSFACTORY_WORK_DIR"]
    with pytest.raises(ValueError, match="HLSFACTORY_WORK_DIR not in environment"):
        get_work_dir(dir_source=DirSource.ENV)


def test_get_work_dir_temp() -> None:
    assert get_work_dir(dir_source=DirSource.TEMP)


def test_get_work_dir_fallthrough() -> None:
    with pytest.raises(ValueError, match="Invalid dir_source"):
        get_work_dir(dir_source=None)


def test_get_tool_paths_envfile() -> None:
    with patch(
        "dotenv.dotenv_values",
        return_value={
            "HLSFACTORY_VITIS_HLS_PATH": "/non_existent_dir/vitis_hls",
            "HLSFACTORY_VIVADO_PATH": "/non_existent_dir/vivado",
        },
    ):
        assert get_tool_paths(tool_paths_source=ToolPathsSource.ENVFILE)

    with (
        patch(
            "dotenv.dotenv_values",
            return_value={"HLSFACTORY_VIVADO_PATH": "/non_existent_dir/vivado"},
        ),
        pytest.raises(ValueError, match="HLSFACTORY_VITIS_HLS_PATH not in .env file"),
    ):
        get_tool_paths(tool_paths_source=ToolPathsSource.ENVFILE)

    with (
        patch(
            "dotenv.dotenv_values",
            return_value={"HLSFACTORY_VITIS_HLS_PATH": "/non_existent_dir/vitis_hls"},
        ),
        pytest.raises(ValueError, match="HLSFACTORY_VIVADO_PATH not in .env file"),
    ):
        get_tool_paths(tool_paths_source=ToolPathsSource.ENVFILE)


def test_get_tool_paths_env() -> None:
    os.environ["HLSFACTORY_VITIS_HLS_PATH"] = "/non_existent_dir/vitis_hls"
    os.environ["HLSFACTORY_VIVADO_PATH"] = "/non_existent_dir/vivado"
    assert get_tool_paths(tool_paths_source=ToolPathsSource.ENV)
    del os.environ["HLSFACTORY_VITIS_HLS_PATH"]
    with pytest.raises(
        ValueError,
        match="HLSFACTORY_VITIS_HLS_PATH not in environment",
    ):
        get_tool_paths(tool_paths_source=ToolPathsSource.ENV)
    os.environ["HLSFACTORY_VITIS_HLS_PATH"] = "/non_existent_dir/vitis_hls"
    del os.environ["HLSFACTORY_VIVADO_PATH"]
    with pytest.raises(ValueError, match="HLSFACTORY_VIVADO_PATH not in environment"):
        get_tool_paths(tool_paths_source=ToolPathsSource.ENV)


def test_get_tool_paths_fallthrough() -> None:
    with pytest.raises(ValueError, match="Invalid tool_paths_source"):
        get_tool_paths(tool_paths_source=None)
