"""Google XLS HLS synthesis flow for DSLX designs."""

from __future__ import annotations

import os
import re
import shlex
import shutil
import time
from collections import Counter
from dataclasses import dataclass
from pathlib import Path

from hlsfactory.design_config import FlowName
from hlsfactory.framework import Design, ToolFlow
from hlsfactory.utils import (
    CallToolResult,
    call_tool,
    flow_already_completed,
    log_execution_time_to_file,
    serialize_methods_for_dataclass,
)


HLSFACTORY_XLS_PATH_ENV_VAR = "HLSFACTORY_XLS_PATH"
DEFAULT_XLS_PATH = Path("/usr/scratch/common/xls")

_XLS_BINARY_LOCATIONS = {
    "ir_converter_main": (
        Path("ir_converter_main"),
        Path("bazel-bin/xls/dslx/ir_convert/ir_converter_main"),
    ),
    "opt_main": (
        Path("opt_main"),
        Path("bazel-bin/xls/tools/opt_main"),
    ),
    "codegen_main": (
        Path("codegen_main"),
        Path("bazel-bin/xls/tools/codegen_main"),
    ),
}


def get_xls_install_dir(xls_install_dir: str | Path | None = None) -> Path:
    """Resolve the XLS installation directory.

    An explicit argument takes precedence over ``HLSFACTORY_XLS_PATH``. The final
    fallback is the shared XLS installation used by the HLSFactory server.
    """
    if xls_install_dir is not None:
        return Path(xls_install_dir).expanduser()

    env_value = os.environ.get(HLSFACTORY_XLS_PATH_ENV_VAR)
    if env_value:
        return Path(env_value).expanduser()

    return DEFAULT_XLS_PATH


def find_xls_binary(xls_install_dir: Path, binary_name: str) -> Path:
    """Find an XLS binary in a release bundle or Bazel source build."""
    try:
        relative_locations = _XLS_BINARY_LOCATIONS[binary_name]
    except KeyError as error:
        raise ValueError(f"Unknown XLS binary: {binary_name}") from error

    candidates = [xls_install_dir / location for location in relative_locations]
    for candidate in candidates:
        if candidate.is_file():
            return candidate

    searched = ", ".join(str(candidate) for candidate in candidates)
    raise FileNotFoundError(
        f"Could not find `{binary_name}` under XLS installation directory "
        f"{xls_install_dir}. Searched: {searched}. Set {HLSFACTORY_XLS_PATH_ENV_VAR} "
        "or pass `xls_install_dir` to XLSHLSSynthFlow.",
    )


def parse_xls_bool_setting(value: str | None, setting_name: str) -> bool:
    """Parse a string-valued boolean from an XLS flow configuration."""
    if value is None:
        return False
    normalized = value.strip().lower()
    if normalized in {"1", "true", "yes", "on"}:
        return True
    if normalized in {"0", "false", "no", "off"}:
        return False
    raise ValueError(
        f"XLS `{setting_name}` must be a boolean string; got {value!r}.",
    )


@serialize_methods_for_dataclass
@dataclass
class DesignHLSSynthData:
    """Artifacts and code-generation settings from an XLS synthesis run."""

    dslx_file: str
    top: str
    generator: str
    pipeline_stages: int | None
    delay_model: str | None
    reset: str | None
    ir_file: str
    optimized_ir_file: str
    rtl_file: str
    module_signature_file: str
    block_metrics_file: str
    interface_proto_file: str
    interface_textproto_file: str
    ir_converter_options_file: str
    optimizer_options_file: str
    optimization_pass_metrics_file: str
    schedule_file: str | None
    scheduled_ir_file: str
    block_ir_file: str
    verilog_line_map_file: str
    scheduling_options_file: str
    codegen_options_file: str
    scheduling_pass_metrics_file: str
    codegen_pass_metrics_file: str
    dump_optimizer_ir: bool
    optimizer_ir_dump_dir: str | None
    dump_codegen_ir: bool
    codegen_ir_dump_dir: str | None
    profile_passes: bool
    optimizer_pass_profile_file: str | None
    codegen_pass_profile_file: str | None
    latency_cycles: int | None
    initiation_interval: int | None
    flop_count: int
    feedthrough_path_exists: bool
    metrics_delay_model: str | None
    max_reg_to_reg_delay_ps: int | None
    max_input_to_reg_delay_ps: int | None
    max_reg_to_output_delay_ps: int | None
    max_feedthrough_path_delay_ps: int | None
    operation_counts: dict[str, int]
    operation_kind_counts: dict[str, int]


@dataclass(frozen=True)
class XLSBlockMetrics:
    """Target-independent metrics emitted by XLS code generation."""

    flop_count: int
    feedthrough_path_exists: bool
    delay_model: str | None
    max_reg_to_reg_delay_ps: int | None
    max_input_to_reg_delay_ps: int | None
    max_reg_to_output_delay_ps: int | None
    max_feedthrough_path_delay_ps: int | None
    operation_counts: dict[str, int]
    operation_kind_counts: dict[str, int]


def _extract_textproto_blocks(text: str, field_name: str) -> list[str]:
    """Return the contents of all ``field_name { ... }`` textproto blocks."""
    pattern = re.compile(rf"(?m)^\s*{re.escape(field_name)}\s*\{{")
    blocks: list[str] = []
    for match in pattern.finditer(text):
        opening_brace = text.find("{", match.start(), match.end())
        depth = 0
        for index in range(opening_brace, len(text)):
            character = text[index]
            if character == "{":
                depth += 1
            elif character == "}":
                depth -= 1
                if depth == 0:
                    blocks.append(text[opening_brace + 1 : index])
                    break
        else:
            raise ValueError(f"Unterminated `{field_name}` block in textproto.")
    return blocks


def _parse_textproto_int(text: str, field_name: str) -> int | None:
    match = re.search(rf"(?m)^\s*{re.escape(field_name)}:\s*(-?\d+)\s*$", text)
    return int(match.group(1)) if match else None


def _parse_textproto_bool(text: str, field_name: str) -> bool | None:
    match = re.search(
        rf"(?m)^\s*{re.escape(field_name)}:\s*(true|false)\s*$",
        text,
    )
    return match.group(1) == "true" if match else None


def _parse_textproto_string(text: str, field_name: str) -> str | None:
    match = re.search(
        rf'(?m)^\s*{re.escape(field_name)}:\s*"([^"\\]*(?:\\.[^"\\]*)*)"\s*$',
        text,
    )
    return match.group(1) if match else None


def _parse_textproto_enum(text: str, field_name: str) -> str | None:
    match = re.search(
        rf"(?m)^\s*{re.escape(field_name)}:\s*([A-Z][A-Z0-9_]*)\s*$",
        text,
    )
    return match.group(1) if match else None


def parse_xls_module_signature(path: Path) -> tuple[int | None, int | None]:
    """Read pipeline latency and initiation interval from an XLS signature."""
    pipeline_blocks = _extract_textproto_blocks(path.read_text(), "pipeline")
    if not pipeline_blocks:
        return None, None
    pipeline = pipeline_blocks[0]
    return (
        _parse_textproto_int(pipeline, "latency"),
        _parse_textproto_int(pipeline, "initiation_interval"),
    )


def parse_xls_block_metrics(path: Path) -> XLSBlockMetrics:
    """Read code-generation metrics and aggregate XLS BOM entries."""
    text = path.read_text()
    metric_blocks = _extract_textproto_blocks(text, "block_metrics")
    if not metric_blocks:
        raise ValueError(f"No `block_metrics` block found in {path}.")
    metrics = metric_blocks[0]

    operation_counts: Counter[str] = Counter()
    operation_kind_counts: Counter[str] = Counter()
    for entry in _extract_textproto_blocks(metrics, "bill_of_materials"):
        operation = _parse_textproto_enum(entry, "op")
        operation_kind = _parse_textproto_enum(entry, "kind")
        if operation is not None:
            operation_counts[operation] += 1
        if operation_kind is not None:
            operation_kind_counts[operation_kind] += 1

    return XLSBlockMetrics(
        # Proto3 omits scalar fields whose value is the default.
        flop_count=_parse_textproto_int(metrics, "flop_count") or 0,
        feedthrough_path_exists=(
            _parse_textproto_bool(metrics, "feedthrough_path_exists") or False
        ),
        delay_model=_parse_textproto_string(metrics, "delay_model"),
        max_reg_to_reg_delay_ps=_parse_textproto_int(
            metrics,
            "max_reg_to_reg_delay_ps",
        ),
        max_input_to_reg_delay_ps=_parse_textproto_int(
            metrics,
            "max_input_to_reg_delay_ps",
        ),
        max_reg_to_output_delay_ps=_parse_textproto_int(
            metrics,
            "max_reg_to_output_delay_ps",
        ),
        max_feedthrough_path_delay_ps=_parse_textproto_int(
            metrics,
            "max_feedthrough_path_delay_ps",
        ),
        operation_counts=dict(sorted(operation_counts.items())),
        operation_kind_counts=dict(sorted(operation_kind_counts.items())),
    )


class XLSHLSSynthFlow(ToolFlow):
    """Compile a DSLX function to optimized XLS IR and synthesizable Verilog."""

    name = "XLSHLSSynthFlow"

    def __init__(
        self,
        xls_install_dir: str | Path | None = None,
        log_output: bool = False,
        log_execution_time: bool = True,
    ) -> None:
        self.xls_install_dir = get_xls_install_dir(xls_install_dir)
        self.log_output = log_output
        self.log_execution_time = log_execution_time

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        design_dir = design.dir
        data_file = design_dir / "data_hls.json"

        config = design.require_config()
        flow_config = config.require_flow_config(FlowName.XLS_HLS_SYNTH)
        dslx_file_name = flow_config.require_setting("dslx_file")
        top = flow_config.require_setting("top")
        generator = flow_config.get_setting("generator", "pipeline")
        if generator not in {"pipeline", "combinational"}:
            raise ValueError(
                "XLS code generator must be `pipeline` or `combinational`; "
                f"got {generator!r}.",
            )

        dslx_file = design_dir / dslx_file_name
        if not dslx_file.is_file():
            raise FileNotFoundError(
                f"DSLX source file {dslx_file} does not exist. This file is "
                "required for XLS synthesis.",
            )

        artifact_stem = dslx_file.stem
        ir_file = design_dir / f"{artifact_stem}.ir"
        optimized_ir_file = design_dir / f"{artifact_stem}.opt.ir"
        rtl_file = design_dir / f"{artifact_stem}.v"
        module_signature_file = design_dir / f"{artifact_stem}.signature.textproto"
        block_metrics_file = design_dir / f"{artifact_stem}.block_metrics.textproto"
        interface_proto_file = design_dir / f"{artifact_stem}.interface.pb"
        interface_textproto_file = design_dir / f"{artifact_stem}.interface.textproto"
        ir_converter_options_file = (
            design_dir / f"{artifact_stem}.ir_converter_options.textproto"
        )
        optimizer_options_file = (
            design_dir / f"{artifact_stem}.optimizer_options.textproto"
        )
        optimization_pass_metrics_file = (
            design_dir / f"{artifact_stem}.optimization_pass_metrics.textproto"
        )
        schedule_file = design_dir / f"{artifact_stem}.schedule.textproto"
        scheduled_ir_file = design_dir / f"{artifact_stem}.scheduled.ir"
        block_ir_file = design_dir / f"{artifact_stem}.block.ir"
        verilog_line_map_file = (
            design_dir / f"{artifact_stem}.verilog_line_map.textproto"
        )
        scheduling_options_file = (
            design_dir / f"{artifact_stem}.scheduling_options.textproto"
        )
        codegen_options_file = design_dir / f"{artifact_stem}.codegen_options.textproto"
        scheduling_pass_metrics_file = (
            design_dir / f"{artifact_stem}.scheduling_pass_metrics.textproto"
        )
        codegen_pass_metrics_file = (
            design_dir / f"{artifact_stem}.codegen_pass_metrics.textproto"
        )
        optimizer_ir_dump_dir = design_dir / f"{artifact_stem}.optimizer_ir_dumps"
        codegen_ir_dump_dir = design_dir / f"{artifact_stem}.codegen_ir_dumps"
        optimizer_pass_profile_file = (
            design_dir / f"{artifact_stem}.optimizer_passes.pprof"
        )
        codegen_pass_profile_file = design_dir / f"{artifact_stem}.codegen_passes.pprof"

        dump_optimizer_ir = parse_xls_bool_setting(
            flow_config.get_setting("dump_optimizer_ir"),
            "dump_optimizer_ir",
        )
        dump_codegen_ir = parse_xls_bool_setting(
            flow_config.get_setting("dump_codegen_ir"),
            "dump_codegen_ir",
        )
        profile_passes = parse_xls_bool_setting(
            flow_config.get_setting("profile_passes"),
            "profile_passes",
        )

        pipeline_stages: int | None = None
        delay_model: str | None = None
        reset = flow_config.get_setting("reset")
        if reset and generator != "pipeline":
            raise ValueError("XLS `reset` is only supported by the pipeline generator.")
        if generator == "pipeline":
            pipeline_stages_value = flow_config.get_setting("pipeline_stages", "1")
            assert pipeline_stages_value is not None
            try:
                pipeline_stages = int(pipeline_stages_value)
            except ValueError as error:
                raise ValueError(
                    "XLS `pipeline_stages` must be a positive integer; "
                    f"got {pipeline_stages_value!r}.",
                ) from error
            if pipeline_stages < 1:
                raise ValueError(
                    "XLS `pipeline_stages` must be a positive integer; "
                    f"got {pipeline_stages}.",
                )
            delay_model = flow_config.get_setting("delay_model", "unit")

        expected_artifacts = (
            ir_file,
            optimized_ir_file,
            rtl_file,
            module_signature_file,
            block_metrics_file,
            interface_proto_file,
            interface_textproto_file,
            ir_converter_options_file,
            optimizer_options_file,
            optimization_pass_metrics_file,
            scheduled_ir_file,
            block_ir_file,
            verilog_line_map_file,
            scheduling_options_file,
            codegen_options_file,
            scheduling_pass_metrics_file,
            codegen_pass_metrics_file,
        )
        if generator == "pipeline":
            expected_artifacts += (schedule_file,)
        if profile_passes:
            expected_artifacts += (
                optimizer_pass_profile_file,
                codegen_pass_profile_file,
            )
        expected_dump_dirs = tuple(
            path
            for enabled, path in (
                (dump_optimizer_ir, optimizer_ir_dump_dir),
                (dump_codegen_ir, codegen_ir_dump_dir),
            )
            if enabled
        )

        if (
            flow_already_completed(
                design_dir,
                self.name,
                success_marker_fp=data_file,
            )
            and all(path.is_file() for path in expected_artifacts)
            and all(path.is_dir() for path in expected_dump_dirs)
        ):
            print(f"[{design_dir}] Skipping {self.name}, already completed")
            return [design]

        error_marker = design_dir / f"error__{self.name}.txt"
        timeout_marker = design_dir / f"timeout__{self.name}.txt"
        generated_files = (
            data_file,
            ir_file,
            optimized_ir_file,
            rtl_file,
            module_signature_file,
            block_metrics_file,
            interface_proto_file,
            interface_textproto_file,
            ir_converter_options_file,
            optimizer_options_file,
            optimization_pass_metrics_file,
            schedule_file,
            scheduled_ir_file,
            block_ir_file,
            verilog_line_map_file,
            scheduling_options_file,
            codegen_options_file,
            scheduling_pass_metrics_file,
            codegen_pass_metrics_file,
            optimizer_pass_profile_file,
            codegen_pass_profile_file,
            error_marker,
            timeout_marker,
        )
        for stale_file in generated_files:
            stale_file.unlink(missing_ok=True)
        for stale_dir in (optimizer_ir_dump_dir, codegen_ir_dump_dir):
            if stale_dir.is_dir():
                shutil.rmtree(stale_dir)
        if dump_optimizer_ir:
            optimizer_ir_dump_dir.mkdir()
        if dump_codegen_ir:
            codegen_ir_dump_dir.mkdir()

        ir_converter = find_xls_binary(self.xls_install_dir, "ir_converter_main")
        optimizer = find_xls_binary(self.xls_install_dir, "opt_main")
        code_generator = find_xls_binary(self.xls_install_dir, "codegen_main")

        ir_converter_command = [
            str(ir_converter),
            f"--top={top}",
            f"--output_file={ir_file.name}",
            f"--interface_proto_file={interface_proto_file.name}",
            f"--interface_textproto_file={interface_textproto_file.name}",
            "--ir_converter_options_used_textproto_file="
            f"{ir_converter_options_file.name}",
            dslx_file.name,
        ]
        optimizer_command = [
            str(optimizer),
            f"--output_path={optimized_ir_file.name}",
            f"--opt_options_used_textproto_file={optimizer_options_file.name}",
            f"--pass_metrics_path={optimization_pass_metrics_file.name}",
        ]
        if dump_optimizer_ir:
            optimizer_command.append(f"--ir_dump_path={optimizer_ir_dump_dir.name}")
        if profile_passes:
            optimizer_command.append(
                f"--passes_profile={optimizer_pass_profile_file.name}",
            )
        optimizer_command.append(ir_file.name)

        commands = [
            ("DSLX to IR conversion", shlex.join(ir_converter_command)),
            ("IR optimization", shlex.join(optimizer_command)),
        ]

        codegen_command = [
            str(code_generator),
            f"--generator={generator}",
            f"--output_verilog_path={rtl_file.name}",
            f"--output_signature_path={module_signature_file.name}",
            f"--block_metrics_path={block_metrics_file.name}",
            f"--ir_interface_proto={interface_proto_file.name}",
            f"--output_schedule_ir_path={scheduled_ir_file.name}",
            f"--output_block_ir_path={block_ir_file.name}",
            f"--output_verilog_line_map_path={verilog_line_map_file.name}",
            f"--scheduling_options_used_textproto_file={scheduling_options_file.name}",
            f"--codegen_options_used_textproto_file={codegen_options_file.name}",
            "--output_scheduling_pass_metrics_path="
            f"{scheduling_pass_metrics_file.name}",
            f"--output_codegen_pass_metrics_path={codegen_pass_metrics_file.name}",
        ]
        if generator == "pipeline":
            codegen_command.extend(
                (
                    f"--pipeline_stages={pipeline_stages}",
                    f"--delay_model={delay_model}",
                    f"--output_schedule_path={schedule_file.name}",
                ),
            )
            if reset:
                codegen_command.append(f"--reset={reset}")
        if dump_codegen_ir:
            codegen_command.append(f"--ir_dump_path={codegen_ir_dump_dir.name}")
        if profile_passes:
            codegen_command.append(
                f"--passes_profile={codegen_pass_profile_file.name}",
            )
        codegen_command.append(optimized_ir_file.name)
        commands.append(("RTL generation", shlex.join(codegen_command)))

        start_time = time.perf_counter()
        for stage_name, command in commands:
            stage_timeout = self._remaining_timeout(start_time, timeout)
            if stage_timeout == 0:
                result = CallToolResult.TIMEOUT
            else:
                result = call_tool(
                    command,
                    cwd=design_dir,
                    log_output=self.log_output,
                    timeout=stage_timeout,
                    raise_on_error=False,
                )

            if result == CallToolResult.TIMEOUT:
                timeout_marker.write_text(f"Timed out during {stage_name}.\n")
                print(f"[{design_dir}] Timeout of {timeout} seconds reached")
                self._log_execution_time(design_dir, start_time)
                return []
            if result == CallToolResult.ERROR:
                error_marker.write_text(f"XLS failed during {stage_name}.\n")
                print(f"[{design_dir}] XLS failed during {stage_name}")
                self._log_execution_time(design_dir, start_time)
                return []

        missing_artifacts = [path for path in expected_artifacts if not path.is_file()]
        missing_dump_dirs = [path for path in expected_dump_dirs if not path.is_dir()]
        if missing_artifacts or missing_dump_dirs:
            missing_names = ", ".join(
                path.name for path in (*missing_artifacts, *missing_dump_dirs)
            )
            error_marker.write_text(
                f"XLS completed without producing expected artifacts: {missing_names}.\n",
            )
            print(f"[{design_dir}] XLS did not produce: {missing_names}")
            self._log_execution_time(design_dir, start_time)
            return []

        try:
            latency_cycles, initiation_interval = parse_xls_module_signature(
                module_signature_file,
            )
            block_metrics = parse_xls_block_metrics(block_metrics_file)
        except (OSError, ValueError) as error:
            error_marker.write_text(f"Could not parse XLS metrics: {error}\n")
            print(f"[{design_dir}] Could not parse XLS metrics: {error}")
            self._log_execution_time(design_dir, start_time)
            return []

        synthesis_data = DesignHLSSynthData(
            dslx_file=dslx_file.name,
            top=top,
            generator=generator,
            pipeline_stages=pipeline_stages,
            delay_model=delay_model,
            reset=reset,
            ir_file=ir_file.name,
            optimized_ir_file=optimized_ir_file.name,
            rtl_file=rtl_file.name,
            module_signature_file=module_signature_file.name,
            block_metrics_file=block_metrics_file.name,
            interface_proto_file=interface_proto_file.name,
            interface_textproto_file=interface_textproto_file.name,
            ir_converter_options_file=ir_converter_options_file.name,
            optimizer_options_file=optimizer_options_file.name,
            optimization_pass_metrics_file=optimization_pass_metrics_file.name,
            schedule_file=schedule_file.name if generator == "pipeline" else None,
            scheduled_ir_file=scheduled_ir_file.name,
            block_ir_file=block_ir_file.name,
            verilog_line_map_file=verilog_line_map_file.name,
            scheduling_options_file=scheduling_options_file.name,
            codegen_options_file=codegen_options_file.name,
            scheduling_pass_metrics_file=scheduling_pass_metrics_file.name,
            codegen_pass_metrics_file=codegen_pass_metrics_file.name,
            dump_optimizer_ir=dump_optimizer_ir,
            optimizer_ir_dump_dir=(
                optimizer_ir_dump_dir.name if dump_optimizer_ir else None
            ),
            dump_codegen_ir=dump_codegen_ir,
            codegen_ir_dump_dir=(codegen_ir_dump_dir.name if dump_codegen_ir else None),
            profile_passes=profile_passes,
            optimizer_pass_profile_file=(
                optimizer_pass_profile_file.name if profile_passes else None
            ),
            codegen_pass_profile_file=(
                codegen_pass_profile_file.name if profile_passes else None
            ),
            latency_cycles=latency_cycles,
            initiation_interval=initiation_interval,
            flop_count=block_metrics.flop_count,
            feedthrough_path_exists=block_metrics.feedthrough_path_exists,
            metrics_delay_model=block_metrics.delay_model,
            max_reg_to_reg_delay_ps=block_metrics.max_reg_to_reg_delay_ps,
            max_input_to_reg_delay_ps=block_metrics.max_input_to_reg_delay_ps,
            max_reg_to_output_delay_ps=block_metrics.max_reg_to_output_delay_ps,
            max_feedthrough_path_delay_ps=(block_metrics.max_feedthrough_path_delay_ps),
            operation_counts=block_metrics.operation_counts,
            operation_kind_counts=block_metrics.operation_kind_counts,
        )
        synthesis_data.to_json(data_file)  # type: ignore[attr-defined]
        self._log_execution_time(design_dir, start_time)
        return [design]

    @staticmethod
    def _remaining_timeout(start_time: float, timeout: float | None) -> float | None:
        if timeout is None:
            return None
        return max(0.0, timeout - (time.perf_counter() - start_time))

    def _log_execution_time(self, design_dir: Path, start_time: float) -> None:
        if self.log_execution_time:
            log_execution_time_to_file(
                design_dir,
                self.name,
                start_time,
                time.perf_counter(),
            )


__all__ = [
    "DEFAULT_XLS_PATH",
    "DesignHLSSynthData",
    "XLSBlockMetrics",
    "XLSHLSSynthFlow",
    "HLSFACTORY_XLS_PATH_ENV_VAR",
    "find_xls_binary",
    "get_xls_install_dir",
    "parse_xls_bool_setting",
    "parse_xls_block_metrics",
    "parse_xls_module_signature",
]
