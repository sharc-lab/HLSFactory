# create a fornt end that uses the C++ preprocesser where a user passes in a list of stes fo define statenebts and that i used to paramaterse the diisgn using a C++ preprocessor

import hashlib
import time
from pathlib import Path
from typing import Any

from hlsfactory.framework import Design, Frontend
from hlsfactory.utils import log_execution_time_to_file


class CPPPreprocessorFrontend(Frontend):
    name = "CPPPreprocessorFrontend"

    def __init__(
        self,
        work_dir: Path,
        define_statements: list[dict[str, Any]],
        log_execution_time: bool = True,
    ) -> None:
        self.work_dir = work_dir
        self.define_statements = define_statements
        self.log_execution_time = log_execution_time

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        t_0 = time.perf_counter()

        new_designs = []

        for define_statement in self.define_statements:
            config_hash = hashlib.md5(  # noqa: S324
                string=str(define_statement).encode(),
            ).hexdigest()

            new_design = design.copy_and_rename_to_new_parent_dir(
                f"{design.name}__cpp_preprocessor_{config_hash}",
                design.dir.parent,
            )
            new_designs.append(new_design)

            # Process the design files with the C++ preprocessor
            sorted(new_design.dir.rglob("*.*"))

        t_1 = time.perf_counter()

        log_execution_time_to_file(new_design.dir, self.name, t_0, t_1)
        log_execution_time_to_file(design.dir, self.name, t_0, t_1)

        return new_designs
