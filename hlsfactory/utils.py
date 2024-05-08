import enum
import json
import os
import pathlib
import shlex
import shutil
import subprocess
import time
from dataclasses import is_dataclass
from pathlib import Path
from tempfile import TemporaryDirectory
from typing import TypeVar

import dotenv
import psutil
import yaml


class CallToolResult(enum.Enum):
    SUCCESS = enum.auto()
    TIMEOUT = enum.auto()
    ERROR = enum.auto()


def call_tool(
    cmd: str,
    cwd: Path,
    shell: bool = False,
    timeout: float | None = None,
    log_output: bool = False,
    raise_on_error: bool = True,
) -> CallToolResult:
    if shell:
        cmd_list = cmd
    else:
        cmd_list = shlex.split(cmd)

    try:
        p = subprocess.Popen(
            cmd_list,
            cwd=cwd,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            shell=shell,  # noqa: S603
        )
        os.sched_setaffinity(p.pid, [psutil.Process().cpu_num()])
        p.wait(timeout=timeout)
    except subprocess.TimeoutExpired:
        process_id = psutil.Process(p.pid)
        children = process_id.children(recursive=True)
        for child in children:
            child.terminate()
        p.terminate()
        return CallToolResult.TIMEOUT

    return_code = p.returncode
    if return_code != 0:
        if raise_on_error:
            msg = f"Command {cmd_list} failed with return code {return_code}"
            raise RuntimeError(msg)
        return CallToolResult.ERROR

    if log_output:
        print(p.stdout)

    return CallToolResult.SUCCESS


def terminate_process_and_children(pid: int) -> None:
    process_id = psutil.Process(pid)
    children = process_id.children(recursive=True)
    for child in children:
        child.terminate()
    process_id.terminate()


def wait_for_files_creation(
    file_paths: list[Path],
    timeout: float,
    poll_interval: float = 1,
) -> bool:
    if timeout < 0:
        msg = "Timeout must be non-negative"
        raise ValueError(msg)
    if poll_interval < 0:
        msg = "Poll interval must be non-negative"
        raise ValueError(msg)

    start_time = time.time()
    while True:
        files_exsit_list = [file_path.exists() for file_path in file_paths]
        all_files_exist = all(files_exsit_list)

        if all_files_exist:
            return True  # Return True if all files were found

        if time.time() - start_time > timeout:
            return False  # Return False if the timeout is exceeded

        time.sleep(poll_interval)


def find_bin_path(cmd: str) -> str:
    bin_path = shutil.which(cmd)
    if bin_path is None:
        msg = (
            "Could not find `{cmd}` automatically (via which), please specify the `cmd`"
            " path manually."
        )
        raise RuntimeError(msg)
    return bin_path


def log_execution_time_to_file(
    design_dir: Path,
    flow_name: str,
    t_0: float,
    t_1: float,
) -> None:
    dt = t_1 - t_0

    if not design_dir.exists():
        msg = f"Design directory {design_dir} does not exist."
        raise RuntimeError(msg)

    execution_time_data_fp = design_dir / "execution_time_data.json"
    if execution_time_data_fp.exists():
        execution_time_data = json.loads(execution_time_data_fp.read_text())
    else:
        execution_time_data = {}
    execution_time_data[flow_name] = {
        "t_start": t_0,
        "t_end": t_1,
        "dt": dt,
        "core": psutil.Process().cpu_num(),
    }
    execution_time_data_fp.write_text(json.dumps(execution_time_data, indent=4))


class FlowTimer:
    def __init__(self, flow_name: str, dir_path: Path) -> None:
        self.flow_name = flow_name
        self.dir_path = dir_path
        self.t_0: float | None = None
        self.t_1: float | None = None

    def start(self: "FlowTimer") -> None:
        self.t_0 = time.time()

    def stop(self: "FlowTimer") -> None:
        self.t_1 = time.time()

    def log(self) -> None:
        # assert self.t_0 is not None
        # assert self.t_1 is not None
        if self.t_0 is None:
            msg = "t_0 is None"
            raise RuntimeError(msg)
        if self.t_1 is None:
            msg = "t_1 is None"
            raise RuntimeError(msg)
        log_execution_time_to_file(self.dir_path, self.flow_name, self.t_0, self.t_1)

    def __enter__(self) -> "FlowTimer":
        self.start()
        return self

    def __exit__(self, _exc_type, _exc_value, _traceback) -> None:  # noqa: ANN001
        self.stop()
        self.log()


T = TypeVar("T")


def serialize_methods_for_dataclass(cls: type[T]) -> type[T]:
    if not is_dataclass(cls):
        msg = "Decorated class must be a dataclass."
        raise TypeError(msg)

    def from_json(cls: type[T], json_path: Path) -> T:
        with json_path.open("r") as f:
            d = json.load(f)
        return cls(**d)  # Type checked return of the dataclass instance

    def to_json(self: T, json_path: Path) -> None:
        with json_path.open("w") as f:
            json.dump(self.__dict__, f, indent=4)

    def from_yaml(cls: type[T], yaml_path: Path) -> T:
        with yaml_path.open("r") as f:
            d = yaml.safe_load(f)
        return cls(**d)  # Type checked return of the dataclass instance

    def to_yaml(self: T, yaml_path: Path) -> None:
        with yaml_path.open("w") as f:
            yaml.safe_dump(self.__dict__, f)

    cls.from_json = classmethod(from_json)
    cls.to_json = to_json
    cls.from_yaml = classmethod(from_yaml)
    cls.to_yaml = to_yaml
    return cls


def timeout_not_supported(flow_name: str) -> None:
    msg = f"Timeout not supported for the current flow: {flow_name}"
    raise RuntimeError(msg)


class DirSource(enum.Enum):
    ENVFILE = enum.auto()
    ENV = enum.auto()
    TEMP = enum.auto()


def get_work_dir(
    dir_source: DirSource = DirSource.ENVFILE,
) -> pathlib.Path:
    match dir_source:
        case DirSource.ENVFILE:
            envfile_vals = dotenv.dotenv_values()
            if "HLSFACTORY_WORK_DIR" not in envfile_vals:
                msg = "HLSFACTORY_WORK_DIR not in .env file"
                raise ValueError(msg)
            return pathlib.Path(envfile_vals["HLSFACTORY_WORK_DIR"])

        case DirSource.ENV:
            if "HLSFACTORY_WORK_DIR" not in os.environ:
                msg = "HLSFACTORY_WORK_DIR not in environment"
                raise ValueError(msg)
            return pathlib.Path(os.getenv("HLSFACTORY_WORK_DIR"))

        case DirSource.TEMP:
            temp_dir = TemporaryDirectory(prefix="hlsdataset__")
            return pathlib.Path(temp_dir.name)

        case _:
            msg = f"Invalid dir_source: {dir_source}"
            raise ValueError(msg)


class ToolPathsSource(enum.Enum):
    ENVFILE = enum.auto()
    ENV = enum.auto()


def get_tool_paths(
    tool_paths_source: ToolPathsSource,
) -> tuple[pathlib.Path, pathlib.Path]:
    match tool_paths_source:
        case ToolPathsSource.ENVFILE:
            envfile_vals = dotenv.dotenv_values()
            if "HLSFACTORY_VITIS_HLS_PATH" not in envfile_vals:
                msg = "HLSFACTORY_VITIS_HLS_PATH not in .env file"
                raise ValueError(msg)
            if "HLSFACTORY_VIVADO_PATH" not in envfile_vals:
                msg = "HLSFACTORY_VIVADO_PATH not in .env file"
                raise ValueError(msg)
            return (
                pathlib.Path(envfile_vals["HLSFACTORY_VITIS_HLS_PATH"]),
                pathlib.Path(envfile_vals["HLSFACTORY_VIVADO_PATH"]),
            )

        case ToolPathsSource.ENV:
            if "HLSFACTORY_VITIS_HLS_PATH" not in os.environ:
                msg = "HLSFACTORY_VITIS_HLS_PATH not in environment"
                raise ValueError(msg)
            if "HLSFACTORY_VIVADO_PATH" not in os.environ:
                msg = "HLSFACTORY_VIVADO_PATH not in environment"
                raise ValueError(msg)
            return (
                pathlib.Path(os.getenv("HLSFACTORY_VITIS_HLS_PATH")),
                pathlib.Path(os.getenv("HLSFACTORY_VIVADO_PATH")),
            )

        case _:
            msg = f"Invalid tool_paths_source: {tool_paths_source}"
            raise ValueError(msg)


def remove_dir_if_exists(dir_path: pathlib.Path) -> None:
    if dir_path.exists():
        shutil.rmtree(dir_path)


def remove_and_make_new_dir_if_exists(dir_path: pathlib.Path) -> None:
    remove_dir_if_exists(dir_path)
    dir_path.mkdir(parents=True, exist_ok=True)
