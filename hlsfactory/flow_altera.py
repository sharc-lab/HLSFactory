"""Altera HLS IP Gen (SYCL/ahls) synthesis, emulation, RTL simulation and Quartus."""

from __future__ import annotations

import json
import os
import re
import shlex
import shutil
import tempfile
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Any

from hlsfactory.design_config import FlowName
from hlsfactory.framework import Design, ToolFlow
from hlsfactory.utils import (
    CallToolResult,
    ExecutionDataStatus,
    call_tool,
    read_execution_data,
    serialize_methods_for_dataclass,
    update_execution_data_with_flow_results,
)

HLSFACTORY_ALTERA_HLS_PATH_ENV_VAR = "HLSFACTORY_ALTERA_HLS_PATH"
HLSFACTORY_ALTERA_QUESTA_SETUP_ENV_VAR = "HLSFACTORY_ALTERA_QUESTA_SETUP"

# New Siemens Questa writes questa.ini, but ahls 2026.1 bundles modelsim.ini.
# Run in the compiler's own scratch directory, after mappings are finalized.
_QUESTA_INI_COMPAT_SCRIPT = """#!/bin/bash
"${HLSFACTORY_ALTERA_REAL_VSIM:?}" "$@"
questa_status=$?
if [[ $questa_status == 0 && -f questa.ini && ! -e modelsim.ini && ! -L modelsim.ini ]]; then
    ln -s questa.ini modelsim.ini || exit $?
fi
exit "$questa_status"
"""


def read_altera_report(path: Path) -> list[dict[str, Any]]:
    """Read the objects in a 2026 HLS IP Gen NDJSON report."""
    with path.open(encoding="utf-8") as stream:
        records = [json.loads(line) for line in stream if line.strip()]
    if not records or any(not isinstance(row, dict) for row in records):
        raise ValueError(f"Expected report objects in {path}")
    return records


@serialize_methods_for_dataclass
@dataclass
class AlteraHLSSynthData:
    resources_ALUTs_used: int
    resources_FFs_used: int
    resources_RAMs_used: int
    resources_DSPs_used: int
    resources_MLABs_used: int
    resources_ALUTs_avail: int
    resources_FFs_avail: int
    resources_RAMs_avail: int
    resources_DSPs_avail: int
    resources_MLABs_avail: int

    @classmethod
    def parse_from_synth_report_file(cls, data_file: Path) -> AlteraHLSSynthData:
        records = read_altera_report(data_file)
        headers = [r for r in records if r.get("summary_type") == "estimatedResources"]
        if len(headers) != 1:
            raise ValueError(f"Missing or ambiguous resource table in {data_file}")
        # The first column labels the row; it is not an entry in data.
        columns = [name.strip() for name in headers[0]["columns"][1:]]
        values = {}
        for row_name, suffix in (("Total", "used"), ("Available", "avail")):
            rows = [
                r
                for r in records
                if r.get("parent") == "estimatedResources" and r.get("name") == row_name
            ]
            if len(rows) != 1 or len(rows[0]["data"]) != len(columns):
                raise ValueError(
                    f"Missing or malformed {row_name} resources in {data_file}"
                )
            row = dict(zip(columns, rows[0]["data"], strict=True))
            for resource in ("ALUTs", "FFs", "RAMs", "DSPs", "MLABs"):
                values[f"resources_{resource}_{suffix}"] = int(
                    str(row[resource]).replace(",", "")
                )
        return cls(**values)


@serialize_methods_for_dataclass
@dataclass
class AlteraHLSDesign:
    name: str
    target_clock: str | None
    family: str
    product: str
    quartus: str
    time: str
    version: str
    kernels: list[str]

    @classmethod
    def parse_from_synth_report_file(
        cls,
        info_json: Path,
        summary_json: Path,
    ) -> AlteraHLSDesign:
        info = next(
            r["compileInfo"]
            for r in read_altera_report(info_json)
            if "compileInfo" in r
        )["nodes"][0]
        clock = re.search(r"(?:-Xsclock=|-clock=|-clock\s+)(\S+)", info["command"])
        return cls(
            name=info["name"],
            target_clock=clock.group(1) if clock else None,
            family=info["family"],
            product=info["product"],
            quartus=info["quartus"],
            time=info["time"],
            version=info["version"],
            kernels=[
                r["name"]
                for r in read_altera_report(summary_json)
                if r.get("parent") == "performanceSummary"
            ],
        )


@serialize_methods_for_dataclass
@dataclass
class AlteraImplDesignResource:
    """Quartus fitter results, retaining each kernel/system row and clock."""

    resource_usage: list[dict[str, Any]]
    clock_frequencies: list[dict[str, Any]]

    @classmethod
    def parse_from_impl_report_file(
        cls, quartus_json: Path
    ) -> AlteraImplDesignResource:
        report = {}
        for record in read_altera_report(quartus_json):
            report.update(record)
        resources = report["quartusFitResourceUsageSummary"]["nodes"]
        clocks = report["quartusFitClockSummary"]["nodes"]
        # Early-link reports also contain this table, but only with TBD values.
        if not resources or not any(
            re.fullmatch(r"\d+(?:\.\d+)?", str(row.get("alm", "")).replace(",", ""))
            for row in resources
        ):
            raise ValueError(f"No completed Quartus fitter results in {quartus_json}")
        return cls(resource_usage=resources, clock_frequencies=clocks)


class _AlteraFlow(ToolFlow):
    flow_name: FlowName
    stage: str
    result_file: str
    questa_setup_script: Path | None = None

    def __init__(
        self,
        ahls_bin: str | Path | None = None,
        altera_install_dir: str | Path | None = None,
        ahls_sh: str | Path | None = None,
        log_output: bool = False,
        log_execution_time: bool = True,
    ) -> None:
        """Use an initialized native environment or the installer's container.

        Explicit compiler/launcher arguments take precedence. Otherwise resolve
        altera_install_dir, HLSFACTORY_ALTERA_HLS_PATH, then PATH.
        Native installations must have their vendor environment initialized.
        """
        if ahls_bin is not None and ahls_sh is not None:
            raise ValueError("Specify either ahls_bin or ahls_sh, not both")
        self.ahls_sh = str(Path(ahls_sh).expanduser().resolve()) if ahls_sh else None
        self.ahls_bin = str(ahls_bin) if ahls_bin else "ahls"
        if ahls_bin is None and ahls_sh is None:
            install = altera_install_dir or os.environ.get(
                HLSFACTORY_ALTERA_HLS_PATH_ENV_VAR
            )
            native = shutil.which("ahls") if install is None else None
            launcher = shutil.which("ahls-sh") if install is None else None
            if native:
                self.ahls_bin = native
            elif launcher:
                self.ahls_sh = launcher
            else:
                if not install:
                    raise FileNotFoundError(
                        "Altera HLS was not configured and neither ahls nor ahls-sh "
                        "was found on PATH. Set "
                        f"{HLSFACTORY_ALTERA_HLS_PATH_ENV_VAR}, altera_install_dir, "
                        "ahls_bin, or ahls_sh."
                    )
                root = Path(install).expanduser().resolve()
                if (root / "bin/ahls-sh").is_file():
                    self.ahls_sh = str(root / "bin/ahls-sh")
                else:
                    for relative in ("bin/ahls", "aclsycl/bin/ahls"):
                        if (root / relative).is_file():
                            self.ahls_bin = str(root / relative)
                            break
                    else:
                        raise FileNotFoundError(
                            f"No ahls or bin/ahls-sh in {root}. Set "
                            f"{HLSFACTORY_ALTERA_HLS_PATH_ENV_VAR}, ahls_bin, or ahls_sh."
                        )
        self.log_output = log_output
        self.log_execution_time = log_execution_time

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        design_dir = design.dir.resolve()
        result_path = design_dir / self.result_file
        try:
            previous = read_execution_data(design_dir, self.name)
        except (OSError, ValueError, KeyError, TypeError):
            previous = None
        # A failed or timed-out run must remain retryable.
        if (
            previous is not None
            and previous.status == ExecutionDataStatus.SUCCESS
            and result_path.is_file()
        ):
            return [design]

        settings = design.require_config().require_flow_config(self.flow_name)
        sources = shlex.split(settings.require_setting("sources"))
        if not sources:
            raise ValueError("Altera sources must contain at least one source file")
        for source in sources:
            if not (design_dir / source).is_file():
                raise FileNotFoundError(design_dir / source)

        waveform_depth = settings.get_setting("waveform_depth")
        if self.stage == "cosim" and waveform_depth is not None:
            if not re.fullmatch(r"[0-9]+", waveform_depth):
                raise ValueError("Altera waveform_depth must be a nonnegative integer")

        build_root = design_dir / f"altera_{self.stage}"
        build_root.mkdir(exist_ok=True)
        # Preserve old attempts for diagnosis and never consume their artifacts.
        build_dir = Path(tempfile.mkdtemp(prefix="run_", dir=build_root))
        relative_build = build_dir.relative_to(design_dir)
        output_name = {
            "csim": "design.fpga_emu",
            # Unknown extensions are retained in the compiler's project name.
            # An extensionless binary consistently produces design.prj.
            "cosim": "design",
        }.get(self.stage, "design.a")
        output = relative_build / output_name
        command = [
            self.ahls_bin,
            *sources,
            *shlex.split(settings.get_setting("cxxflags", "") or ""),
        ]
        if self.stage == "csim":
            command += ["-DFPGA_EMULATOR", "-Xsemulator"]
        else:
            command += [
                "-DFPGA_SIMULATOR" if self.stage == "cosim" else "-DFPGA_HARDWARE",
                "-Xssimulation" if self.stage == "cosim" else "-Xshardware",
                f"-Xstarget={settings.get_setting('target', 'Agilex7')}",
            ]
            clock = settings.get_setting("clock")
            if clock:
                command.append(f"-Xsclock={clock}")
            command += shlex.split(settings.get_setting("fpga_flags", "") or "")
            if self.stage == "cosim":
                if waveform_depth is not None:
                    command.append(f"-Xsghdl={waveform_depth}")
            else:
                command.append(
                    "-fsycl-link=early"
                    if self.stage == "synth"
                    else "-fsycl-link=image"
                )
        command += ["-o", str(output)]
        script = "#!/bin/bash\nset -e\n"
        if self.stage == "cosim":
            if self.questa_setup_script is not None:
                script += "source " + shlex.quote(str(self.questa_setup_script)) + "\n"
            compat_dir = build_dir / "questa_compat"
            compat_dir.mkdir()
            compat_vsim = compat_dir / "vsim"
            compat_vsim.write_text(_QUESTA_INI_COMPAT_SCRIPT, encoding="utf-8")
            compat_vsim.chmod(0o755)
            # Opt in from the trusted setup script; older vendor editions do
            # not need this. Scope both the override and alias to this attempt.
            script += (
                'if [[ "${HLSFACTORY_ALTERA_QUESTA_INI_COMPAT:-0}" == 1 ]]; then\n'
                '    HLSFACTORY_ALTERA_REAL_VSIM="$(command -v vsim)"\n'
                "    export HLSFACTORY_ALTERA_REAL_VSIM\n"
                "    export PATH=" + shlex.quote(str(compat_dir)) + ':"$PATH"\n'
                "fi\n"
            )
            # Qsys's Java helpers otherwise try to use a stale SSH X11 display.
            script += "unset DISPLAY\n"
        script += shlex.join(command) + "\n"
        if self.stage == "cosim":
            # Keep runtime side effects inside this attempt. Point the runtime
            # explicitly at its own XML rather than auto-discovering another run.
            script += "cd " + shlex.quote(str(relative_build)) + "\n"
            script += "test -f design.prj/ipinterfaces.xml\n"
            script += 'export INTELFPGA_SIM_DEVICE_SPEC_DIR="$PWD/design.prj"\n'
        if self.stage in {"csim", "cosim"}:
            script += (
                "exec "
                + shlex.join(
                    [
                        "./" + (output.name if self.stage == "cosim" else str(output)),
                        *shlex.split(settings.get_setting("run_args", "") or ""),
                    ]
                )
                + "\n"
            )
        script_path = relative_build / "run.sh"
        (design_dir / script_path).write_text(script, encoding="utf-8")
        # A script avoids the unquoted $@ in the vendor ahls-sh launcher.
        launch = ([self.ahls_sh] if self.ahls_sh else []) + ["bash", str(script_path)]
        log_path = build_dir / "flow.log"
        logged_command = (
            shlex.join(launch) + " > " + shlex.quote(str(log_path)) + " 2>&1"
        )
        started = time.perf_counter()
        status = ExecutionDataStatus.ERROR
        error = None
        try:
            outcome = call_tool(
                shlex.join(["bash", "-c", "exec " + logged_command]),
                cwd=design_dir,
                timeout=timeout,
                raise_on_error=False,
            )
            if outcome == CallToolResult.TIMEOUT:
                status = ExecutionDataStatus.TIMEOUT
                error = f"Timeout of {timeout}s reached; see {log_path}"
            elif outcome == CallToolResult.ERROR:
                error = f"Altera {self.stage} failed; see {log_path}"
            else:
                if not (design_dir / output).is_file():
                    raise FileNotFoundError(f"Compiler did not produce {output}")
                self._collect_results(design_dir, output, settings.get_setting("clock"))
                status = ExecutionDataStatus.SUCCESS
        except (
            OSError,
            ValueError,
            KeyError,
            IndexError,
            TypeError,
            StopIteration,
        ) as exc:
            error = f"Altera {self.stage}: {exc}; see {log_path}"
        if self.log_output and log_path.exists():
            print(log_path.read_text(encoding="utf-8", errors="replace"))
        if self.log_execution_time:
            update_execution_data_with_flow_results(
                design_dir,
                self.name,
                status,
                started,
                time.perf_counter(),
                error_message=error,
            )
        if status != ExecutionDataStatus.SUCCESS:
            (design_dir / f"{status.value}__{self.name}.txt").write_text(
                error or "", encoding="utf-8"
            )
            print(f"[{design_dir}] {error}")
            return []
        for marker in ("error", "timeout"):
            (design_dir / f"{marker}__{self.name}.txt").unlink(missing_ok=True)
        return [design]

    def _collect_results(
        self, design_dir: Path, output: Path, clock: str | None
    ) -> None:
        if self.stage == "csim":
            data = {"status": "success", "executable": str(output)}
        elif self.stage == "cosim":
            project = output.with_suffix(".prj")
            for required in ("ipinterfaces.xml", "reports/report.html"):
                if not (design_dir / project / required).is_file():
                    raise FileNotFoundError(design_dir / project / required)
            data = {
                "status": "success",
                "executable": str(output),
                "project_dir": str(project),
                "report": str(project / "reports/report.html"),
                "interface_spec": str(project / "ipinterfaces.xml"),
                "statistics_files": [
                    str(project / "reports/resources/json" / filename)
                    for filename in ("sim_stats.ndjson", "simulation_raw.ndjson")
                    if (
                        design_dir / project / "reports/resources/json" / filename
                    ).is_file()
                ],
                "waveform_files": sorted(
                    str(p.relative_to(design_dir))
                    for p in (design_dir / output.parent).rglob("*.wlf")
                ),
                "log_files": sorted(
                    str(p.relative_to(design_dir))
                    for p in (design_dir / output.parent).rglob("*.log")
                ),
            }
        else:
            project = output.with_suffix(".prj")
            reports = project / "reports/resources/json"
            summary = design_dir / reports / "summary.ndjson"
            synth_data = AlteraHLSSynthData.parse_from_synth_report_file(summary)
            design_data = AlteraHLSDesign.parse_from_synth_report_file(
                design_dir / reports / "info.ndjson",
                summary,
            )
            if clock:
                design_data.target_clock = clock
            rtl = sorted(
                str(p.relative_to(design_dir))
                for p in (design_dir / project).rglob("*.sv")
            )
            rtl += sorted(
                str(p.relative_to(design_dir))
                for p in (design_dir / project).rglob("*.v")
            )
            if not rtl or not (design_dir / project / "reports/report.html").is_file():
                raise ValueError(f"Missing generated RTL or HTML report in {project}")
            data = {
                "archive": str(output),
                "project_dir": str(project),
                "report": str(project / "reports/report.html"),
                "rtl_files": rtl,
            }
            if self.stage == "impl":
                impl = AlteraImplDesignResource.parse_from_impl_report_file(
                    design_dir / reports / "quartus.ndjson",
                )
                impl.to_json(design_dir / "data_implementation.json")
            else:
                synth_data.to_json(design_dir / "data_hls.json")
                design_data.to_json(design_dir / "data_design.json")
        (design_dir / self.result_file).write_text(
            json.dumps(data, indent=4), encoding="utf-8"
        )


class AlteraHLSSynthFlow(_AlteraFlow):
    """Generate RTL IP, optimization reports and estimated resource metrics."""

    name = "AlteraHLSSynthFlow"
    flow_name = FlowName.ALTERA_HLS_SYNTH
    stage = "synth"
    result_file = "data_altera_synth.json"


class AlteraHLSCsimFlow(_AlteraFlow):
    """Compile and run the SYCL host testbench on the FPGA emulator."""

    name = "AlteraHLSCsimFlow"
    flow_name = FlowName.ALTERA_HLS_CSIM
    stage = "csim"
    result_file = "data_altera_csim.json"


class AlteraQuartusImplFlow(_AlteraFlow):
    """Compile a hardware device image through Quartus; requires Quartus licensing."""

    name = "AlteraQuartusImplFlow"
    flow_name = FlowName.ALTERA_QUARTUS_IMPL
    stage = "impl"
    result_file = "data_altera_impl.json"


class AlteraHLSCosimFlow(_AlteraFlow):
    """Compile and run the host testbench against RTL in Questa."""

    name = "AlteraHLSCosimFlow"
    flow_name = FlowName.ALTERA_HLS_COSIM
    stage = "cosim"
    result_file = "data_altera_cosim.json"

    def __init__(
        self,
        ahls_bin: str | Path | None = None,
        altera_install_dir: str | Path | None = None,
        ahls_sh: str | Path | None = None,
        log_output: bool = False,
        log_execution_time: bool = True,
        questa_setup_script: str | Path | None = None,
    ) -> None:
        """Optionally source a trusted Bash simulator setup after vendor setup.

        The script configures PATH and licensing for both compilation and runtime.
        It must be accessible at the same absolute path inside the container.
        """
        super().__init__(
            ahls_bin, altera_install_dir, ahls_sh, log_output, log_execution_time
        )
        setup = questa_setup_script or os.environ.get(
            HLSFACTORY_ALTERA_QUESTA_SETUP_ENV_VAR
        )
        if setup is not None:
            self.questa_setup_script = Path(setup).expanduser().resolve()
            if not self.questa_setup_script.is_file():
                raise FileNotFoundError(self.questa_setup_script)
