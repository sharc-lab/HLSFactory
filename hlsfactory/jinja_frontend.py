import hashlib
import logging
import time
from pathlib import Path
from typing import Any

from jinja2 import Template

from hlsfactory.framework import Design, Frontend
from hlsfactory.utils import log_execution_time_to_file


class JinjaFrontend(Frontend):
    name = "JinjaFrontend"

    def __init__(
        self,
        work_dir: Path,
        jinja_configs: list[dict[str, Any]],
        log_execution_time: bool = True,
    ) -> None:
        self.work_dir = work_dir
        self.log_execution_time = log_execution_time

        self.jinja_configs = jinja_configs

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        t_0 = time.perf_counter()

        new_designs = []

        for jinja_config in self.jinja_configs:
            config_hash = hashlib.md5(  # noqa: S324
                string=str(jinja_config).encode(),
            ).hexdigest()

            new_design = design.copy_and_rename_to_new_parent_dir(
                f"{design.name}__jinja_{config_hash}",
                design.dir.parent,
            )
            new_designs.append(new_design)

            # in the new desing find all .jinja files recursively
            # and replace the content with the jinja_config
            jinja_files = sorted(new_design.dir.rglob("*.jinja"))
            for jinja_file in jinja_files:
                t: Template = Template(
                    jinja_file.read_text(),
                )
                # write the rendered template to the file without the .jinja extension
                new_file = jinja_file.with_suffix("")
                new_file.write_text(
                    t.render(
                        **jinja_config,
                    ),
                )
                # remove the .jinja file
                jinja_file.unlink()

        t_1 = time.perf_counter()

        log_execution_time_to_file(new_design.dir, self.name, t_0, t_1)
        log_execution_time_to_file(design.dir, self.name, t_0, t_1)

        return new_designs
