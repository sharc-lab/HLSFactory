# create a fornt end that uses the C++ preprocesser where a user passes in a list of stes fo define statenebts and that i used to paramaterse the diisgn using a C++ preprocessor

import hashlib
import time
import subprocess
import tempfile
import shutil
from pathlib import Path
from typing import Any

from hlsfactory.framework import Design, Frontend, EXTENSIONS_CPP
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
            # create a stable hash for this configuration
            config_hash = hashlib.md5(str(define_statement).encode()).hexdigest()

            new_design = design.copy_and_rename_to_new_parent_dir(
                f"{design.name}__cpp_preprocessor_{config_hash}",
                design.dir.parent,
            )
            new_designs.append(new_design)

            # Process the design files with the C++ preprocessor
            # build -D arguments from the define statement (dict)
            defines_args: list[str] = []
            for key, val in define_statement.items():
                if val is None:
                    defines_args.append(f"-D{key}")
                elif isinstance(val, bool):
                    if val:
                        defines_args.append(f"-D{key}")
                else:
                    # convert non-bool values to strings
                    defines_args.append(f"-D{key}={val}")

            # find C/C++ source files and run the preprocessor in-place
            for src in sorted(new_design.dir.rglob("*")):
                if not src.is_file():
                    continue
                if src.suffix.lower() not in EXTENSIONS_CPP:
                    continue

                # run g++ preprocessor and replace the original file with preprocessed output
                with tempfile.NamedTemporaryFile(delete=False) as tmpf:
                    tmp_path = Path(tmpf.name)
                cmd = [
                    "g++",
                    "-E",  # preprocess only
                    "-P",  # inhibit linemarkers (optional)
                    "-x",
                    "c++",
                    str(src),
                    "-o",
                    str(tmp_path),
                ]
                # insert defines right after flags (safe)
                cmd[1:1] = defines_args

                try:
                    subprocess.run(cmd, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                    # preserve file permissions
                    shutil.copymode(src, tmp_path)
                    shutil.move(str(tmp_path), str(src))
                except subprocess.CalledProcessError as e:
                    # cleanup and raise a helpful error
                    if tmp_path.exists():
                        tmp_path.unlink()
                    raise RuntimeError(
                        f"Preprocessing failed for {src} with defines={define_statement}: "
                        f"{e.stderr.decode(errors='ignore')}"
                    ) from e

        t_1 = time.perf_counter()

        log_execution_time_to_file(new_design.dir, self.name, t_0, t_1)
        log_execution_time_to_file(design.dir, self.name, t_0, t_1)

        return new_designs
