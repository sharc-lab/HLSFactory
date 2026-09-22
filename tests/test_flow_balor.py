from pathlib import Path

import pytest

from hlsfactory.design_config import FlowConfig, FlowConfigError, FlowName
from hlsfactory.flow_balor import (
    DEFAULT_BALOR_DOCKER_IMAGE,
    HLSFACTORY_BALOR_PATH_ENV_VAR,
    BalorGraphData,
    BalorGraphFlow,
    extract_dot_graph,
    find_graph_compiler,
    flatten_dot_quoted_newlines,
    get_balor_install_dir,
    validate_mode,
)

FIXTURE_DIR = Path(__file__).parent / "fixtures" / "balor"
# Captured from emmetmurphy/balor:latest on kernel_plain.cpp with
# `--mode opt --generalize_types`. Counted independently with grep:
# 57 `^nodeN [` declarations and 104 `^nodeN -> nodeM` declarations.
REAL_FIXTURE = FIXTURE_DIR / "kernel_plain_opt.dot"

# A DOT graph in the shape Graphviz emits. Balor's exact attribute vocabulary is
# not yet verified against a real run, so the summary parser counts attribute
# keys generically rather than assuming specific names.
SAMPLE_DOT = """digraph G {
    node [shape=record];
    "0" [label="alloca", type="instruction", opcode="alloca"];
    "1" [label="load", type="instruction", opcode="load"];
    "2" [label="add", type="instruction", opcode="add"];
    "0" -> "1" [flow="data", position="0"];
    "1" -> "2" [flow="data", position="1"];
    "0" -> "2" [flow="control"];
}
"""


def _make_balor_checkout(root: Path) -> Path:
    compiler = root / "graph_compiler" / "run_graph_compiler.py"
    compiler.parent.mkdir(parents=True, exist_ok=True)
    compiler.write_text("# stub\n")
    return root


def test_flow_name_registered() -> None:
    assert FlowName.BALOR_GRAPH.value == "BalorGraphFlow"


def test_required_settings_registered() -> None:
    with pytest.raises(FlowConfigError):
        FlowConfig(flow_name=FlowName.BALOR_GRAPH.value, flow_settings={})

    config = FlowConfig(
        flow_name=FlowName.BALOR_GRAPH.value,
        flow_settings={"source_file": "kernel.cpp", "top": "kernel"},
    )
    assert config.require_setting("top") == "kernel"


def test_get_balor_install_dir_precedence(tmp_path: Path, monkeypatch) -> None:
    monkeypatch.setenv(HLSFACTORY_BALOR_PATH_ENV_VAR, str(tmp_path / "from_env"))
    assert get_balor_install_dir(tmp_path / "explicit") == tmp_path / "explicit"
    assert get_balor_install_dir() == tmp_path / "from_env"


def test_get_balor_install_dir_unset_selects_docker(monkeypatch) -> None:
    monkeypatch.delenv(HLSFACTORY_BALOR_PATH_ENV_VAR, raising=False)
    assert get_balor_install_dir() is None


def test_find_graph_compiler(tmp_path: Path) -> None:
    root = _make_balor_checkout(tmp_path / "balor")
    assert find_graph_compiler(root).name == "run_graph_compiler.py"


def test_find_graph_compiler_missing(tmp_path: Path) -> None:
    with pytest.raises(FileNotFoundError, match="run_graph_compiler.py"):
        find_graph_compiler(tmp_path / "nope")


@pytest.mark.parametrize("mode", ["base", "opt"])
def test_validate_mode_accepts_valid(mode: str) -> None:
    assert validate_mode(mode) == mode


def test_validate_mode_rejects_invalid() -> None:
    with pytest.raises(ValueError, match="must be one of"):
        validate_mode("optimized")


def test_extract_dot_graph_strips_command_echo() -> None:
    # run_graph_compiler.py echoes the command it runs before the graph.
    output = "./bin/graph_compiler --src /root/k.cpp --top k\ndigraph {\n}\n"
    assert extract_dot_graph(output).startswith("digraph {")


def test_extract_dot_graph_detects_failure() -> None:
    # The compiler aborts with a C++ exception but still exits 0, so the
    # absence of a digraph block is the only reliable failure signal.
    output = (
        "./bin/graph_compiler --src /root/k.cpp --top k\n"
        "terminate called after throwing an instance of 'std::runtime_error'\n"
        "  what():  Couldn't read unroll factor from pragma\n"
    )
    with pytest.raises(ValueError, match="produced no DOT graph"):
        extract_dot_graph(output)


def test_flatten_dot_quoted_newlines() -> None:
    # Balor emits multi-line label="..." values.
    text = 'node0 [label="Line one\nLine two" bitwidth="32" ]\n'
    flattened = flatten_dot_quoted_newlines(text)
    assert flattened.count("\n") == 1
    assert "\\nLine two" in flattened


def test_flatten_dot_leaves_unquoted_newlines() -> None:
    text = 'node0 [a="x"]\nnode1 [b="y"]\n'
    assert flatten_dot_quoted_newlines(text) == text


def test_parse_real_balor_fixture() -> None:
    summary = BalorGraphData.parse_from_dot_file(
        REAL_FIXTURE,
        top="kernel_plain",
        mode="opt",
    )

    assert summary.num_nodes == 57
    assert summary.num_edges == 104
    assert summary.edge_attribute_counts["flowType"] == 104
    # Attributes Balor derives from HLS pragmas.
    for key in ("pipelined", "unrollFactor1", "tripcount", "bitwidth"):
        assert key in summary.node_attribute_counts


def test_parse_real_fixture_handles_multiline_labels() -> None:
    # The raw file has more lines than declarations because labels wrap.
    raw_lines = len(REAL_FIXTURE.read_text().splitlines())
    summary = BalorGraphData.parse_from_dot_file(REAL_FIXTURE, "kernel_plain", "opt")
    assert raw_lines > summary.num_nodes + summary.num_edges
    assert summary.node_attribute_counts["label"] == 57


def test_parse_dot_counts_nodes_and_edges(tmp_path: Path) -> None:
    dot = tmp_path / "graph.dot"
    dot.write_text(SAMPLE_DOT)

    summary = BalorGraphData.parse_from_dot_file(dot, top="kernel", mode="opt")

    assert summary.format == "balor"
    assert summary.top == "kernel"
    assert summary.mode == "opt"
    assert summary.num_nodes == 3
    assert summary.num_edges == 3


def test_parse_dot_collects_attribute_keys(tmp_path: Path) -> None:
    dot = tmp_path / "graph.dot"
    dot.write_text(SAMPLE_DOT)

    summary = BalorGraphData.parse_from_dot_file(dot, top="kernel", mode="opt")

    assert summary.node_attribute_counts == {"label": 3, "type": 3, "opcode": 3}
    assert summary.edge_attribute_counts == {"flow": 3, "position": 2}


def test_parse_dot_ignores_graph_level_defaults(tmp_path: Path) -> None:
    # `node [shape=record];` is a default declaration, not a node.
    dot = tmp_path / "graph.dot"
    dot.write_text(SAMPLE_DOT)
    summary = BalorGraphData.parse_from_dot_file(dot, top="k", mode="opt")
    assert "shape" not in summary.node_attribute_counts


def test_parse_dot_empty_graph(tmp_path: Path) -> None:
    dot = tmp_path / "graph.dot"
    dot.write_text("digraph G {\n}\n")
    summary = BalorGraphData.parse_from_dot_file(dot, top="k", mode="opt")
    assert summary.num_nodes == 0
    assert summary.num_edges == 0


def test_build_command_native(tmp_path: Path) -> None:
    root = _make_balor_checkout(tmp_path / "balor")
    flow = BalorGraphFlow(balor_install_dir=root)

    command = flow.build_command(
        source_path=tmp_path / "kernel.cpp",
        top="kernel_2mm",
        mode="opt",
        output_dir=tmp_path / "out",
        generalize_types=True,
        make_pdf=False,
    )

    assert "run_graph_compiler.py" in command
    assert "--mode opt" in command
    assert "--top kernel_2mm" in command
    assert "--generalize_types" in command
    assert "--make_pdf" not in command
    assert "docker" not in command


def test_build_command_docker(tmp_path: Path, monkeypatch) -> None:
    monkeypatch.delenv(HLSFACTORY_BALOR_PATH_ENV_VAR, raising=False)
    monkeypatch.setattr("shutil.which", lambda _: "/usr/bin/docker")

    flow = BalorGraphFlow()
    command = flow.build_command(
        source_path=tmp_path / "kernel.cpp",
        top="kernel_2mm",
        mode="base",
        output_dir=tmp_path / "out",
        generalize_types=False,
        make_pdf=True,
    )

    assert command.startswith("docker run --rm")
    assert DEFAULT_BALOR_DOCKER_IMAGE in command
    assert "/root/kernel.cpp" in command
    assert "--mode base" in command
    assert "--make_pdf" in command
    assert "--generalize_types" not in command


def test_constructor_without_docker_or_install(monkeypatch) -> None:
    monkeypatch.delenv(HLSFACTORY_BALOR_PATH_ENV_VAR, raising=False)
    monkeypatch.setattr("shutil.which", lambda _: None)

    with pytest.raises(RuntimeError, match="cannot be installed with pip"):
        BalorGraphFlow()


def test_constructor_rejects_bad_install_dir(tmp_path: Path) -> None:
    with pytest.raises(FileNotFoundError, match="run_graph_compiler.py"):
        BalorGraphFlow(balor_install_dir=tmp_path / "not-a-checkout")
