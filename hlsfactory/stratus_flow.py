"""Cadence Stratus HLS synthesis flow.

Stratus projects use ``project.tcl`` to describe their modules and synthesis
configurations. Cadence's supported batch flow first generates
``Makefile.prj`` with ``bdw_makegen`` and then executes an ``hls_*`` make
target. This module follows that flow and reads results through Stratus's
Unified Metrics API.
"""

from __future__ import annotations

import os
import re
import shlex
import time
from contextlib import contextmanager
from dataclasses import dataclass
from pathlib import Path
from typing import Iterator

from hlsfactory.design_config import FlowName
from hlsfactory.framework import Design, ToolFlow
from hlsfactory.utils import (
    CallToolResult,
    ExecutionDataStatus,
    call_tool,
    find_bin_path,
    flow_already_completed,
    serialize_methods_for_dataclass,
    update_execution_data_with_flow_results,
)

_METRICS_TCL_FILENAME = ".hlsfactory_stratus_metrics.tcl"
_METRICS_TSV_FILENAME = ".hlsfactory_stratus_metrics.tsv"

MetricValue = int | float | str

_NUMBER_PATTERN = (
    r"[0-9]+(?:\.[0-9]*)?(?:[eE][+-]?[0-9]+)?|\.[0-9]+(?:[eE][+-]?[0-9]+)?"
)
_TOTAL_AREA_PATTERN = re.compile(
    rf"Total\s+Area\s+({_NUMBER_PATTERN})(?:\([0-9]+\))?\s+"
    rf"({_NUMBER_PATTERN})\s+({_NUMBER_PATTERN})\s+({_NUMBER_PATTERN})",
    re.IGNORECASE,
)


def _parse_metric_value(value: str) -> MetricValue:
    """Convert a Stratus metric to an int/float when it is numeric."""
    stripped = value.strip()
    try:
        return int(stripped)
    except ValueError:
        try:
            return float(stripped)
        except ValueError:
            return stripped


def parse_metrics_file(metrics_file: Path) -> dict[str, MetricValue]:
    """Parse the tab-separated file emitted by the metrics Tcl script."""
    metrics: dict[str, MetricValue] = {}
    for line_number, line in enumerate(
        metrics_file.read_text(encoding="utf-8").splitlines(),
        start=1,
    ):
        if not line.strip():
            continue
        try:
            name, value = line.split("\t", maxsplit=1)
        except ValueError as error:
            raise ValueError(
                f"Malformed Stratus metric at {metrics_file}:{line_number}",
            ) from error
        if not name:
            raise ValueError(
                f"Empty Stratus metric name at {metrics_file}:{line_number}",
            )
        metrics[name] = _parse_metric_value(value)
    if not metrics:
        raise ValueError(f"No Stratus metrics found in {metrics_file}")
    return metrics


def parse_area_metrics_from_synth_log(
    synth_log: Path,
) -> dict[str, MetricValue]:
    """Read the final allocation area's seq/comb/black-box/total columns."""
    matches = list(
        _TOTAL_AREA_PATTERN.finditer(
            synth_log.read_text(encoding="utf-8", errors="ignore"),
        ),
    )
    if not matches:
        raise ValueError(f"No Total Area allocation row found in {synth_log}")

    sequential, combinational, black_box, total = (
        float(value) for value in matches[-1].groups()
    )
    return {
        "design.area": total,
        "design.area.combinatorial": combinational,
        "design.area.sequential": sequential,
        "design.area.black_box": black_box,
    }


def auto_find_hls_output_dir(
    design_dir: Path,
    hls_module: str,
    hls_config: str,
) -> Path:
    """Find the output directory for one Stratus module/configuration pair."""
    suffix = Path("modules") / hls_module / hls_config / "stratus_hls.bdr"
    suffix_parts = suffix.parts
    results = sorted(
        report.parent
        for report in design_dir.rglob("stratus_hls.bdr")
        if report.is_file()
        and tuple(report.parts[-len(suffix_parts) :]) == suffix_parts
    )
    if not results:
        raise FileNotFoundError(
            "No Stratus synthesis database found for "
            f"{hls_module}/{hls_config} below {design_dir}",
        )
    if len(results) > 1:
        raise ValueError(
            "Found multiple Stratus synthesis databases for "
            f"{hls_module}/{hls_config} below {design_dir}: {results}",
        )
    return results[0]


def _require_generated_rtl(output_dir: Path) -> Path:
    rtl_files = sorted(output_dir.glob("*_rtl.v"))
    if not rtl_files:
        raise FileNotFoundError(f"No generated *_rtl.v file found in {output_dir}")
    return rtl_files[0]


def _tcl_quote(value: str) -> str:
    """Quote a Python string as a Tcl double-quoted word."""
    escaped = value.replace("\\", "\\\\")
    escaped = escaped.replace("$", "\\$").replace("[", "\\[")
    escaped = escaped.replace('"', '\\"').replace("\n", "\\n")
    return f'"{escaped}"'


def _metrics_tcl(
    project_tcl: Path,
    metrics_file: Path,
    hls_module: str,
    hls_config: str,
) -> str:
    config_id = f"{hls_module}/{hls_config}"
    return "\n".join(
        (
            f"open_project {_tcl_quote(str(project_tcl))}",
            f"set config_id {_tcl_quote(config_id)}",
            f"set output [open {_tcl_quote(str(metrics_file))} w]",
            'puts $output "__tool_version\\t[get_version]"',
            'puts $output "__clock_period\\t[get_attr clock_period $config_id]"',
            "foreach {name value} [get_metric * -id $config_id] {",
            '    puts $output "$name\\t$value"',
            "}",
            "close $output",
            "exit",
            "",
        ),
    )


@serialize_methods_for_dataclass
@dataclass
class DesignHLSSynthData:
    """QoR metrics reported by Stratus HLS."""

    total_area: float
    combinational_area: float | None
    sequential_area: float | None
    latency_total_cycles: int | float | None
    latency_cycles: dict[str, int | float]
    metrics: dict[str, MetricValue]

    @classmethod
    def parse_from_metrics_file(cls, metrics_file: Path) -> DesignHLSSynthData:
        return cls.parse_from_metrics(
            parse_metrics_file(metrics_file),
            source=str(metrics_file),
        )

    @classmethod
    def parse_from_metrics(
        cls,
        metrics: dict[str, MetricValue],
        *,
        source: str,
    ) -> DesignHLSSynthData:
        """Build synthesis data from collected and optionally supplemented metrics."""

        try:
            total_area = float(metrics["design.area"])
        except (KeyError, TypeError, ValueError) as error:
            raise ValueError(
                f"Stratus metric design.area is missing or invalid in {source}",
            ) from error

        def optional_float(name: str) -> float | None:
            value = metrics.get(name)
            if value is None:
                return None
            try:
                return float(value)
            except (TypeError, ValueError) as error:
                raise ValueError(
                    f"Stratus metric {name} is not numeric in {source}",
                ) from error

        latency_cycles: dict[str, int | float] = {}
        for name, value in metrics.items():
            if not name.startswith("design.latency."):
                continue
            if not isinstance(value, (int, float)):
                raise ValueError(
                    f"Stratus latency metric {name} is not numeric in {source}",
                )
            latency_cycles[name.removeprefix("design.latency.")] = value

        public_metrics = {
            name: value for name, value in metrics.items() if not name.startswith("__")
        }
        return cls(
            total_area=total_area,
            combinational_area=optional_float("design.area.combinatorial"),
            sequential_area=optional_float("design.area.sequential"),
            latency_total_cycles=max(latency_cycles.values(), default=None),
            latency_cycles=latency_cycles,
            metrics=public_metrics,
        )


@serialize_methods_for_dataclass
@dataclass
class StratusHLSDesign:
    """Design and tool metadata for a Stratus HLS result."""

    name: str
    hls_config: str
    target_clock_period: float
    version_stratus: str

    @classmethod
    def parse_from_metrics_file(
        cls,
        metrics_file: Path,
        *,
        hls_module: str,
        hls_config: str,
    ) -> StratusHLSDesign:
        metrics = parse_metrics_file(metrics_file)
        try:
            clock_period = float(metrics["__clock_period"])
            version = str(metrics["__tool_version"])
        except (KeyError, TypeError, ValueError) as error:
            raise ValueError(
                f"Stratus tool metadata is missing or invalid in {metrics_file}",
            ) from error
        return cls(
            name=hls_module,
            hls_config=hls_config,
            target_clock_period=clock_period,
            version_stratus=version,
        )


@contextmanager
def _stratus_environment(*tool_paths: str) -> Iterator[None]:
    """Make sibling Stratus executables visible to generated make rules."""
    old_path = os.environ.get("PATH")
    tool_dirs = list(
        dict.fromkeys(
            str(Path(tool).parent)
            for tool in tool_paths
            if Path(tool).parent != Path(".")
        ),
    )
    existing_path = old_path or ""
    os.environ["PATH"] = os.pathsep.join((*tool_dirs, existing_path))
    try:
        yield
    finally:
        if old_path is None:
            os.environ.pop("PATH", None)
        else:
            os.environ["PATH"] = old_path


class StratusHLSSynthFlow(ToolFlow):
    """Run one Stratus HLS module/configuration and collect its metrics."""

    name = "StratusHLSSynthFlow"

    def __init__(
        self,
        stratus_install_dir: str | Path | None = None,
        stratus_bin: str | None = None,
        bdw_makegen_bin: str | None = None,
        make_bin: str | None = None,
        log_output: bool = False,
        log_execution_time: bool = True,
    ) -> None:
        if stratus_install_dir is not None:
            install_bin = Path(stratus_install_dir) / "bin"
            stratus_bin = stratus_bin or str(install_bin / "stratus")
            bdw_makegen_bin = bdw_makegen_bin or str(install_bin / "bdw_makegen")

        self.stratus_bin = stratus_bin or find_bin_path("stratus")
        self.bdw_makegen_bin = bdw_makegen_bin or find_bin_path("bdw_makegen")
        self.make_bin = make_bin or find_bin_path("make")
        self.log_output = log_output
        self.log_execution_time = log_execution_time

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        design_dir = design.dir
        data_file = design_dir / "data_hls.json"

        if flow_already_completed(design_dir, self.name):
            print(f"[{design_dir}] Skipping {self.name}, already completed")
            return [design]

        start_time = time.perf_counter()
        flow_config = design.require_config().require_flow_config(
            FlowName.STRATUS_HLS_SYNTH,
        )
        project_tcl_name = flow_config.require_setting("project_tcl")
        hls_module = flow_config.require_setting("hls_module")
        hls_config = flow_config.require_setting("hls_config")
        makefile_name = flow_config.get_setting("makefile", "Makefile")
        assert makefile_name is not None

        project_tcl = design_dir / project_tcl_name
        makefile = design_dir / makefile_name
        for build_file in (project_tcl, makefile):
            if not build_file.is_file():
                raise FileNotFoundError(
                    f"Build file {build_file} does not exist. This file is required "
                    "for Stratus synthesis.",
                )

        makegen_command = " ".join(
            (
                shlex.quote(self.bdw_makegen_bin),
                shlex.quote(project_tcl_name),
                "-o",
                "Makefile.prj",
            ),
        )
        make_target = f"hls_{hls_module}_{hls_config}"
        make_command = " ".join(
            (
                shlex.quote(self.make_bin),
                "-f",
                shlex.quote(makefile_name),
                shlex.quote(make_target),
            ),
        )

        with _stratus_environment(self.stratus_bin, self.bdw_makegen_bin):
            result = call_tool(
                makegen_command,
                cwd=design_dir,
                log_output=self.log_output,
                timeout=timeout,
                raise_on_error=False,
            )
            if result != CallToolResult.SUCCESS:
                return self._handle_tool_failure(
                    design_dir,
                    result,
                    timeout,
                    start_time,
                    "Makefile generation",
                )

            result = call_tool(
                make_command,
                cwd=design_dir,
                log_output=self.log_output,
                timeout=timeout,
                raise_on_error=False,
            )
            if result != CallToolResult.SUCCESS:
                return self._handle_tool_failure(
                    design_dir,
                    result,
                    timeout,
                    start_time,
                    "synthesis",
                )

            try:
                output_dir = auto_find_hls_output_dir(
                    design_dir,
                    hls_module,
                    hls_config,
                )
                _require_generated_rtl(output_dir)
                metrics_file = design_dir / _METRICS_TSV_FILENAME
                metrics_tcl = design_dir / _METRICS_TCL_FILENAME
                metrics_tcl.write_text(
                    _metrics_tcl(
                        project_tcl.resolve(),
                        metrics_file.resolve(),
                        hls_module,
                        hls_config,
                    ),
                    encoding="utf-8",
                )
                try:
                    metrics_result = call_tool(
                        " ".join(
                            (
                                shlex.quote(self.stratus_bin),
                                "-batch",
                                "-files",
                                shlex.quote(metrics_tcl.name),
                            ),
                        ),
                        cwd=design_dir,
                        log_output=self.log_output,
                        timeout=timeout,
                        raise_on_error=False,
                    )
                    if metrics_result != CallToolResult.SUCCESS:
                        return self._handle_tool_failure(
                            design_dir,
                            metrics_result,
                            timeout,
                            start_time,
                            "metrics collection",
                        )

                    metrics = parse_metrics_file(metrics_file)
                    if "design.area" not in metrics:
                        metrics.update(
                            parse_area_metrics_from_synth_log(
                                output_dir / "stratus_hls.log",
                            ),
                        )
                    hls_data = DesignHLSSynthData.parse_from_metrics(
                        metrics,
                        source=str(metrics_file),
                    )
                    design_data = StratusHLSDesign.parse_from_metrics_file(
                        metrics_file,
                        hls_module=hls_module,
                        hls_config=hls_config,
                    )
                finally:
                    metrics_tcl.unlink(missing_ok=True)
                    metrics_file.unlink(missing_ok=True)
            except (FileNotFoundError, OSError, ValueError) as error:
                self._write_error(
                    design_dir, f"Could not collect Stratus data: {error}"
                )
                self._log_execution_time(
                    design_dir,
                    start_time,
                    status=ExecutionDataStatus.ERROR,
                    error_message=f"Could not collect Stratus data: {error}",
                )
                return []

        hls_data.to_json(data_file)  # type: ignore[attr-defined]
        design_data.to_json(design_dir / "data_design.json")  # type: ignore[attr-defined]
        self._log_execution_time(design_dir, start_time)
        return [design]

    def _handle_tool_failure(
        self,
        design_dir: Path,
        result: CallToolResult,
        timeout: float | None,
        start_time: float,
        stage: str,
    ) -> list[Design]:
        if result == CallToolResult.TIMEOUT:
            error_message = f"Timeout of {timeout}s reached during Stratus {stage}"
            (design_dir / f"timeout__{self.name}.txt").touch()
            print(
                f"[{design_dir}] Timeout of {timeout} seconds reached during "
                f"Stratus {stage}",
            )
        else:
            error_message = f"Stratus {stage} failed"
            self._write_error(design_dir, error_message)
        self._log_execution_time(
            design_dir,
            start_time,
            status=(
                ExecutionDataStatus.TIMEOUT
                if result == CallToolResult.TIMEOUT
                else ExecutionDataStatus.ERROR
            ),
            error_message=error_message,
        )
        return []

    def _write_error(self, design_dir: Path, message: str) -> None:
        (design_dir / f"error__{self.name}.txt").write_text(
            f"{message}\n",
            encoding="utf-8",
        )
        print(f"[{design_dir}] {message}")

    def _log_execution_time(
        self,
        design_dir: Path,
        start_time: float,
        status: ExecutionDataStatus = ExecutionDataStatus.SUCCESS,
        error_message: str | None = None,
    ) -> None:
        if self.log_execution_time:
            update_execution_data_with_flow_results(
                design_dir,
                self.name,
                status,
                start_time,
                time.perf_counter(),
                error_message=error_message,
            )


__all__ = [
    "DesignHLSSynthData",
    "StratusHLSDesign",
    "StratusHLSSynthFlow",
    "auto_find_hls_output_dir",
    "parse_area_metrics_from_synth_log",
    "parse_metrics_file",
]
