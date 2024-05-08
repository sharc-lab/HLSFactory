import json
import re
from dataclasses import dataclass
from pathlib import Path

from hlsfactory.framework import Design, ToolFlow
from hlsfactory.utils import (
    call_tool,
    find_bin_path,
    serialize_methods_for_dataclass,
)


@serialize_methods_for_dataclass
@dataclass
class DesignHLSSynthData:
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
    def parse_from_synth_report_file(cls, data_file: Path) -> "DesignHLSSynthData":
        fd = open(data_file, encoding="utf-8")
        data = json.load(fd)
        resource_data = {}
        resource_data["used_abs"] = {}
        resource_data["used_abs"]["ALUTs"] = int(
            data["estimatedResources"]["children"][1]["data"][0],
        )
        resource_data["used_abs"]["FFs"] = int(
            data["estimatedResources"]["children"][1]["data"][1],
        )
        resource_data["used_abs"]["RAMs"] = int(
            data["estimatedResources"]["children"][1]["data"][2],
        )
        resource_data["used_abs"]["DSPs"] = int(
            data["estimatedResources"]["children"][1]["data"][3],
        )
        resource_data["used_abs"]["MALBs"] = int(
            data["estimatedResources"]["children"][1]["data"][4],
        )

        resource_data["avail_abs"] = {}
        resource_data["avail_abs"]["ALUTs"] = int(
            data["estimatedResources"]["children"][2]["data"][0],
        )
        resource_data["avail_abs"]["FFs"] = int(
            data["estimatedResources"]["children"][2]["data"][1],
        )
        resource_data["avail_abs"]["RAMs"] = int(
            data["estimatedResources"]["children"][2]["data"][2],
        )
        resource_data["avail_abs"]["DSPs"] = int(
            data["estimatedResources"]["children"][2]["data"][3],
        )
        resource_data["avail_abs"]["MALBs"] = int(
            data["estimatedResources"]["children"][2]["data"][4],
        )

        return cls(
            resources_ALUTs_used=resource_data["used_abs"]["ALUTs"],
            resources_FFs_used=resource_data["used_abs"]["FFs"],
            resources_RAMs_used=resource_data["used_abs"]["RAMs"],
            resources_DSPs_used=resource_data["used_abs"]["DSPs"],
            resources_MLABs_used=resource_data["used_abs"]["MALBs"],
            resources_ALUTs_avail=resource_data["avail_abs"]["ALUTs"],
            resources_FFs_avail=resource_data["avail_abs"]["FFs"],
            resources_RAMs_avail=resource_data["avail_abs"]["RAMs"],
            resources_DSPs_avail=resource_data["avail_abs"]["DSPs"],
            resources_MLABs_avail=resource_data["avail_abs"]["MALBs"],
        )
        fd.close()


@serialize_methods_for_dataclass
@dataclass
class IntelHLSDesign:
    name: str
    target_clock_period: None | str
    family: str
    product: str
    quartus: str
    time: str
    version: str

    @classmethod
    def parse_from_synth_report_file(
        cls,
        info_json: Path,
        summary_json: Path,
    ) -> "IntelHLSDesign":
        fp_summary_json = open(summary_json, encoding="utf-8")
        data = json.load(fp_summary_json)
        name = data["estimatedResources"]["children"][0]["name"]
        fp_summary_json.close()

        fp_info_json = open(info_json, encoding="utf-8")
        data = json.load(fp_info_json)

        command = data["compileInfo"]["nodes"][0]["command"]
        match = re.search(r"--clock\s+(\d+ns)", command)
        target_clock_period = None
        if match:
            target_clock_period = match.group(1)

        family = data["compileInfo"]["nodes"][0]["family"]
        product = data["compileInfo"]["nodes"][0]["product"]
        quartus = data["compileInfo"]["nodes"][0]["quartus"]
        time = data["compileInfo"]["nodes"][0]["time"]
        version = data["compileInfo"]["nodes"][0]["version"]

        fp_info_json.close()

        return cls(
            name=name,
            target_clock_period=target_clock_period,
            family=family,
            product=product,
            quartus=quartus,
            time=time,
            version=version,
        )


class IntelHLSSynthFlow(ToolFlow):
    name = "IntelHLSSynthFlow"

    def __init__(
        self,
        ipp_bin: str | None = None,
        # arch: str = "Cyclone 10 GX",
        arch: str = "1ST110EN1F43E1VG",
        clock: str = "10ns",
        verbose: bool = True,
    ):
        if ipp_bin is None:
            self.ipp_bin = find_bin_path("i++")
        else:
            self.ipp_bin = ipp_bin

        self.arch = arch
        self.clock = clock
        self.verbose = verbose

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        # ....../atax/atax_1/
        design_dir = design.dir

        # ....../atax/atax_1/atax.c
        design_name = design.name

        # atax_1
        bench_name = design.dir.name

        # i++ -march="Cyclone 10 GX" -ghdl --clock 10ns --component $benchmark $design.c --simulator "none" -v -o $design
        # i++ -march=1ST110EN1F43E1VG -ghdl $PATH/atax_1.c --simulator "none" -v -o $PATH/atax_1.prj
        cmd = ""
        cmd += f"{self.ipp_bin} "
        cmd += f"-march={self.arch} "
        cmd += f"-ghdl {design_name} "
        cmd += f"--clock {self.clock} "
        # cmd += f"--component {design_name} "
        # cmd += f"{design_dir / f'{bench_name}.c'} "
        cmd += '--simulator "none" '
        cmd += "-v "
        cmd += f"-o {design_dir / f'{bench_name}.prj'}"

        call_tool(cmd, design_dir)

        summary_report = (
            design_dir
            / f"{bench_name}.prj"
            / "reports"
            / "lib"
            / "json"
            / "summary.json"
        )
        hls_data = DesignHLSSynthData.parse_from_synth_report_file(summary_report)
        hls_data.to_json(design_dir / "data_hls.json")  # type: ignore

        tool_report = (
            design_dir / f"{bench_name}.prj" / "reports" / "lib" / "json" / "info.json"
        )
        design_data = IntelHLSDesign.parse_from_synth_report_file(
            tool_report,
            summary_report,
        )
        design_data.to_json(design_dir / "data_design.json")  # type: ignore

        return [design]


@serialize_methods_for_dataclass
@dataclass
class IntelImpDesignResource:
    name: str
    clock_unit: str | None
    clock: float | None
    clock1x: float | None
    alm: float
    reg: int
    dsp: int
    ram: int
    mlab: int

    @classmethod
    def parse_from_impl_report_file(
        cls,
        quartus_json: Path,
    ) -> "IntelImpDesignResource":
        fp = open(quartus_json, encoding="utf-8")
        data = json.load(fp)

        try:
            clock_unit = str(data["quartusFitClockSummary"]["nodes"][0]["name"])
        except (ValueError, KeyError):
            clock_unit = None
        try:
            clock = float(data["quartusFitClockSummary"]["nodes"][0]["clock"])
        except (ValueError, KeyError):
            clock = None
        try:
            clock1x = float(data["quartusFitClockSummary"]["nodes"][0]["clock1x"])
        except (ValueError, KeyError):
            clock1x = None
        try:
            name = str(data["quartusFitResourceUsageSummary"]["nodes"][0]["name"])
        except (ValueError, KeyError):
            name = None
        try:
            alm = str(data["quartusFitResourceUsageSummary"]["nodes"][0]["alm"])
        except (ValueError, KeyError):
            alm = None
        try:
            reg = str(data["quartusFitResourceUsageSummary"]["nodes"][0]["reg"])
        except (ValueError, KeyError):
            reg = None
        try:
            dsp = str(data["quartusFitResourceUsageSummary"]["nodes"][0]["dsp"])
        except (ValueError, KeyError):
            dsp = None
        try:
            ram = str(data["quartusFitResourceUsageSummary"]["nodes"][0]["ram"])
        except (ValueError, KeyError):
            ram = None
        try:
            mlab = str(data["quartusFitResourceUsageSummary"]["nodes"][0]["mlab"])
        except (ValueError, KeyError):
            mlab = None

        fp.close()

        return cls(
            name=name,
            clock_unit=clock_unit,
            clock=clock,
            clock1x=clock1x,
            alm=alm,
            reg=reg,
            dsp=dsp,
            ram=ram,
            mlab=mlab,
        )


class IntelQuartusImplFlow(ToolFlow):
    name = "IntelQuartusImplFlow"

    def __init__(self, quartus_bin: str | None = None, verbose: bool = True):
        if quartus_bin is None:
            self.quartus_bin = find_bin_path("quartus_sh")
        else:
            self.quartus_bin = quartus_bin

        self.verbose = verbose

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        design_dir = design.dir
        # design_name = design.name
        bench_name = design.dir.name

        cwd = design_dir / f"{bench_name}.prj" / "quartus"
        cmd = f"{self.quartus_bin} --flow compile quartus_compile"
        call_tool(cmd, cwd)

        prj_dir = design_dir / f"{bench_name}.prj"
        power_to_csv = prj_dir / "power_to_csv.tcl"
        quartus_power_set = prj_dir / "quartus_power_set"
        output_pwr = prj_dir / f"{bench_name}_power.csv"
        qsf = cwd / "quartus_compile.qsf"

        resource_report = prj_dir / "reports" / "lib" / "json" / "quartus.json"
        resource_data = IntelImpDesignResource.parse_from_impl_report_file(
            resource_report,
        )
        resource_data.to_json(design_dir / "data_implementation.json")  # type: ignore

        # if power configuration files exsit, run power analysis
        if power_to_csv.is_file():
            # with open(quartus_power_set) as f:
            with quartus_power_set.open() as f:
                lines = f.readlines()
            # with open(qsf, "a+") as f:
            with qsf.open("a+") as f:
                f.write("\n".join(lines))

            cmd = "quartus_pow quartus_compile"
            call_tool(cmd, cwd)

            # quartus_sh -t $base_dir/power_to_csv.tcl -project "quartus_compile"
            # -panel "Power Analyzer||Power Analyzer Summary" -file ../../../data/"$benchmark"_"$i".csv
            cmd = f'{self.quartus_bin} -t {power_to_csv} -project "quartus_compile" \
                -panel "Power Analyzer||Power Analyzer Summary" -file {output_pwr}'
            call_tool(cmd, cwd)

        return [design]
