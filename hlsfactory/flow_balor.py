from __future__ import annotations

import os
import re
import shlex
import shutil
import subprocess
import time
from dataclasses import dataclass
from pathlib import Path

from hlsfactory.design_config import FlowName
from hlsfactory.framework import Design, ToolFlow
from hlsfactory.utils import (
    flow_already_completed,
    serialize_methods_for_dataclass,
    write_execution_data,
)

HLSFACTORY_BALOR_PATH_ENV_VAR = "HLSFACTORY_BALOR_PATH"
HLSFACTORY_BALOR_DOCKER_IMAGE_ENV_VAR = "HLSFACTORY_BALOR_DOCKER_IMAGE"

DEFAULT_BALOR_DOCKER_IMAGE = "emmetmurphy/balor:latest"
GRAPH_COMPILER_RELATIVE_PATH = Path("graph_compiler/run_graph_compiler.py")

GRAPH_FILENAME = "data_graph_balor.dot"
GRAPH_SUMMARY_FILENAME = "data_graph_balor.json"

VALID_MODES = frozenset({"base", "opt"})

# `node [...]`, `edge [...]` and `graph [...]` set defaults for everything that
# follows; they are not themselves nodes.
_DOT_DEFAULT_DECLARATIONS = frozenset({"node", "edge", "graph"})

_DOT_NODE_PATTERN = re.compile(
    r"^\s*(?P<id>\"[^\"]+\"|[\w.:]+)\s*\[(?P<attrs>.*)\]\s*;?\s*$",
)
_DOT_EDGE_PATTERN = re.compile(
    r"^\s*(?P<src>\"[^\"]+\"|[\w.:]+)\s*->\s*(?P<dst>\"[^\"]+\"|[\w.:]+)"
    r"\s*(?:\[(?P<attrs>.*)\])?\s*;?\s*$",
)
_DOT_ATTR_PATTERN = re.compile(
    r"(?P<key>[\w.]+)\s*=\s*(?P<value>\"(?:[^\"\\]|\\.)*\"|[^,\]\s]+)",
)


def extract_dot_graph(output: str) -> str:
    start = output.find("digraph")
    if start == -1:
        detail = output.strip().splitlines()
        message = detail[-1] if detail else "no output"
        raise ValueError(
            f"Balor graph compiler produced no DOT graph. Last output line: {message}",
        )
    return output[start:]


def flatten_dot_quoted_newlines(text: str) -> str:
    out: list[str] = []
    in_quotes = False
    escaped = False
    for char in text:
        if escaped:
            out.append(char)
            escaped = False
            continue
        if char == "\\":
            out.append(char)
            escaped = True
            continue
        if char == '"':
            in_quotes = not in_quotes
            out.append(char)
            continue
        if char == "\n" and in_quotes:
            out.append("\\n")
            continue
        out.append(char)
    return "".join(out)


def get_balor_install_dir(balor_install_dir: str | Path | None = None) -> Path | None:
    if balor_install_dir is not None:
        return Path(balor_install_dir).expanduser()

    env_value = os.environ.get(HLSFACTORY_BALOR_PATH_ENV_VAR)
    if env_value:
        return Path(env_value).expanduser()

    return None


def find_graph_compiler(balor_install_dir: Path) -> Path:
    candidates = [
        balor_install_dir / GRAPH_COMPILER_RELATIVE_PATH,
        balor_install_dir / GRAPH_COMPILER_RELATIVE_PATH.name,
    ]
    for candidate in candidates:
        if candidate.is_file():
            return candidate

    searched = ", ".join(str(candidate) for candidate in candidates)
    raise FileNotFoundError(
        f"Could not find `run_graph_compiler.py` under Balor installation "
        f"directory {balor_install_dir}. Searched: {searched}. Set "
        f"{HLSFACTORY_BALOR_PATH_ENV_VAR} or pass `balor_install_dir` to "
        "BalorGraphFlow.",
    )


def validate_mode(mode: str) -> str:
    if mode not in VALID_MODES:
        raise ValueError(
            f"Balor `mode` must be one of {sorted(VALID_MODES)}; got {mode!r}.",
        )
    return mode


def _parse_dot_attributes(attrs: str | None) -> dict[str, str]:
    if not attrs:
        return {}
    parsed: dict[str, str] = {}
    for match in _DOT_ATTR_PATTERN.finditer(attrs):
        value = match.group("value").strip()
        if value.startswith('"') and value.endswith('"') and len(value) >= 2:
            value = value[1:-1]
        parsed[match.group("key")] = value
    return parsed


@serialize_methods_for_dataclass
@dataclass
class BalorGraphData:
    format: str
    top: str
    mode: str
    graph_file: str
    num_nodes: int
    num_edges: int
    node_attribute_counts: dict[str, int]
    edge_attribute_counts: dict[str, int]

    @classmethod
    def parse_from_dot_file(
        cls,
        fp: Path,
        top: str,
        mode: str,
    ) -> "BalorGraphData":
        """Count nodes, edges, and attribute keys in a DOT graph."""
        node_ids: set[str] = set()
        num_edges = 0
        node_attribute_counts: dict[str, int] = {}
        edge_attribute_counts: dict[str, int] = {}

        text = flatten_dot_quoted_newlines(fp.read_text(errors="replace"))
        for raw_line in text.splitlines():
            line = raw_line.strip()
            if not line or line.startswith(("//", "#")):
                continue
            if line.startswith(("digraph", "graph", "subgraph", "}", "{")):
                continue

            edge_match = _DOT_EDGE_PATTERN.match(line)
            if edge_match is not None:
                num_edges += 1
                node_ids.add(edge_match.group("src").strip('"'))
                node_ids.add(edge_match.group("dst").strip('"'))
                for key in _parse_dot_attributes(edge_match.group("attrs")):
                    edge_attribute_counts[key] = edge_attribute_counts.get(key, 0) + 1
                continue

            node_match = _DOT_NODE_PATTERN.match(line)
            if node_match is not None:
                node_id = node_match.group("id").strip('"')
                if node_id in _DOT_DEFAULT_DECLARATIONS:
                    continue
                attrs = _parse_dot_attributes(node_match.group("attrs"))
                if not attrs:
                    continue
                node_ids.add(node_id)
                for key in attrs:
                    node_attribute_counts[key] = node_attribute_counts.get(key, 0) + 1

        return cls(
            format="balor",
            top=top,
            mode=mode,
            graph_file=GRAPH_FILENAME,
            num_nodes=len(node_ids),
            num_edges=num_edges,
            node_attribute_counts=node_attribute_counts,
            edge_attribute_counts=edge_attribute_counts,
        )


class BalorGraphFlow(ToolFlow):
    name = "BalorGraphFlow"

    def __init__(
        self,
        balor_install_dir: str | Path | None = None,
        docker_image: str | None = None,
        python_bin: str = "python",
        log_output: bool = False,
    ) -> None:
        self.balor_install_dir = get_balor_install_dir(balor_install_dir)
        self.python_bin = python_bin
        self.log_output = log_output

        if self.balor_install_dir is None:
            self.docker_image = docker_image or os.environ.get(
                HLSFACTORY_BALOR_DOCKER_IMAGE_ENV_VAR,
                DEFAULT_BALOR_DOCKER_IMAGE,
            )
            if shutil.which("docker") is None:
                raise RuntimeError(
                    "BalorGraphFlow needs either a local Balor checkout (set "
                    f"{HLSFACTORY_BALOR_PATH_ENV_VAR}) or Docker to run the "
                    f"`{self.docker_image}` image, but `docker` was not found on "
                    "PATH. Balor's graph compiler is built on ROSE and cannot be "
                    "installed with pip.",
                )
        else:
            self.docker_image = None
            find_graph_compiler(self.balor_install_dir)

    def build_command(
        self,
        source_path: Path,
        top: str,
        mode: str,
        output_dir: Path,
        generalize_types: bool,
        make_pdf: bool,
    ) -> str:
        """Build the graph-compiler command line for the selected backend."""
        if self.balor_install_dir is not None:
            compiler = find_graph_compiler(self.balor_install_dir)
            parts = [
                self.python_bin,
                str(compiler),
                "--mode",
                mode,
                "--src",
                str(source_path),
                "--top",
                top,
                "--outputFolder",
                str(output_dir),
            ]
            if generalize_types:
                parts.append("--generalize_types")
            if make_pdf:
                parts.append("--make_pdf")
            return " ".join(shlex.quote(part) for part in parts)

        container_src = f"/root/{source_path.name}"
        parts = [
            "docker",
            "run",
            "--rm",
            "-v",
            f"{source_path}:{container_src}",
            "-v",
            f"{output_dir}:/root/output/",
            "-w",
            "/root/balor/graph_compiler",
            str(self.docker_image),
            "python",
            "/root/balor/graph_compiler/run_graph_compiler.py",
            "--mode",
            mode,
            "--src",
            container_src,
            "--top",
            top,
            "--outputFolder",
            "/root/output/",
        ]
        if generalize_types:
            parts.append("--generalize_types")
        if make_pdf:
            parts.append("--make_pdf")
        return " ".join(shlex.quote(part) for part in parts)

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        design_dir = design.dir
        data_file = design_dir / GRAPH_SUMMARY_FILENAME

        if flow_already_completed(design_dir, self.name, success_marker_fp=data_file):
            print(f"[{design_dir}] Skipping {self.name}, already completed")
            return [design]

        config = design.require_config()
        flow_config = config.require_flow_config(FlowName.BALOR_GRAPH)
        top = flow_config.require_setting("top")
        source_file = flow_config.require_setting("source_file")
        mode = validate_mode(flow_config.get_setting("mode", "opt"))
        generalize_types = flow_config.get_setting("generalize_types", "true") == "true"
        make_pdf = flow_config.get_setting("make_pdf", "false") == "true"

        source_path = design_dir / source_file
        if not source_path.is_file():
            raise FileNotFoundError(
                f"Balor source file {source_path} does not exist. It is required "
                "for graph extraction.",
            )

        output_dir = design_dir / "balor_output"
        output_dir.mkdir(exist_ok=True)

        start_time = time.perf_counter()
        command = self.build_command(
            source_path,
            top,
            mode,
            output_dir,
            generalize_types,
            make_pdf,
        )

        try:
            completed = subprocess.run(  # noqa: S603
                shlex.split(command),
                cwd=design_dir,
                capture_output=True,
                timeout=timeout,
                check=False,
            )
        except subprocess.TimeoutExpired:
            message = f"Timeout of {timeout} seconds reached"
            print(f"[{design_dir}] {message}")
            self._log(design_dir, "timeout", start_time, message)
            return []

        stdout = completed.stdout.decode("utf-8", errors="replace")
        if self.log_output:
            print(stdout)

        # The graph compiler exits 0 even when it aborts with a C++ exception,
        # so the exit code alone cannot be trusted.
        try:
            graph_text = extract_dot_graph(stdout)
        except ValueError as error:
            message = str(error)
            print(f"[{design_dir}] {message}")
            self._log(design_dir, "error", start_time, message)
            return []

        graph_file = design_dir / GRAPH_FILENAME
        graph_file.write_text(graph_text)

        summary = BalorGraphData.parse_from_dot_file(graph_file, top, mode)
        summary.to_json(data_file)  # type: ignore[attr-defined]

        self._log(design_dir, "success", start_time, None)
        return [design]

    def _log(
        self,
        design_dir: Path,
        status: str,
        start_time: float,
        message: str | None,
    ) -> None:
        write_execution_data(
            design_dir,
            self.name,
            status,
            start_time,
            time.perf_counter(),
            return_code=0 if status == "success" else 1,
            error_message=message,
        )


__all__ = [
    "DEFAULT_BALOR_DOCKER_IMAGE",
    "HLSFACTORY_BALOR_DOCKER_IMAGE_ENV_VAR",
    "HLSFACTORY_BALOR_PATH_ENV_VAR",
    "BalorGraphData",
    "BalorGraphFlow",
    "extract_dot_graph",
    "find_graph_compiler",
    "flatten_dot_quoted_newlines",
    "get_balor_install_dir",
    "validate_mode",
]
