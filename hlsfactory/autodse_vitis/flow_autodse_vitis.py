import re
from pathlib import Path

from hlsfactory.autodse_vitis.src.design_space.container import TemplateContainer
from hlsfactory.autodse_vitis.src.design_space.explorer import DSEExplorer
from hlsfactory.autodse_vitis.src.design_space.logger import TeeLogger
from hlsfactory.autodse_vitis.src.objective.objectives import (
    ResourceConstrainedLatencyObjective,
)
from hlsfactory.framework import Design, Flow


def _extract_top_module_from_tcl(template_dir: Path) -> str | None:  # noqa: C901, PLR0911
    """Extract top module name from project.tcl or infer from directory/files."""
    tcl_file = template_dir / "project.tcl"
    if tcl_file.exists():
        try:
            with tcl_file.open() as f:
                for raw_line in f:
                    line = raw_line.strip()
                    if line.startswith("set_top"):
                        match = re.search(r'set_top\s+["\']([^"\']+)["\']', line)
                        if match:
                            return match.group(1)
                        match = re.search(r"set_top\s+(\S+)", line)
                        if match:
                            return match.group(1)
        except OSError:
            pass

    template_name = template_dir.name
    if "fft" in template_name.lower():
        return "FFT_TOP"
    if "3mm" in template_name.lower():
        return "kernel_3mm"
    if template_name.startswith("kernel_"):
        return template_name

    for cpp_file in template_dir.glob("*.cpp"):
        if "testbench" not in cpp_file.name.lower():
            base_name = cpp_file.stem
            if base_name.lower() in ("fft", "fft_top"):
                return "FFT_TOP"
            return f"kernel_{base_name}"

    return None


class AutoDSEFlow(Flow):
    name: str = "AutoDSEFlow"

    def __init__(
        self,
        work_dir: Path,
        *,
        output_folder_name: str = "autodse",
        max_iterations: int = 3,
        timeout_min: int = 10,
    ) -> None:
        super().__init__(work_dir)
        self.output_folder_name = output_folder_name
        self.max_iterations = max_iterations
        self.timeout_min = timeout_min

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        template_dir = design.dir
        output_dir = template_dir / self.output_folder_name
        output_dir.mkdir(parents=True, exist_ok=True)

        timeout_min = (
            max(1, int(timeout / 60)) if timeout is not None else self.timeout_min
        )

        top_module = _extract_top_module_from_tcl(template_dir)
        if top_module is None:
            top_module = "unknown_top"

        container = TemplateContainer(str(template_dir))
        objective = ResourceConstrainedLatencyObjective(
            max_dsp=5472,
            max_bram=3456,
            max_lut=945792,
            max_ff=1891584,
            max_uram=768,
            top_module=top_module,
        )
        explorer = DSEExplorer(container=container, objective_fn=objective)

        logger = TeeLogger(output_dir / "explore.log", console_output=True)
        try:
            explorer.set_logger(logger)
            explorer.run_exploration_LevelHeap_DFS(
                max_iterations=self.max_iterations,
                output_dir=str(output_dir),
                timeout_min=timeout_min,
            )
        finally:
            logger.close()

        return [design]
