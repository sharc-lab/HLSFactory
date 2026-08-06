from hlsfactory.design_config import FlowName
from hlsfactory.framework import Design, ToolFlow
from hlsfactory.utils import find_bin_path


class LightningSimFlow(ToolFlow):
    name = "LightningSimFlow"

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
        design_dir = design.dir
        config = design.require_config()
        solution_name = config.require_flow_setting(FlowName.LIGHTNING_SIM, "solution_dir_name")

        # use the lightningsim API to simualtion the design from the solution directory

        if r == CallToolResult.SUCCESS:
            return [design]

        return []