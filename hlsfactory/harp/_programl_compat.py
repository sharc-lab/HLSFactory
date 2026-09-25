"""Compatibility shims for the pinned `programl` release used by HARP.

`programl` 0.3.2 is the latest published release and predates several
dependency changes. Importing and using it on a modern stack needs four
adjustments, all applied by :func:`load_programl`:

1. Its generated ``*_pb2.py`` modules were built with protoc < 3.19 and are
   rejected by modern protobuf unless the pure-Python implementation is used.
2. ``programl.transform_ops`` imports ``dgl`` unconditionally to provide
   ``to_dgl``, which HARP never calls. A real ``dgl`` drags in ``torch`` plus a
   ``libgraphbolt`` built for one exact torch version, so a stub is injected
   instead.
3. ``programl.to_networkx`` calls ``node_link_graph`` without naming the edges
   key. networkx >= 3.4 changed that default from ``links`` to ``edges`` while
   programl still emits ``links``, so :func:`program_graph_to_networkx` does the
   conversion explicitly rather than pinning networkx.
4. The bundled ``llvm2graph-10`` binary needs ``libtinfo.so.5``. That is an
   environment prerequisite and cannot be fixed from Python; see
   :func:`check_programl_runtime`.
"""

from __future__ import annotations

import os
import sys
import types
from typing import Any

PROTOBUF_IMPL_ENV_VAR = "PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION"

_INSTALL_HINT = (
    "HARPGraphFlow requires `networkx` and `programl`. `programl` is not part "
    "of the `harp` extra because it depends on dgl/torch/CUDA, none of which "
    "HARP uses. Install both:\n"
    "  pip install 'hlsfactory[harp]'\n"
    "  pip install programl"
)


def _install_dgl_stub() -> None:
    if "dgl" in sys.modules:
        return

    dgl = types.ModuleType("dgl")
    dgl.__path__ = []  # type: ignore[attr-defined]

    heterograph = types.ModuleType("dgl.heterograph")

    class DGLHeteroGraph:
        """Placeholder for the unused `programl.to_dgl` return type."""

    heterograph.DGLHeteroGraph = DGLHeteroGraph  # type: ignore[attr-defined]
    dgl.heterograph = heterograph  # type: ignore[attr-defined]

    sys.modules["dgl"] = dgl
    sys.modules["dgl.heterograph"] = heterograph


def load_programl() -> Any:
    """Import and return the `programl` module with all shims applied.

    Returns:
        Any: The imported `programl` module.

    Raises:
        ImportError: If `programl` or `networkx` is not installed.
    """
    os.environ.setdefault(PROTOBUF_IMPL_ENV_VAR, "python")
    _install_dgl_stub()

    try:
        import networkx  # noqa: F401
        import programl
    except ImportError as error:
        raise ImportError(f"{_INSTALL_HINT} ({error})") from error

    return programl


def program_graph_to_networkx(program_graph: Any) -> Any:
    """Convert a ProGraML `ProgramGraph` to a networkx `MultiDiGraph`.

    Replaces `programl.to_networkx`, which breaks on networkx >= 3.4.

    Args:
        program_graph (Any): A ProGraML `ProgramGraph`.

    Returns:
        Any: The graph as a networkx `MultiDiGraph`.
    """
    from networkx.readwrite import json_graph as nx_json

    programl = load_programl()
    data = programl.to_json(program_graph)
    edges_key = "links" if "links" in data else "edges"
    return nx_json.node_link_graph(
        data,
        multigraph=True,
        directed=True,
        edges=edges_key,
    )


def check_programl_runtime() -> None:
    """Verify that the bundled ProGraML binary can actually run.

    The `llvm2graph` binaries link against `libtinfo.so.5`, which modern
    distributions no longer ship. Failing here gives a clearer message than the
    `GraphCreationError` raised on first use.

    Raises:
        RuntimeError: If the bundled binary cannot be executed.
    """
    programl = load_programl()

    try:
        programl.from_llvm_ir("define void @probe() {\nentry:\n  ret void\n}\n")
    except Exception as error:  # noqa: BLE001
        message = str(error)
        if "libtinfo.so.5" in message:
            raise RuntimeError(
                "The ProGraML binary bundled with `programl` requires "
                "libtinfo.so.5, which is not available on this system. Install "
                "it (e.g. extract libtinfo5 from a Debian bullseye package) and "
                "add its directory to LD_LIBRARY_PATH.",
            ) from error
        raise RuntimeError(
            f"The ProGraML binary bundled with `programl` failed to run: {message}",
        ) from error
