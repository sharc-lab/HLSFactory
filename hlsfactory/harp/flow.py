from __future__ import annotations

import json
import shutil
import time
from pathlib import Path

import networkx as nx

from hlsfactory.design_config import FlowName
from hlsfactory.framework import Design, Flow
from hlsfactory.harp._programl_compat import load_programl
from hlsfactory.harp.harp_graph import (
    emit_llvm_ir,
    get_harp_clang_bin,
    graph_generator,
)
from hlsfactory.utils import write_execution_data

GRAPH_FILENAME = "data_graph_harp.gexf"
GRAPH_SUMMARY_FILENAME = "data_graph_harp.json"


class HARPGraphFlow(Flow):


    name = "HARPGraphFlow"

    def __init__(
        self,
        clang_bin: str | None = None,
        log_output: bool = False,
    ) -> None:
        load_programl()
        if shutil.which(get_harp_clang_bin(clang_bin)) is None:
            raise RuntimeError(
                f"Could not find `{get_harp_clang_bin(clang_bin)}` on PATH. HARP "
                "graph extraction needs a clang no newer than 14 because the "
                "ProGraML binary bundled with `programl` is an LLVM 10 parser. "
                "Install it (e.g. `apt install clang-14`) or set "
                "HLSFACTORY_HARP_CLANG.",
            )

        self.clang_bin = clang_bin
        self.log_output = log_output

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        design_dir = design.dir
        t_0 = time.perf_counter()

        config = design.require_config()
        top = config.require_flow_setting(FlowName.HARP_GRAPH, "top")
        source_file = config.require_flow_setting(FlowName.HARP_GRAPH, "source_file")

        source_path = design_dir / source_file
        if not source_path.is_file():
            raise FileNotFoundError(
                f"HARP source file {source_path} does not exist. It is required "
                "for graph extraction.",
            )

        try:
            self._extract(design_dir, top, source_path)
        except Exception as error:  # noqa: BLE001
            message = f"HARP graph extraction failed: {error}"
            print(f"[{design_dir}] {message}")
            write_execution_data(
                design_dir,
                self.name,
                "error",
                t_0,
                time.perf_counter(),
                return_code=1,
                error_message=message,
            )
            return []

        write_execution_data(
            design_dir,
            self.name,
            "success",
            t_0,
            time.perf_counter(),
        )
        return [design]

    def _extract(self, design_dir: Path, top: str, source_path: Path) -> None:
        stem = source_path.stem

        emit_llvm_ir(stem, str(design_dir), clang_bin=self.clang_bin)

        graph_generator(
            stem,
            str(design_dir),
            benchmark=None,
            generate_programl=False,
            output_dir=str(design_dir),
        )

        produced = design_dir / f"{stem}_processed_result.gexf"
        if not produced.is_file():
            raise RuntimeError(
                f"HARP did not produce the expected graph {produced}.",
            )
        produced.replace(design_dir / GRAPH_FILENAME)

        graph = nx.read_gexf(design_dir / GRAPH_FILENAME)
        self._write_summary(design_dir, graph, top)

    @staticmethod
    def _write_summary(design_dir: Path, graph, top: str) -> None:
        node_type_counts: dict[str, int] = {}
        for _, data in graph.nodes(data=True):
            key = str(data.get("type", "unknown"))
            node_type_counts[key] = node_type_counts.get(key, 0) + 1

        edge_flow_counts: dict[str, int] = {}
        for _, _, data in graph.edges(data=True):
            key = str(data.get("flow", "unknown"))
            edge_flow_counts[key] = edge_flow_counts.get(key, 0) + 1

        summary = {
            "format": "harp",
            "top": top,
            "graph_file": GRAPH_FILENAME,
            "num_nodes": graph.number_of_nodes(),
            "num_edges": graph.number_of_edges(),
            "node_type_counts": node_type_counts,
            "edge_flow_counts": edge_flow_counts,
        }
        (design_dir / GRAPH_SUMMARY_FILENAME).write_text(
            json.dumps(summary, indent=4),
        )
