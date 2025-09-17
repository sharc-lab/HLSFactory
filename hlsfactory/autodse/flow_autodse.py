from pathlib import Path

from hlsfactory.framework import Design, Flow


class AutoDSEFlow(Flow):
    name: str = "AutoDSEFlow"

    def __init__(self, work_dir: Path) -> None:
        self.work_dir = work_dir

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]: ...
