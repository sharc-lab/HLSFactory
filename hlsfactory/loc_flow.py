import json
import time
from pprint import pp

from hlsfactory.framework import Design, Flow
from hlsfactory.utils import log_execution_time_to_file


class LOCCounterFlow(Flow):
    name = "LOCCounterFlow"

    def __init__(
        self,
        log_execution_time: bool = True,
    ) -> None:
        self.log_execution_time = log_execution_time

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        t_0 = time.perf_counter()

        design_dir = design.dir

        loc_count_map = {}
        source_files = design.cpp_source_files
        for file in source_files:
            txt = file.read_text()
            lines = txt.splitlines()
            non_empty_lines = [line for line in lines if line.strip() != ""]
            loc_count_map[file.name] = len(non_empty_lines)

        loc_count_total = sum(loc_count_map.values())

        data = {
            "loc_count_total": loc_count_total,
            "loc_count_map": loc_count_map,
        }

        json_path = design_dir / "data_loc_count.json"
        json_path.write_text(json.dumps(data, indent=4))

        t_1 = time.perf_counter()
        if self.log_execution_time:
            log_execution_time_to_file(design_dir, self.name, t_0, t_1)

        return [design]
