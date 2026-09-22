"""Siemens Catapult HLS synthesis flow."""

from __future__ import annotations

import os
import re
import shlex
import time
from dataclasses import dataclass
from pathlib import Path

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


HLSFACTORY_CATAPULT_PATH_ENV_VAR = "HLSFACTORY_CATAPULT_PATH"

_NUMBER_PATTERN = r"(?:[0-9]+(?:\.[0-9]*)?|\.[0-9]+)(?:[eE][+-]?\d+)?"

_TOTAL_AREA_PATTERNS = (
    re.compile(
        rf"\btotal\s+area\b\s*[:=]\s*({_NUMBER_PATTERN})",
        re.IGNORECASE,
    ),
    re.compile(
        rf"\btotal\b[^\n]*?\barea\b[^0-9]*({_NUMBER_PATTERN})",
        re.IGNORECASE,
    ),
)

_TOTAL_AREA_BREAKDOWN_PATTERN = re.compile(
    rf"^\s*TOTAL AREA \(After Assignment\):\s*"
    rf"(?P<total>{_NUMBER_PATTERN})\s+"
    rf"(?P<combinational>{_NUMBER_PATTERN})\s+"
    rf"(?P<raw>{_NUMBER_PATTERN})\s+"
    rf"(?P<sequential>{_NUMBER_PATTERN})",
    re.IGNORECASE | re.MULTILINE,
)


def get_catapult_bin(catapult_bin: str | Path | None = None) -> str:
    """Resolve the Catapult executable from an override, environment, or PATH.

    ``HLSFACTORY_CATAPULT_PATH`` may name the executable directly or an
    installation directory containing ``bin/catapult`` or
    ``Mgc_home/bin/catapult``.
    """
    if catapult_bin is not None:
        return str(Path(catapult_bin).expanduser())

    env_value = os.environ.get(HLSFACTORY_CATAPULT_PATH_ENV_VAR)
    if env_value:
        catapult_path = Path(env_value).expanduser()
        candidates = (
            catapult_path,
            catapult_path / "bin" / "catapult",
            catapult_path / "Mgc_home" / "bin" / "catapult",
        )
        for candidate in candidates:
            if candidate.is_file():
                return str(candidate)

        searched = ", ".join(str(candidate) for candidate in candidates)
        raise FileNotFoundError(
            f"Could not find the Catapult executable using "
            f"{HLSFACTORY_CATAPULT_PATH_ENV_VAR}={env_value!r}. "
            f"Searched: {searched}.",
        )

    return find_bin_path("catapult")


def auto_find_synth_report(dir_path: Path) -> Path:
    """Find the Catapult ``rtl.rpt`` generated below ``dir_path``."""
    report_results = sorted(dir_path.rglob("rtl.rpt"))
    if not report_results:
        raise FileNotFoundError(f"No rtl.rpt report file found in {dir_path}")
    if len(report_results) > 1:
        print(
            f"Found multiple rtl.rpt report files in {dir_path}. "
            f"Using the first one: {report_results[0]}",
        )
    return report_results[0]


def auto_find_cycle_report(dir_path: Path) -> Path:
    """Find the Catapult ``cycle.rpt`` generated below ``dir_path``."""
    report_results = sorted(dir_path.rglob("cycle.rpt"))
    if not report_results:
        raise FileNotFoundError(f"No cycle.rpt report file found in {dir_path}")
    if len(report_results) > 1:
        print(
            f"Found multiple cycle.rpt report files in {dir_path}. "
            f"Using the first one: {report_results[0]}",
        )
    return report_results[0]


def _parse_post_assignment_area(report_text: str, label: str) -> float | None:
    match = re.search(
        rf"^\s*{re.escape(label)}:\s*(?P<values>.*)$",
        report_text,
        re.MULTILINE,
    )
    if match is None:
        return None

    values_without_percentages = re.sub(r"\([^)]*%\)", "", match.group("values"))
    values = re.findall(_NUMBER_PATTERN, values_without_percentages)
    return float(values[-1]) if values else None


@serialize_methods_for_dataclass
@dataclass
class DesignHLSSynthData:
    """Synthesis metrics extracted from a Catapult RTL report."""

    total_area: float

    tool_version: str | None = None

    clock_signal: str | None = None
    clock_edge: str | None = None
    clock_period: float | None = None
    clock_period_ns: float | None = None
    clock_uncertainty_ns: float | None = None
    clock_allocation_percent: float | None = None

    real_operation_count: int | None = None
    latency_cycles: int | None = None
    latency_seconds: float | None = None
    throughput_cycles: int | None = None
    throughput_seconds: float | None = None
    reset_length_cycles: int | None = None
    initiation_interval_cycles: int | None = None

    combinational_area: float | None = None
    raw_area: float | None = None
    sequential_area: float | None = None
    total_area_score: float | None = None
    total_register_area: float | None = None
    datapath_area: float | None = None
    mux_area: float | None = None
    functional_unit_area: float | None = None
    logic_area: float | None = None
    buffer_area: float | None = None
    memory_area: float | None = None
    rom_area: float | None = None
    register_area: float | None = None
    fsm_area: float | None = None
    fsm_register_area: float | None = None
    fsm_combinational_area: float | None = None

    critical_path_delay_ns: float | None = None
    critical_path_slack_ns: float | None = None

    @classmethod
    def parse_from_synth_report_file(
        cls,
        report_file: Path,
        cycle_report_file: Path | None = None,
    ) -> DesignHLSSynthData:
        """Parse timing, latency, throughput, and area synthesis metrics."""
        report_text = report_file.read_text(encoding="utf-8", errors="ignore")

        area_breakdown_match = _TOTAL_AREA_BREAKDOWN_PATTERN.search(report_text)
        if area_breakdown_match is not None:
            total_area = float(area_breakdown_match.group("total"))
            combinational_area = float(
                area_breakdown_match.group("combinational"),
            )
            raw_area = float(area_breakdown_match.group("raw"))
            sequential_area = float(area_breakdown_match.group("sequential"))
        else:
            total_area = None
            combinational_area = None
            raw_area = None
            sequential_area = None
            for line in report_text.splitlines():
                if "total" not in line.lower() or "area" not in line.lower():
                    continue
                for pattern in _TOTAL_AREA_PATTERNS:
                    match = pattern.search(line)
                    if match is not None:
                        total_area = float(match.group(1))
                        break
                if total_area is not None:
                    break

        if total_area is None:
            raise ValueError(f"Could not find a total area value in {report_file}")

        version_match = re.search(
            r"^-- Version:\s*(?P<version>.+?)\s*$",
            report_text,
            re.MULTILINE,
        )
        design_total_match = re.search(
            r"^\s*Design Total:\s*"
            r"(?P<operations>\d+)\s+"
            r"(?P<latency>\d+)\s+"
            r"(?P<throughput>\d+)\s+"
            r"(?P<reset_length>\d+)\s+"
            r"(?P<ii>\d+)",
            report_text,
            re.MULTILINE,
        )
        max_delay_match = re.search(
            rf"^\s*Max Delay:\s*(?P<value>{_NUMBER_PATTERN})",
            report_text,
            re.MULTILINE,
        )
        slack_match = re.search(
            rf"^\s*Slack:\s*(?P<value>{_NUMBER_PATTERN})",
            report_text,
            re.MULTILINE,
        )

        clock_signal = None
        clock_edge = None
        clock_period_ns = None
        clock_allocation_percent = None
        clock_uncertainty_ns = None
        if cycle_report_file is not None:
            cycle_report_text = cycle_report_file.read_text(
                encoding="utf-8",
                errors="ignore",
            )
            clock_match = re.search(
                rf"^\s*(?P<signal>\S+)\s+"
                rf"(?P<edge>rising|falling)\s+"
                rf"(?P<period>{_NUMBER_PATTERN})\s+"
                rf"(?P<allocation>{_NUMBER_PATTERN})\s+"
                rf"(?P<uncertainty>{_NUMBER_PATTERN})\s+/\S+",
                cycle_report_text,
                re.IGNORECASE | re.MULTILINE,
            )
            if clock_match is not None:
                clock_signal = clock_match.group("signal")
                clock_edge = clock_match.group("edge").lower()
                clock_period_ns = float(clock_match.group("period"))
                clock_allocation_percent = float(clock_match.group("allocation"))
                clock_uncertainty_ns = float(clock_match.group("uncertainty"))

        clock_period = clock_period_ns * 1e-9 if clock_period_ns is not None else None
        latency_cycles = (
            int(design_total_match.group("latency"))
            if design_total_match is not None
            else None
        )
        throughput_cycles = (
            int(design_total_match.group("throughput"))
            if design_total_match is not None
            else None
        )

        return cls(
            total_area=total_area,
            tool_version=(
                version_match.group("version").strip()
                if version_match is not None
                else None
            ),
            clock_signal=clock_signal,
            clock_edge=clock_edge,
            clock_period=clock_period,
            clock_period_ns=clock_period_ns,
            clock_uncertainty_ns=clock_uncertainty_ns,
            clock_allocation_percent=clock_allocation_percent,
            real_operation_count=(
                int(design_total_match.group("operations"))
                if design_total_match is not None
                else None
            ),
            latency_cycles=latency_cycles,
            latency_seconds=(
                latency_cycles * clock_period
                if latency_cycles is not None and clock_period is not None
                else None
            ),
            throughput_cycles=throughput_cycles,
            throughput_seconds=(
                throughput_cycles * clock_period
                if throughput_cycles is not None and clock_period is not None
                else None
            ),
            reset_length_cycles=(
                int(design_total_match.group("reset_length"))
                if design_total_match is not None
                else None
            ),
            initiation_interval_cycles=(
                int(design_total_match.group("ii"))
                if design_total_match is not None
                else None
            ),
            combinational_area=combinational_area,
            raw_area=raw_area,
            sequential_area=sequential_area,
            total_area_score=_parse_post_assignment_area(
                report_text,
                "Total Area Score",
            ),
            total_register_area=_parse_post_assignment_area(
                report_text,
                "Total Reg",
            ),
            datapath_area=_parse_post_assignment_area(report_text, "DataPath"),
            mux_area=_parse_post_assignment_area(report_text, "MUX"),
            functional_unit_area=_parse_post_assignment_area(report_text, "FUNC"),
            logic_area=_parse_post_assignment_area(report_text, "LOGIC"),
            buffer_area=_parse_post_assignment_area(report_text, "BUFFER"),
            memory_area=_parse_post_assignment_area(report_text, "MEM"),
            rom_area=_parse_post_assignment_area(report_text, "ROM"),
            register_area=_parse_post_assignment_area(report_text, "REG"),
            fsm_area=_parse_post_assignment_area(report_text, "FSM"),
            fsm_register_area=_parse_post_assignment_area(report_text, "FSM-REG"),
            fsm_combinational_area=_parse_post_assignment_area(
                report_text,
                "FSM-COMB",
            ),
            critical_path_delay_ns=(
                float(max_delay_match.group("value"))
                if max_delay_match is not None
                else None
            ),
            critical_path_slack_ns=(
                float(slack_match.group("value")) if slack_match is not None else None
            ),
        )


class CatapultHLSSynthFlow(ToolFlow):
    """Run a design's Catapult synthesis Tcl script and collect its area report."""

    name = "CatapultHLSSynthFlow"

    def __init__(
        self,
        catapult_bin: str | Path | None = None,
        log_output: bool = False,
        log_execution_time: bool = True,
    ) -> None:
        self.catapult_bin = get_catapult_bin(catapult_bin)
        self.log_output = log_output
        self.log_execution_time = log_execution_time

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        design_dir = design.dir
        data_file = design_dir / "data_hls.json"

        if flow_already_completed(design_dir, self.name):
            print(f"[{design_dir}] Skipping {self.name}, already completed")
            return [design]

        start_time = time.perf_counter()

        config = design.require_config()
        synth_tcl_name = config.require_flow_setting(
            FlowName.CATAPULT_HLS_SYNTH,
            "synth_tcl",
        )
        synth_tcl = design_dir / synth_tcl_name
        if not synth_tcl.is_file():
            raise FileNotFoundError(
                f"Build file {synth_tcl} does not exist. This build file is required "
                "for Catapult synthesis.",
            )

        error_marker = design_dir / f"error__{self.name}.txt"
        timeout_marker = design_dir / f"timeout__{self.name}.txt"
        for stale_file in (data_file, error_marker, timeout_marker):
            stale_file.unlink(missing_ok=True)

        command = " ".join(
            (
                shlex.quote(self.catapult_bin),
                "-shell",
                "-file",
                shlex.quote(synth_tcl_name),
            ),
        )
        result = call_tool(
            command,
            cwd=design_dir,
            log_output=self.log_output,
            timeout=timeout,
            raise_on_error=False,
        )

        if result == CallToolResult.TIMEOUT:
            timeout_marker.touch()
            print(f"[{design_dir}] Timeout of {timeout} seconds reached")
            self._log_execution_time(
                design_dir,
                start_time,
                status=ExecutionDataStatus.TIMEOUT,
                error_message=f"Timeout of {timeout}s reached",
            )
            return []

        if result == CallToolResult.ERROR:
            error_marker.touch()
            print(f"[{design_dir}] Error occurred during execution")
            self._log_execution_time(
                design_dir,
                start_time,
                status=ExecutionDataStatus.ERROR,
                error_message="Catapult synthesis execution error",
            )
            return []

        try:
            report_file = auto_find_synth_report(design_dir)
            try:
                cycle_report_file = auto_find_cycle_report(design_dir)
            except FileNotFoundError:
                cycle_report_file = None
            synthesis_data = DesignHLSSynthData.parse_from_synth_report_file(
                report_file,
                cycle_report_file,
            )
        except (FileNotFoundError, OSError, ValueError) as error:
            error_marker.write_text(f"{error}\n")
            print(f"[{design_dir}] Could not collect Catapult synthesis data: {error}")
            self._log_execution_time(
                design_dir,
                start_time,
                status=ExecutionDataStatus.ERROR,
                error_message=f"Could not collect Catapult synthesis data: {error}",
            )
            return []

        synthesis_data.to_json(data_file)  # type: ignore[attr-defined]
        self._log_execution_time(design_dir, start_time)
        return [design]

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
    "CatapultHLSSynthFlow",
    "DesignHLSSynthData",
    "HLSFACTORY_CATAPULT_PATH_ENV_VAR",
    "auto_find_cycle_report",
    "auto_find_synth_report",
    "get_catapult_bin",
]
