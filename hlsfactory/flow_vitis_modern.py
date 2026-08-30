import os
import time

from hlsfactory.design_config import FlowName
from hlsfactory.flow_vitis import (
    DesignHLSSynthData,
    VitisHLSDesign,
    auto_find_synth_report,
    check_build_files_exist,
)
from hlsfactory.framework import Design, ToolFlow
from hlsfactory.utils import (
    CallToolResult,
    call_tool,
    find_bin_path,
    flow_already_completed,
    log_execution_time_to_file,
)

# Both the synth flow and the csim flow below operate on the same HLS
# component directory, since the modern flow's csim/cosim/impl/package steps
# all run against a component that was already created (typically by a prior
# C-synthesis run). Using a fixed, well-known name lets the csim/cosim flows
# find the component created by VitisHLSModernSynthFlow without needing extra
# design-config plumbing.
HLS_COMPONENT_DIR_NAME = "hls_component"

# Starting with the Vitis Unified IDE (any release after 2024.2), Vitis HLS is
# no longer a standalone tool with its own `vitis_hls` TCL entry point.
# Instead HLS becomes an "HLS component" driven by the common `v++`/`vitis-run`
# command line tools and a plain INI-style HLS config file (`.cfg`), rather
# than a `.tcl` script. See:
#   https://docs.amd.com/r/en-US/ug1399-vitis-hls/Vitis-HLS-Migration-Guide
#   https://docs.amd.com/r/en-US/ug1399-vitis-hls/Vitis-HLS-Flow-Steps
#
# C-synthesis (and packaging of the resulting RTL as a Vivado IP/Vitis kernel)
# is run via:
#   v++ -c --mode hls --config <hls_config.cfg> --work_dir <work_dir>
#
# This produces a component directory (marked by a `vitis-comp.json` file)
# with the synthesis report at `<work_dir>/hls/syn/report/csynth.xml`, which
# uses the same XML schema as the classic Vitis HLS `csynth.xml` report, so
# the existing parsers in `hlsfactory.flow_vitis` are reused as-is.


class VitisHLSModernSynthFlow(ToolFlow):
    name = "VitisHLSModernSynthFlow"

    def __init__(
        self,
        vpp_bin: str | None = None,
        log_output: bool = False,
        log_execution_time: bool = True,
        env_var_xilinx_hls: str | None = None,
        env_var_xilinx_vitis: str | None = None,
        env_var_xilinx_vivado: str | None = None,
    ) -> None:
        if vpp_bin is None:
            self.vpp_bin = find_bin_path("v++")
        else:
            self.vpp_bin = vpp_bin

        self.log_output = log_output
        self.log_execution_time = log_execution_time
        self.env_var_xilinx_hls = env_var_xilinx_hls
        self.env_var_xilinx_vitis = env_var_xilinx_vitis
        self.env_var_xilinx_vivado = env_var_xilinx_vivado

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        design_dir = design.dir

        if flow_already_completed(
            design_dir,
            self.name,
            success_marker_fp=design_dir / "data_hls.json",
        ):
            print(f"[{design_dir}] Skipping {self.name}, already completed")
            return [design]

        t_0 = time.perf_counter()

        # Get the HLS config file path from the design config
        config = design.require_config()
        hls_config_name = config.require_flow_setting(
            FlowName.VITIS_HLS_MODERN_SYNTH, "hls_config"
        )
        fp_hls_config = design_dir / hls_config_name
        check_build_files_exist([fp_hls_config])

        if self.env_var_xilinx_hls:
            os.environ["XILINX_HLS"] = self.env_var_xilinx_hls
        if self.env_var_xilinx_vitis:
            os.environ["XILINX_VITIS"] = self.env_var_xilinx_vitis
        if self.env_var_xilinx_vivado:
            os.environ["XILINX_VIVADO"] = self.env_var_xilinx_vivado

        cmd = (
            f"{self.vpp_bin} -c --mode hls "
            f"--config {hls_config_name} --work_dir {HLS_COMPONENT_DIR_NAME}"
        )

        return_result = call_tool(
            cmd,
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

        work_dir = design_dir / HLS_COMPONENT_DIR_NAME
        csynth_report_fp = auto_find_synth_report(work_dir)

        hls_data = DesignHLSSynthData.parse_from_synth_report_file(csynth_report_fp)
        hls_data.to_json(design_dir / "data_hls.json")  # type: ignore

        design_data = VitisHLSDesign.parse_from_synth_report_file(csynth_report_fp)
        design_data.to_json(design_dir / "data_design.json")  # type: ignore

        t_1 = time.perf_counter()
        if self.log_execution_time:
            log_execution_time_to_file(design_dir, self.name, t_0, t_1)

        return [design]


class VitisHLSModernCsimFlow(ToolFlow):
    name = "VitisHLSModernCsimFlow"

    def __init__(
        self,
        vitis_run_bin: str | None = None,
        log_output: bool = False,
        log_execution_time: bool = True,
        env_var_xilinx_hls: str | None = None,
        env_var_xilinx_vitis: str | None = None,
        env_var_xilinx_vivado: str | None = None,
    ) -> None:
        if vitis_run_bin is None:
            self.vitis_run_bin = find_bin_path("vitis-run")
        else:
            self.vitis_run_bin = vitis_run_bin

        self.log_output = log_output
        self.log_execution_time = log_execution_time
        self.env_var_xilinx_hls = env_var_xilinx_hls
        self.env_var_xilinx_vitis = env_var_xilinx_vitis
        self.env_var_xilinx_vivado = env_var_xilinx_vivado

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        design_dir = design.dir

        t_0 = time.perf_counter()

        # Get the HLS config file path from the design config. The same
        # config file used for VitisHLSModernSynthFlow is reused here; it
        # must also declare a `tb.file` for csim to have a test bench to run.
        config = design.require_config()
        hls_config_name = config.require_flow_setting(
            FlowName.VITIS_HLS_MODERN_CSIM, "hls_config"
        )
        fp_hls_config = design_dir / hls_config_name
        check_build_files_exist([fp_hls_config])

        if self.env_var_xilinx_hls:
            os.environ["XILINX_HLS"] = self.env_var_xilinx_hls
        if self.env_var_xilinx_vitis:
            os.environ["XILINX_VITIS"] = self.env_var_xilinx_vitis
        if self.env_var_xilinx_vivado:
            os.environ["XILINX_VIVADO"] = self.env_var_xilinx_vivado

        cmd = (
            f"{self.vitis_run_bin} --mode hls --config {hls_config_name} "
            f"--csim --work_dir {HLS_COMPONENT_DIR_NAME}"
        )

        return_result = call_tool(
            cmd,
            cwd=design_dir,
            log_output=self.log_output,
            timeout=timeout,
            raise_on_error=False,
        )

        t_1 = time.perf_counter()
        if self.log_execution_time:
            log_execution_time_to_file(design_dir, self.name, t_0, t_1)

        if return_result == CallToolResult.TIMEOUT:
            (design_dir / f"timeout__{self.name}.txt").touch()
            print(f"[{design_dir}] Timeout of {timeout} seconds reached")
            return []
        if return_result == CallToolResult.ERROR:
            (design_dir / f"error__{self.name}.txt").touch()
            print(f"[{design_dir}] Error occurred during execution")
            return []

        return [design]
