import shutil
from pathlib import Path

import pytest

from hlsfactory.design_config import FlowConfigError, FlowName

pytest.importorskip(
    "networkx",
    reason="HARP needs the optional `harp` extra: pip install 'hlsfactory[harp]'",
)

from hlsfactory.harp.harp_graph import (  
    emit_llvm_ir,
    extract_function_names,
    get_harp_clang_bin,
    get_icmp,
    get_pragmas_loops,
)

KERNEL_SOURCE = """void vecadd(int a[64], int b[64], int c[64]) {
    #pragma HLS pipeline II=1
    for (int i = 0; i < 64; i++) {
        c[i] = a[i] + b[i];
    }
}
"""

DESIGN_CONFIG = """design_name = "vecadd"
dataset_name = "harp_test"

[[flow_configs]]
flow_name = "HARPGraphFlow"
source_file = "vecadd.c"
top = "vecadd"
"""

clang_available = pytest.mark.skipif(
    shutil.which(get_harp_clang_bin()) is None,
    reason="HARP requires a clang no newer than 14",
)


def _write_design(design_dir: Path) -> Path:
    design_dir.mkdir(parents=True, exist_ok=True)
    (design_dir / "vecadd.c").write_text(KERNEL_SOURCE)
    (design_dir / "hlsfactory.toml").write_text(DESIGN_CONFIG)
    return design_dir


def test_flow_name_registered() -> None:
    assert FlowName.HARP_GRAPH.value == "HARPGraphFlow"


def test_required_settings_registered() -> None:
    from hlsfactory.design_config import FlowConfig

    with pytest.raises(FlowConfigError):
        FlowConfig(flow_name=FlowName.HARP_GRAPH.value, flow_settings={})

    config = FlowConfig(
        flow_name=FlowName.HARP_GRAPH.value,
        flow_settings={"source_file": "vecadd.c", "top": "vecadd"},
    )
    assert config.require_setting("top") == "vecadd"


def test_extract_function_names_finds_the_function() -> None:
    names = extract_function_names(KERNEL_SOURCE)
    assert [n for n, _ in names] == ["vecadd"]


def test_extract_function_names_ignores_control_keywords() -> None:
    names = [n for n, _ in extract_function_names(KERNEL_SOURCE)]
    assert "for" not in names


def test_extract_function_names_ignores_bare_control_blocks() -> None:
    code = "int f(int x) {\n    while (x > 0) {\n        x--;\n    }\n    return x;\n}\n"
    names = [n for n, _ in extract_function_names(code)]
    assert names == ["f"]


def test_get_pragmas_loops_collects_the_triggering_pragma(tmp_path: Path) -> None:
    design_dir = _write_design(tmp_path / "vecadd")
    for_dict, for_count = get_pragmas_loops(str(design_dir), "vecadd")

    assert for_count == 1
    assert list(for_dict) == ["vecadd"]
    (_, pragmas), = for_dict["vecadd"].values()
    assert pragmas == ["#pragma HLS pipeline II=1"]


@clang_available
def test_emit_llvm_ir_produces_named_blocks(tmp_path: Path) -> None:
    design_dir = _write_design(tmp_path / "vecadd")
    ll_file = Path(emit_llvm_ir("vecadd", str(design_dir)))

    assert ll_file.is_file()
    text = ll_file.read_text()
    assert "for.cond:" in text
    assert "noundef" not in text
    assert "i32*" in text


@clang_available
def test_emit_llvm_ir_missing_source(tmp_path: Path) -> None:
    tmp_path.mkdir(parents=True, exist_ok=True)
    with pytest.raises(FileNotFoundError, match="No HARP source file"):
        emit_llvm_ir("nope", str(tmp_path))


@clang_available
def test_llvm_and_source_loop_counts_agree(tmp_path: Path) -> None:
    design_dir = _write_design(tmp_path / "vecadd")
    emit_llvm_ir("vecadd", str(design_dir))

    _, llvm_count = get_icmp(str(design_dir), "vecadd")
    _, source_count = get_pragmas_loops(str(design_dir), "vecadd")

    assert llvm_count == source_count == 1
