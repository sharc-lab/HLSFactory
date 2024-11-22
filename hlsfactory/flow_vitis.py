import json
import os
import re
import time
import xml.etree.ElementTree as ET
from dataclasses import dataclass
from pathlib import Path

from hlsfactory.framework import Design, ToolFlow
from hlsfactory.utils import (
    CallToolResult,
    call_tool,
    find_bin_path,
    log_execution_time_to_file,
    serialize_methods_for_dataclass,
    timeout_not_supported,
)


def print_xml_element(node: ET.Element) -> None:
    print("".join(node.itertext()))


def auto_find_solutions(dir_path: Path) -> list[Path]:
    solutions = list(dir_path.rglob("**/*.aps"))
    solutions = list(
        filter(
            lambda x: (
                '<AutoPilot:solution xmlns:AutoPilot="com.autoesl.autopilot.solution">'
            )
            in x.read_text(),
            solutions,
        ),
    )
    if len(solutions) == 0:
        raise FileNotFoundError(f"No solution files found in {dir_path}")

    solution_dirs = [x.parent for x in solutions]
    for solution_dir in solution_dirs:
        if not solution_dir.is_dir():
            raise FileNotFoundError(f"Solution directory {solution_dir} does not exist")

    return solution_dirs


def auto_find_synth_report(dir_path: Path) -> Path:
    report_results = list(dir_path.rglob("**/csynth.xml"))
    if len(report_results) == 0:
        raise FileNotFoundError(f"No csynth.xml report file found in {dir_path}")
    if len(report_results) > 1:
        print(
            f"Found multiple csynth.xml report files in {dir_path}. "
            f"Using the first one: {report_results[0]}",
        )
    return report_results[0]


def auto_find_multiple_synth_report(dir_path: Path) -> list[Path]:
    report_results = list(dir_path.rglob("**/csynth.xml"))
    if len(report_results) == 0:
        raise FileNotFoundError(f"No csynth.xml report file found in {dir_path}")
    return report_results


@serialize_methods_for_dataclass
@dataclass
class DesignHLSSynthData:
    clock_period: float

    latency_best_cycles: int | None
    latency_best_seconds: float | None
    latency_average_cycles: int | None
    latency_average_seconds: float | None
    latency_worst_cycles: int | None
    latency_worst_seconds: float | None

    resources_lut_used: int
    resources_ff_used: int
    resources_dsp_used: int
    resources_bram_used: int
    resources_uram_used: int

    # resources_lut_total: int  # noqa: ERA001
    # resources_ff_total: int  # noqa: ERA001
    # resources_dsp_total: int  # noqa: ERA001
    # resources_bram_total: int  # noqa: ERA001
    # resources_uram_total: int  # noqa: ERA001

    # resources_lut_fraction_used: float  # noqa: ERA001
    # resources_ff_fraction_used: float  # noqa: ERA001
    # resources_dsp_fraction_used: float  # noqa: ERA001
    # resources_bram_fraction_used: float  # noqa: ERA001
    # resources_uram_fraction_used: float  # noqa: ERA001

    @classmethod
    def parse_from_synth_report_file(cls, fp: Path) -> "DesignHLSSynthData":
        tree = ET.parse(fp)
        root = tree.getroot()

        # Gather latency data
        performance_estimates = root.find("PerformanceEstimates")

        summary_of_timing_analysis = performance_estimates.find(  # type: ignore
            "SummaryOfTimingAnalysis",
        )
        clock_units = str(summary_of_timing_analysis.find("unit").text)  # type: ignore
        clock_period = float(
            summary_of_timing_analysis.find("EstimatedClockPeriod").text,  # type: ignore
        )
        unit_scaler = 1
        if clock_units == "ns":
            unit_scaler = 1e-9
        elif clock_units == "us":
            unit_scaler = 1e-6
        elif clock_units == "ms":
            unit_scaler = 1e-3
        else:
            raise NotImplementedError(f"Unknown clock unit: {clock_units}")

        clock_period_t = clock_period * unit_scaler

        summary_of_overall_latency = performance_estimates.find(  # type: ignore
            "SummaryOfOverallLatency",
        )
        latency_data = {}
        # fmt: off
        best_case_latency = summary_of_overall_latency.find("Best-caseLatency").text # type: ignore
        try:
            latency_data["best_case_latency"] = int(best_case_latency) # type: ignore
        except ValueError:
            latency_data["best_case_latency"] = None
        average_case_latency = summary_of_overall_latency.find("Average-caseLatency").text # type: ignore
        try:
            latency_data["average_case_latency"] = int(average_case_latency) # type: ignore
        except ValueError:
            latency_data["average_case_latency"] = None
        worst_case_latency = summary_of_overall_latency.find("Worst-caseLatency").text # type: ignore
        try:
            latency_data["worst_case_latency"] = int(worst_case_latency) # type: ignore
        except ValueError:
            latency_data["worst_case_latency"] = None

        if latency_data["best_case_latency"] is not None:
            latency_data["best_case_latency_t"] = latency_data["best_case_latency"] * clock_period_t
        else:
            latency_data["best_case_latency_t"] = None
        if latency_data["average_case_latency"] is not None:
            latency_data["average_case_latency_t"] = latency_data["average_case_latency"] * clock_period_t
        else:
            latency_data["average_case_latency_t"] = None
        if latency_data["worst_case_latency"] is not None:
            latency_data["worst_case_latency_t"] = latency_data["worst_case_latency"] * clock_period_t
        else:
            latency_data["worst_case_latency_t"] = None
        # fmt: on

        # Gather resource data
        area_estimates = root.find("AreaEstimates")
        resource_data = {}
        # fmt: off
        resource_data["used_abs"] = {}
        resource_data["used_abs"]["BRAM_18K"] = int( area_estimates.find("Resources").find("BRAM_18K").text ) # type: ignore
        resource_data["used_abs"]["DSP"] = int(area_estimates.find("Resources").find("DSP").text) # type: ignore
        resource_data["used_abs"]["FF"] = int(area_estimates.find("Resources").find("FF").text) # type: ignore
        resource_data["used_abs"]["LUT"] = int(area_estimates.find("Resources").find("LUT").text) # type: ignore
        resource_data["used_abs"]["URAM"] = int( area_estimates.find("Resources").find("URAM").text ) # type: ignore
        # resource_data["available_abs"] = {}  # noqa: ERA001
        # resource_data["available_abs"]["BRAM_18K"] = int( area_estimates.find("AvailableResources").find("BRAM_18K").text )  # noqa: ERA001, E501
        # resource_data["available_abs"]["DSP"] = int( area_estimates.find("AvailableResources").find("DSP").text )  # noqa: ERA001, E501
        # resource_data["available_abs"]["FF"] = int( area_estimates.find("AvailableResources").find("FF").text )  # noqa: ERA001, E501
        # resource_data["available_abs"]["LUT"] = int( area_estimates.find("AvailableResources").find("LUT").text )  # noqa: ERA001, E501
        # resource_data["available_abs"]["URAM"] = int( area_estimates.find("AvailableResources").find("URAM").text )  # noqa: ERA001, E501
        # resource_data["used_percent"] = {}  # noqa: ERA001
        # resource_data["used_percent"]["BRAM_18K"] = float(resource_data["used_abs"]["BRAM_18K"] / resource_data["available_abs"]["BRAM_18K"])  # noqa: ERA001, E501
        # resource_data["used_percent"]["DSP"] = float(resource_data["used_abs"]["DSP"] / resource_data["available_abs"]["DSP"])  # noqa: ERA001, E501
        # resource_data["used_percent"]["FF"] = float(resource_data["used_abs"]["FF"] / resource_data["available_abs"]["FF"])  # noqa: ERA001, E501
        # resource_data["used_percent"]["LUT"] = float(resource_data["used_abs"]["LUT"] / resource_data["available_abs"]["LUT"])  # noqa: ERA001, E501
        # resource_data["used_percent"]["URAM"] = float(resource_data["used_abs"]["URAM"] / resource_data["available_abs"]["URAM"])  # noqa: ERA001, E501
        # fmt: on

        return cls(
            clock_period=clock_period_t,
            latency_best_cycles=latency_data["best_case_latency"],
            latency_best_seconds=latency_data["best_case_latency_t"],
            latency_average_cycles=latency_data["average_case_latency"],
            latency_average_seconds=latency_data["average_case_latency_t"],
            latency_worst_cycles=latency_data["worst_case_latency"],
            latency_worst_seconds=latency_data["worst_case_latency_t"],
            resources_lut_used=resource_data["used_abs"]["LUT"],
            resources_ff_used=resource_data["used_abs"]["FF"],
            resources_dsp_used=resource_data["used_abs"]["DSP"],
            resources_bram_used=resource_data["used_abs"]["BRAM_18K"],
            resources_uram_used=resource_data["used_abs"]["URAM"],
        )


@serialize_methods_for_dataclass
@dataclass
class VitisHLSDesign:
    name: str
    part: str
    target_clock_period: float
    version_vitis_hls: None | str
    version_vivado: None | str

    @classmethod
    def parse_from_synth_report_file(cls, fp: Path) -> "VitisHLSDesign":
        tree = ET.parse(fp)
        root = tree.getroot()
        vitis_hls_version = root.find("ReportVersion").find("Version").text  # type: ignore
        user_assignments = root.find("UserAssignments")  # type: ignore
        part = user_assignments.find("Part").text  # type: ignore
        target_clock_period = float(user_assignments.find("TargetClockPeriod").text)  # type: ignore
        name = root.find("RTLDesignHierarchy").find("TopModule").find("ModuleName").text  # type: ignore
        return cls(
            name=name,  # type: ignore
            part=part,  # type: ignore
            target_clock_period=target_clock_period,
            version_vitis_hls=vitis_hls_version,
            version_vivado=None,
        )


def auto_find_exported_ip(dir_path: Path) -> list[Path]:
    ip_dirs = list(dir_path.rglob("**/ip"))
    ip_dirs = list(filter(lambda x: x.is_dir(), ip_dirs))
    return ip_dirs  # noqa: RET504


def check_build_files_exist(build_files: list[Path]) -> None:
    for fp in build_files:
        if not fp.exists():
            raise FileNotFoundError(
                f"Build file {fp} does not exist. This build file is required for the"
                " build process of all designs.",
            )


def check_files_exist(files: list[Path]) -> None:
    for fp in files:
        if not fp.exists():
            raise FileNotFoundError(
                f"File {fp} does not exist. This files needed for the current flow.",
            )


def warn_for_reset_flags(files: list[Path], reset_flag_str: str = "-reset") -> None:
    for fp in files:
        raw_tcl_txt = fp.read_text()
        if reset_flag_str in raw_tcl_txt:
            print(
                f'Warning: {fp} contains the "-reset" flag {reset_flag_str}. '
                f'Since "dataset_hls.tcl" is run first it will '
                f"create the project and synthesis solution. "
                f'A "-reset" flag in {reset_flag_str} '
                f"will overwrite the already created project or solution.",
            )


class VitisHLSSynthFlow(ToolFlow):
    name = "VitisHLSSynthFlow"

    def __init__(
        self,
        vitis_hls_bin: str | None = None,
        log_output: bool = False,
        log_execution_time: bool = True,
        env_var_xilinx_hls: str | None = None,
        env_var_xilinx_vivado: str | None = None,
    ) -> None:
        if vitis_hls_bin is None:
            self.vitis_hls_bin = find_bin_path("vitis_hls")
        else:
            self.vitis_hls_bin = vitis_hls_bin

        self.log_output = log_output
        self.log_execution_time = log_execution_time
        self.env_var_xilinx_hls = env_var_xilinx_hls
        self.env_var_xilinx_vivado = env_var_xilinx_vivado

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        t_0 = time.perf_counter()

        design_dir = design.dir

        fp_hls_synth_tcl = design_dir / "dataset_hls.tcl"
        build_files = [fp_hls_synth_tcl]
        check_build_files_exist(build_files)

        if self.env_var_xilinx_hls:
            os.environ["XILINX_HLS"] = self.env_var_xilinx_hls
        if self.env_var_xilinx_vivado:
            os.environ["XILINX_VIVADO"] = self.env_var_xilinx_vivado

        if timeout is not None:
            return_result = call_tool(
                f"{self.vitis_hls_bin} dataset_hls.tcl",
                cwd=design_dir,
                log_output=self.log_output,
                timeout=timeout,
                raise_on_error=False,
            )
            if return_result == CallToolResult.TIMEOUT:
                (design_dir / f"timeout__{self.name}.txt").touch()
                print(f"[{design_dir}] Timeout of {timeout} seconds reached")

                t_1 = time.perf_counter()
                if self.log_execution_time:
                    log_execution_time_to_file(design_dir, self.name, t_0, t_1)

                return []
            if return_result == CallToolResult.ERROR:
                (design_dir / f"error__{self.name}.txt").touch()
                print(f"[{design_dir}] Error occurred during execution")

                t_1 = time.perf_counter()
                if self.log_execution_time:
                    log_execution_time_to_file(design_dir, self.name, t_0, t_1)

                return []
        else:
            return_result = call_tool(
                f"{self.vitis_hls_bin} dataset_hls.tcl",
                cwd=design_dir,
                log_output=self.log_output,
                raise_on_error=False,
            )
            if return_result == CallToolResult.ERROR:
                (design_dir / f"error__{self.name}.txt").touch()
                print(f"[{design_dir}] Error occurred during execution")

                t_1 = time.perf_counter()
                if self.log_execution_time:
                    log_execution_time_to_file(design_dir, self.name, t_0, t_1)

                return []

        csynth_report_fp = auto_find_synth_report(design_dir)

        hls_data = DesignHLSSynthData.parse_from_synth_report_file(csynth_report_fp)
        hls_data.to_json(design_dir / "data_hls.json")  # type: ignore

        design_data = VitisHLSDesign.parse_from_synth_report_file(csynth_report_fp)
        design_data.to_json(design_dir / "data_design.json")  # type: ignore

        t_1 = time.perf_counter()
        if self.log_execution_time:
            log_execution_time_to_file(design_dir, self.name, t_0, t_1)

        return [design]


class VitisHLSCosimSetupFlow(ToolFlow):
    name = "VitisHLSCosimSetupFlow"

    def __init__(
        self,
        vitis_hls_bin: str | None = None,
        log_output: bool = False,
    ) -> None:
        if vitis_hls_bin is None:
            self.vitis_hls_bin = find_bin_path("vitis_hls")
        else:
            self.vitis_hls_bin = vitis_hls_bin

        self.log_output = log_output

        self.patch_sim_fp = Path(__file__).parent / "patch_sim.sh"

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        # design_dir = design.dir

        # fp_hls_cosim_setup_tcl = design_dir / "dataset_hls_cosim_setup.tcl"
        # build_files = [fp_hls_cosim_setup_tcl]
        # check_build_files_exist(build_files)
        # warn_for_reset_flags(build_files)

        # call_tool(
        #     f"{self.vitis_hls_bin} dataset_hls_cosim_setup.tcl",
        #     cwd=design_dir,
        #     log_output=self.log_output,
        # )
        # cosim_dir = next(iter(design_dir.rglob("**/sim")))
        # solution_dir = cosim_dir.parent
        # call_tool(
        #     f"bash {self.patch_sim_fp}",
        #     cwd=solution_dir,
        #     log_output=self.log_output,
        # )

        # return [design]

        raise NotImplementedError("This flow is not yet implemented")


class VitisHLSImplFlow(ToolFlow):
    name = "VitisHLSImplFlow"

    def __init__(
        self,
        vitis_hls_bin: str | None = None,
        log_output: bool = False,
        env_var_xilinx_hls: str | None = None,
        env_var_xilinx_vivado: str | None = None,
    ) -> None:
        if vitis_hls_bin is None:
            self.vitis_hls_bin = find_bin_path("vitis_hls")
        else:
            self.vitis_hls_bin = vitis_hls_bin

        self.log_output = log_output
        self.env_var_xilinx_hls = env_var_xilinx_hls
        self.env_var_xilinx_vivado = env_var_xilinx_vivado

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        t_0 = time.perf_counter()

        design_dir = design.dir

        fp_hls_ip_export = design_dir / "dataset_hls_ip_export.tcl"
        build_files = [fp_hls_ip_export]
        check_build_files_exist(build_files)
        warn_for_reset_flags(build_files)

        if self.env_var_xilinx_hls:
            os.environ["XILINX_HLS"] = self.env_var_xilinx_hls
        if self.env_var_xilinx_vivado:
            os.environ["XILINX_VIVADO"] = self.env_var_xilinx_vivado

        if timeout is not None:
            return_result = call_tool(
                f"{self.vitis_hls_bin} dataset_hls_ip_export.tcl",
                cwd=design_dir,
                log_output=self.log_output,
                timeout=timeout,
                raise_on_error=False,
                shell=False,
            )
            if return_result == CallToolResult.TIMEOUT:
                (design_dir / f"timeout__{self.name}.txt").touch()
                print(f"[{design_dir}] Timeout of {timeout} seconds reached")
                t_1 = time.perf_counter()
                log_execution_time_to_file(design_dir, self.name, t_0, t_1)
                return []
            if return_result == CallToolResult.ERROR:
                (design_dir / f"error__{self.name}.txt").touch()
                print(f"[{design_dir}] Error occurred during execution")
                t_1 = time.perf_counter()
                log_execution_time_to_file(design_dir, self.name, t_0, t_1)
                return []
        else:
            return_result = call_tool(
                f"{self.vitis_hls_bin} dataset_hls_ip_export.tcl",
                cwd=design_dir,
                log_output=self.log_output,
                raise_on_error=False,
                shell=False,
            )
            if return_result == CallToolResult.ERROR:
                (design_dir / f"error__{self.name}.txt").touch()
                print(f"[{design_dir}] Error occurred during execution")
                t_1 = time.perf_counter()
                log_execution_time_to_file(design_dir, self.name, t_0, t_1)
                return []

        t_1 = time.perf_counter()
        log_execution_time_to_file(design_dir, self.name, t_0, t_1)

        return [design]


class VitisHLSImplReportFlow(ToolFlow):
    name = "VitisHLSImplReportFlow"

    def __init__(
        self,
        vitis_hls_bin: str | None = None,
        vivado_bin: str | None = None,
        log_output: bool = False,
        env_var_xilinx_hls: str | None = None,
        env_var_xilinx_vivado: str | None = None,
    ) -> None:
        if vitis_hls_bin is None:
            self.vitis_hls_bin = find_bin_path("vitis_hls")
        else:
            self.vitis_hls_bin = vitis_hls_bin

        if vivado_bin is None:
            self.vivado_bin = find_bin_path("vivado")
        else:
            self.vivado_bin = vivado_bin

        self.log_output = log_output
        self.env_var_xilinx_hls = env_var_xilinx_hls
        self.env_var_xilinx_vivado = env_var_xilinx_vivado

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        t_0 = time.perf_counter()

        if timeout is not None:
            timeout_not_supported(self.name)

        if self.env_var_xilinx_hls:
            os.environ["XILINX_HLS"] = self.env_var_xilinx_hls
        if self.env_var_xilinx_vivado:
            os.environ["XILINX_VIVADO"] = self.env_var_xilinx_vivado

        design_dir = design.dir

        vivado_xpr_file_results = list(design_dir.rglob("**/*.xpr"))
        if len(vivado_xpr_file_results) == 0:
            raise FileNotFoundError(f"No vivado project file found in {design_dir}")
        vivado_xpr_file = vivado_xpr_file_results[0]

        tcl_run_vivado_reporting_fp = design_dir / "run_vivado_reporting.tcl"

        s = ""
        s += f"open_project {vivado_xpr_file}\n"
        s += "open_run impl_1\n"
        s += "set_units -power mW\n"
        s += "report_power -hier all -file power.rpt\n"
        s += "report_utilization -hierarchical -file utilization.rpt\n"
        s += "report_timing -file slack.rpt -sort_by slack\n"
        s += "report_timing_summary -setup -hold -file timing.rpt\n"
        s += "close_design\n"
        s += "close_project\n"

        tcl_run_vivado_reporting_fp.write_text(s)

        return_result = call_tool(
            f"{self.vivado_bin} -mode batch -source run_vivado_reporting.tcl",
            cwd=design_dir,
        )
        if return_result == CallToolResult.ERROR:
            (design_dir / f"error__{self.name}.txt").touch()
            print(f"[{design_dir}] Error occurred during execution")

            t_1 = time.perf_counter()
            log_execution_time_to_file(design_dir, self.name, t_0, t_1)

            return []

        # check that the report files exist
        power_report_fp = design_dir / "power.rpt"
        utilization_report_fp = design_dir / "utilization.rpt"
        timing_report_fp = design_dir / "timing.rpt"

        check_files_exist([power_report_fp, utilization_report_fp, timing_report_fp])

        data = VitisHLSImplReportFlow.parse_all_reports(design_dir)
        data_fp = design_dir / "data_implementation.json"
        data_fp.write_text(json.dumps(data, indent=4))

        t_1 = time.perf_counter()
        log_execution_time_to_file(design_dir, self.name, t_0, t_1)

        return [design]

    @staticmethod
    def parse_all_reports(design_dir: Path) -> dict[str, dict]:
        power_report = VitisHLSImplReportFlow.parse_power_report(design_dir)
        utilization_report = VitisHLSImplReportFlow.parse_utilization_report(design_dir)
        timing_report = VitisHLSImplReportFlow.parse_timing_report(design_dir)

        data = {}
        for k, v in power_report.items():
            data[f"power__{k}"] = v
        for k, v in utilization_report.items():
            data[f"utilization__{k}"] = v
        for k, v in timing_report.items():
            data[f"timing__{k}"] = v

        return data

    @staticmethod
    def parse_power_report(design_dir: Path) -> dict:
        report_fp = design_dir / "power.rpt"
        check_files_exist([report_fp])

        data = {}

        report_raw = report_fp.read_text()

        report_raw = report_fp.read_text()
        lines = report_raw.splitlines()

        # | Total On-Chip Power (mW) | xxx.xx    |

        total_power_line = next(
            filter(lambda x: "Total On-Chip Power (mW)" in x, lines),
        )
        total_power = float(total_power_line.split("|")[2].strip())
        data["total_power"] = total_power

        # | Dynamic (mW)             | x.xxx        |
        # | Device Static (mW)       | xxxx.xxx     |

        dynamic_power_line = next(filter(lambda x: "Dynamic (mW)" in x, lines))
        dynamic_power = float(dynamic_power_line.split("|")[2].strip())
        data["dynamic_power"] = dynamic_power

        static_power_line = next(filter(lambda x: "Device Static (mW)" in x, lines))
        static_power = float(static_power_line.split("|")[2].strip())
        data["static_power"] = static_power

        return data

    @staticmethod
    def parse_utilization_report(design_dir: Path) -> dict:
        report_fp = design_dir / "utilization.rpt"
        check_files_exist([report_fp])

        report_raw = report_fp.read_text()
        lines = report_raw.splitlines()

        cols_to_search = [
            "Instance",
            "Module",
            "Total LUTs",
            "Logic LUTs",
            "LUTRAMs",
            "SRLs",
            "FFs",
            "RAMB36",
            "RAMB18",
            "URAM",
            "DSP Blocks",
        ]

        numeric_cols = [
            "Total LUTs",
            "Logic LUTs",
            "LUTRAMs",
            "SRLs",
            "FFs",
            "RAMB36",
            "RAMB18",
            "URAM",
            "DSP Blocks",
        ]

        line_idx = None
        for idx, line in enumerate(lines):
            if all(x in line for x in cols_to_search):
                line_idx = idx
                break

        if line_idx is None:
            raise ValueError("Could not find the line containing all the search terms")

        line_to_parse_data = lines[line_idx + 2]
        line_list = line_to_parse_data.split("|")
        line_list = [s.strip() for s in line_list]
        line_list = [x for x in line_list if x]
        line_list_numeric = line_list[2:]
        line_list_numeric = map(int, line_list_numeric)

        return dict(zip(numeric_cols, line_list_numeric, strict=False))

    @staticmethod
    def parse_timing_report(design_dir: Path) -> dict[str, float | int | str]:
        report_fp = design_dir / "timing.rpt"
        check_files_exist([report_fp])

        report_raw = report_fp.read_text()
        lines = report_raw.splitlines()

        timing_summary_cols = [
            "WNS(ns)",
            "TNS(ns)",
            "TNS Failing Endpoints",
            "TNS Total Endpoints",
            "WHS(ns)",
            "THS(ns)",
            "THS Failing Endpoints",
            "THS Total Endpoints",
            "WPWS(ns)",
            "TPWS(ns)",
            "TPWS Failing Endpoints",
            "TPWS Total Endpoints",
        ]

        cols_to_keep = [0, 1, 4, 5, 8, 9]
        cols_to_keep_names = [
            "WNS",
            "TNS",
            "WHS",
            "THS",
            "WPWS",
            "TPWS",
        ]

        timing_summary_cols_line_num = None
        for idx, line in enumerate(lines):
            if all(x in line for x in timing_summary_cols):
                timing_summary_cols_line_num = idx
                break

        if timing_summary_cols_line_num is None:
            raise ValueError("Could not find the line containing all the search terms")

        timing_summary_data_line = lines[timing_summary_cols_line_num + 2]
        timing_summary_data_line_list = timing_summary_data_line.split()
        timing_summary_data_line_list = [
            s.strip() for s in timing_summary_data_line_list
        ]
        timing_summary_data_line_list = [x for x in timing_summary_data_line_list if x]
        timing_summary_data_line_list = list(map(float, timing_summary_data_line_list))

        data_timing_summary = {}
        for idx, col_name in zip(cols_to_keep, cols_to_keep_names, strict=False):
            data_timing_summary[col_name] = timing_summary_data_line_list[idx]
        data_timing_summary = {k.lower(): v for k, v in data_timing_summary.items()}

        clock_cols = [
            "Clock",
            "Waveform(ns)",
            "Period(ns)",
            "Frequency(MHz)",
        ]

        clock_cols_line_num = None
        for idx, line in enumerate(lines):
            if all(x in line for x in clock_cols):
                clock_cols_line_num = idx
                break

        if clock_cols_line_num is None:
            raise ValueError("Could not find the line containing all the search terms")

        clock_data_line = lines[clock_cols_line_num + 2]
        clock_data_line_list = re.split(r"\s{2,}", clock_data_line)
        clock_data_line_list = [s.strip() for s in clock_data_line_list]
        clock_data_line_list = [x for x in clock_data_line_list if x]
        clock_data_line_list[2] = float(clock_data_line_list[2])
        clock_data_line_list[3] = float(clock_data_line_list[3])

        data_clock = {
            "clock_period": clock_data_line_list[2],
            "clock_frequency": clock_data_line_list[3],
        }

        return {
            **data_timing_summary,
            **data_clock,
        }
