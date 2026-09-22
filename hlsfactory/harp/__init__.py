from typing import Any

__all__ = [
    "HARPGraphFlow",
    "check_programl_runtime",
    "emit_llvm_ir",
    "get_harp_clang_bin",
    "graph_generator",
    "llvm_to_nx",
    "load_programl",
    "program_graph_to_networkx",
]

_COMPAT_EXPORTS = frozenset(
    {"check_programl_runtime", "load_programl", "program_graph_to_networkx"},
)
_GRAPH_EXPORTS = frozenset(
    {"emit_llvm_ir", "get_harp_clang_bin", "graph_generator", "llvm_to_nx"},
)


def __getattr__(name: str) -> Any:
    if name in _COMPAT_EXPORTS:
        from hlsfactory.harp import _programl_compat

        return getattr(_programl_compat, name)

    if name in _GRAPH_EXPORTS:
        from hlsfactory.harp import harp_graph

        return getattr(harp_graph, name)

    if name == "HARPGraphFlow":
        from hlsfactory.harp.flow import HARPGraphFlow

        return HARPGraphFlow

    raise AttributeError(f"module {__name__!r} has no attribute {name!r}")


def __dir__() -> list[str]:
    return sorted(__all__)
