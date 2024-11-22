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
    """
    Represents the result of a tool call using `call_tool`.
    """

    SUCCESS = enum.auto()
    TIMEOUT = enum.auto()
    ERROR = enum.auto()


def call_tool(
    cmd: str,
    cwd: Path,
    shell: bool = False,
    timeout: float | None = None,
    log_output: bool = False,
    raise_on_error: bool = False,
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
            shell=shell,
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
    """
    Terminate a process and all its child processes.

    Args:
        pid (int): The process ID of the parent process.
    """
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
    """
    Find the path of a binary executable.

    Args:
        cmd (str): The name of the binary executable.

    Returns:
        str: The path of the binary executable.

    Raises:
        RuntimeError: If the binary executable cannot be found.

    """
    bin_path = shutil.which(cmd)
    if bin_path is None:
        msg = (
            f"Could not find `{cmd}` automatically (via which), please specify the `cmd`"
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
    """
    Logs the execution time of a specific flow to a file.

    Args:
        design_dir (Path): The directory where the design is located.
        flow_name (str): The name of the flow.
        t_0 (float): The start time of the execution.
        t_1 (float): The end time of the execution.

    Raises:
        RuntimeError: If the design directory does not exist.
    """
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
    """
    A class for measuring the execution time of a flow.

    Attributes:
        flow_name (str): The name of the flow.
        dir_path (Path): The directory path where the execution time will be logged.
        t_0 (float | None): The start time of the flow execution.
        t_1 (float | None): The stop time of the flow execution.
    """

    def __init__(self, flow_name: str, dir_path: Path) -> None:
        self.flow_name = flow_name
        self.dir_path = dir_path
        self.t_0: float | None = None
        self.t_1: float | None = None

    def start(self: "FlowTimer") -> None:
        """
        Start the timer for measuring the flow execution time.
        """
        self.t_0 = time.time()

    def stop(self: "FlowTimer") -> None:
        """
        Stop the timer for measuring the flow execution time.
        """
        self.t_1 = time.time()

    def log(self) -> None:
        """
        Log the execution time of the flow to a file.
        Raises:
            RuntimeError: If either t_0 or t_1 is None.
        """
        if self.t_0 is None:
            msg = "t_0 is None"
            raise RuntimeError(msg)
        if self.t_1 is None:
            msg = "t_1 is None"
            raise RuntimeError(msg)
        log_execution_time_to_file(self.dir_path, self.flow_name, self.t_0, self.t_1)

    def __enter__(self) -> "FlowTimer":
        """
        Start the timer when entering a context.
        Returns:
            FlowTimer: The FlowTimer instance.
        """
        self.start()
        return self

    def __exit__(self, _exc_type, _exc_value, _traceback) -> None:  # noqa: ANN001
        """
        Stop the timer and log the execution time when exiting a context.
        """
        self.stop()
        self.log()


T = TypeVar("T")


def serialize_methods_for_dataclass(cls: type[T]) -> type[T]:
    """
    Decorator function that adds serialization methods to a dataclass.

    The serialization methods added are:
    - from_json: A class method that
    creates a dataclass instance from a JSON file.
    - to_json: An instance method that
    writes the dataclass instance to
    a JSON file.
    - from_yaml: A class method that
    creates a dataclass instance from a YAML file.
    - to_yaml: An instance method that
    writes the dataclass instance to
    a YAML file.


    Args:
        cls (type[T]): The dataclass to decorate.

    Returns:
        type[T]: The decorated dataclass.

    Raises:
        TypeError: If the decorated class is not a dataclass.
    """
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
    """
    Raises a RuntimeError indicating that timeout is not supported for the current flow.

    Args:
        flow_name (str): The name of the current flow.

    Raises:
        RuntimeError: Indicates that timeout is not supported for the current flow.
    """
    msg = f"Timeout not supported for the current flow: {flow_name}"
    raise RuntimeError(msg)


class DirSource(enum.Enum):
    """
    Enum class representing different sources for directories.

    Used by `get_work_dir` to determine the source to look
    for a specific work directory to use.

    Options:
    - ENVFILE: Look for the directory in the .env file.
    - ENV: Look for the directory in the environment variables.
    - TEMP: Create a temporary directory.
    """

    ENVFILE = enum.auto()
    ENV = enum.auto()
    TEMP = enum.auto()


def get_work_dir(
    dir_source: DirSource = DirSource.ENVFILE,
    env_file_path: Path | None = None,
    use_cwd: bool = True,
) -> pathlib.Path:
    """
    Get the working directory path based on the specified directory source.

    Args:
        dir_source (DirSource, optional): The directory source to use.
        Defaults to DirSource.ENVFILE.

    Returns:
        pathlib.Path: The path to the working directory.

    Raises:
        ValueError: If the specified directory source is
        invalid or the working directory path is not found.

    """
    match dir_source:
        case DirSource.ENVFILE:
            if env_file_path is None:
                env_file_path_search = dotenv.find_dotenv(usecwd=use_cwd)
                if env_file_path_search is None:
                    msg = "The .env file could not be automatically found."
                    raise ValueError(msg)
                env_fp = Path(env_file_path_search)
            else:
                env_fp = env_file_path
            envfile_vals = dotenv.dotenv_values(
                env_fp,
            )
            if "HLSFACTORY_WORK_DIR" not in envfile_vals:
                msg = "HLSFACTORY_WORK_DIR not in .env file"
                raise ValueError(msg)
            if envfile_vals["HLSFACTORY_WORK_DIR"] is None:
                msg = "HLSFACTORY_WORK_DIR not set to a valid path in .env file"
                raise ValueError(msg)
            return pathlib.Path(envfile_vals["HLSFACTORY_WORK_DIR"])

        case DirSource.ENV:
            if "HLSFACTORY_WORK_DIR" not in os.environ:
                msg = "HLSFACTORY_WORK_DIR not in environment"
                raise ValueError(msg)
            val = os.getenv("HLSFACTORY_WORK_DIR")
            if val is None:
                msg = "HLSFACTORY_WORK_DIR not set to a valid path in environment"
                raise ValueError(msg)
            return pathlib.Path(val)

        case DirSource.TEMP:
            temp_dir = TemporaryDirectory(prefix="hlsfactory__")
            return pathlib.Path(temp_dir.name)

        case _:
            msg = f"Invalid dir_source: {dir_source}"
            raise ValueError(msg)


class ToolPathsSource(enum.Enum):
    """
    Enumeration representing the source of tool paths.

    Used by `get_tool_paths` to determine the source to look
    for the paths of the tools to use.

    Options:
    - ENVFILE: Look for the paths in the .env file.
    - ENV: Look for the paths in the environment variables.
    """

    ENVFILE = enum.auto()
    ENV = enum.auto()
    # TODO: Implement the WHICH option to use the shutil.which function


def get_tool_paths(
    tool_paths_source: ToolPathsSource,
    env_file_path: Path | None = None,
    use_cwd: bool = True,
) -> tuple[pathlib.Path, pathlib.Path]:
    """
    Get the paths for Vitis HLS and Vivado tools based on the specified source.

    Args:
        tool_paths_source (ToolPathsSource): The source from which to
        retrieve the tool paths.

    Returns:
        tuple[pathlib.Path, pathlib.Path]: A tuple containing the paths for
        Vitis HLS and Vivado tools.

    Raises:
        ValueError: If the tool paths are not found in the specified source.

    """
    match tool_paths_source:
        case ToolPathsSource.ENVFILE:
            if env_file_path is None:
                env_file_path_search = dotenv.find_dotenv(usecwd=use_cwd)
                if env_file_path_search is None:
                    msg = "The .env file could not be automatically found."
                    raise ValueError(msg)
                env_fp = Path(env_file_path_search)
            else:
                env_fp = env_file_path
            envfile_vals = dotenv.dotenv_values(
                env_fp,
            )
            if "HLSFACTORY_VITIS_HLS_PATH" not in envfile_vals:
                msg = "HLSFACTORY_VITIS_HLS_PATH not in .env file"
                raise ValueError(msg)
            if "HLSFACTORY_VIVADO_PATH" not in envfile_vals:
                msg = "HLSFACTORY_VIVADO_PATH not in .env file"
                raise ValueError(msg)

            if envfile_vals["HLSFACTORY_VITIS_HLS_PATH"] is None:
                msg = "HLSFACTORY_VITIS_HLS_PATH not set to a valid path in .env file"
                raise ValueError(msg)
            if envfile_vals["HLSFACTORY_VIVADO_PATH"] is None:
                msg = "HLSFACTORY_VIVADO_PATH not set to a valid path in .env file"
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
            env_val_vitis = os.getenv("HLSFACTORY_VITIS_HLS_PATH")
            env_val_vivado = os.getenv("HLSFACTORY_VIVADO_PATH")

            if env_val_vitis is None:
                msg = "HLSFACTORY_VITIS_HLS_PATH not set to a valid path in environment"
                raise ValueError(msg)
            if env_val_vivado is None:
                msg = "HLSFACTORY_VIVADO_PATH not set to a valid path in environment"
                raise ValueError(msg)
            return (
                pathlib.Path(env_val_vitis),
                pathlib.Path(env_val_vivado),
            )

        case _:
            msg = f"Invalid tool_paths_source: {tool_paths_source}"
            raise ValueError(msg)


def remove_dir_if_exists(dir_path: pathlib.Path) -> None:
    """
    Remove a directory if it exists.

    Args:
        dir_path (pathlib.Path): The path to the directory.

    Returns:
        None
    """
    if dir_path.exists():
        shutil.rmtree(dir_path)


def remove_and_make_new_dir_if_exists(dir_path: pathlib.Path) -> None:
    """
    Removes the directory at the given path if it exists,
    and then creates a new directory at the same path.

    Args:
        dir_path (pathlib.Path): The path to the directory.

    Returns:
        None
    """
    remove_dir_if_exists(dir_path)
    dir_path.mkdir(parents=True, exist_ok=True)
