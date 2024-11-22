import time
from hlsfactory.flow_vitis import check_build_files_exist
from hlsfactory.framework import Design, ToolFlow
from hlsfactory.utils import (
    CallToolResult,
    call_tool,
    find_bin_path,
    log_execution_time_to_file,
)


class BambuSynthFlow(ToolFlow):
    raise NotImplementedError("BambuSynthFlow is not implemented yet")

    name = "BambuSynthFlow"

    def __init__(
        self,
        bambu_bin: str | None = None,
        log_output: bool = False,
        log_execution_time: bool = True,
    ) -> None:
        if bambu_bin is None:
            self.bambu_bin = find_bin_path("bambu_bin")
        else:
            self.bambu_bin = bambu_bin

        self.log_output = log_output
        self.log_execution_time = log_execution_time

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        t_0 = time.perf_counter()

        design_dir = design.dir

        fp_source = (
            design_dir / "top.cpp"
        )  # TODO: temp palceholder, need a way to define source files externally

        fp_bambu_options = design_dir / "bambu.xml"
        build_files = [fp_bambu_options, fp_source]
        check_build_files_exist(build_files)

        cmd_str = f"{self.bambu_bin} -f {fp_bambu_options} -o {design_dir / 'top'} {fp_source}"  #  TODO: not correct

        if timeout is not None:
            return_result = call_tool(
                # f"{self.vitis_hls_bin} dataset_hls.tcl",
                cmd_str,
                cwd=design_dir,
                log_output=self.log_output,
                timeout=timeout,
                raise_on_error=True,
            )
            if return_result == CallToolResult.TIMEOUT:
                (design_dir / f"timeout__{self.name}.txt").touch()
                print(f"[{design_dir}] Timeout of {timeout} seconds reached")

                t_1 = time.perf_counter()
                if self.log_execution_time:
                    log_execution_time_to_file(design_dir, self.name, t_0, t_1)

                return []
        else:
            call_tool(
                cmd_str,
                cwd=design_dir,
                log_output=self.log_output,
                raise_on_error=True,
            )

        t_1 = time.perf_counter()
        if self.log_execution_time:
            log_execution_time_to_file(design_dir, self.name, t_0, t_1)

        return [design]
