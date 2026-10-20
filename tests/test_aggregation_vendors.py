import zipfile
from pathlib import Path

import pytest

from hlsfactory.data_packaging import (
    ArtifactsAltera,
    ArtifactsCatapult,
    ArtifactsIntel,
    ArtifactsStratus,
    ArtifactsVitisModern,
    ArtifactsXLS,
    DataAggregatorAltera,
    DataAggregatorCatapult,
    DataAggregatorIntel,
    DataAggregatorStratus,
    DataAggregatorVitisModern,
    DataAggregatorXilinx,
    DataAggregatorXLS,
)
from hlsfactory.framework import Design
from hlsfactory.utils import (
    ExecutionDataStatus,
    update_execution_data_with_flow_results,
)


def _touch(root: Path, *rel_paths: str) -> None:
    for rel in rel_paths:
        path = root / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(rel)


# (aggregator, kind enum, files the flow leaves, files that must be ignored,
#  expected files per kind)
CASES = {
    "xls": (
        DataAggregatorXLS,
        ArtifactsXLS,
        [
            "adder.ir",
            "adder.opt.ir",
            "adder.v",
            "adder.signature.textproto",
            "adder.schedule.textproto",
            "adder.interface.pb",
            "adder.codegen_options.textproto",
            "adder.verilog_line_map.textproto",
            "adder.optimizer_ir_dumps/0_pass.ir",
        ],
        ["adder.x", "notes.txt"],
        {
            ArtifactsXLS.IR: {"adder.ir", "adder.opt.ir"},
            ArtifactsXLS.RTL: {"adder.v"},
            ArtifactsXLS.METRICS: {
                "adder.signature.textproto",
                "adder.schedule.textproto",
            },
            ArtifactsXLS.INTERFACE: {"adder.interface.pb"},
            ArtifactsXLS.OPTIONS: {"adder.codegen_options.textproto"},
            ArtifactsXLS.SOURCE_MAP: {"adder.verilog_line_map.textproto"},
            ArtifactsXLS.DEBUG: {"adder.optimizer_ir_dumps/0_pass.ir"},
        },
    ),
    "catapult": (
        DataAggregatorCatapult,
        ArtifactsCatapult,
        ["proj/sol.v1/cycle.rpt", "proj/sol.v1/rtl.rpt", "proj/sol.v1/rtl.v"],
        ["dot_product.cpp", "synth.tcl"],
        {
            ArtifactsCatapult.REPORT: {"proj/sol.v1/cycle.rpt", "proj/sol.v1/rtl.rpt"},
            ArtifactsCatapult.RTL: {"proj/sol.v1/rtl.v"},
        },
    ),
    "stratus": (
        DataAggregatorStratus,
        ArtifactsStratus,
        [
            "modules/fir/BASIC/fir_rtl.v",
            "modules/fir/BASIC/stratus_hls.bdr",
            "modules/fir/BASIC/stratus_hls.log",
            ".hlsfactory_stratus_metrics.tsv",
        ],
        ["fir.cpp", "project.tcl"],
        {
            ArtifactsStratus.RTL: {"modules/fir/BASIC/fir_rtl.v"},
            ArtifactsStratus.REPORT: {
                "modules/fir/BASIC/stratus_hls.bdr",
                ".hlsfactory_stratus_metrics.tsv",
            },
            ArtifactsStratus.LOG: {"modules/fir/BASIC/stratus_hls.log"},
        },
    ),
    "intel": (
        DataAggregatorIntel,
        ArtifactsIntel,
        [
            "atax.prj/reports/report.html",
            "atax.prj/reports/lib/json/summary.json",
            "atax.prj/components/atax/atax.v",
            "atax.prj/quartus/quartus_compile.qsf",
            "atax.prj/quartus/output_files/atax.fit.rpt",
        ],
        ["atax.c", "atax.prj/quartus/big.sof"],
        {
            ArtifactsIntel.REPORT: {
                "atax.prj/reports/report.html",
                "atax.prj/reports/lib/json/summary.json",
            },
            ArtifactsIntel.HDL: {"atax.prj/components/atax/atax.v"},
            ArtifactsIntel.QUARTUS: {
                "atax.prj/quartus/quartus_compile.qsf",
                "atax.prj/quartus/output_files/atax.fit.rpt",
            },
        },
    ),
    "altera": (
        DataAggregatorAltera,
        ArtifactsAltera,
        [
            "build/design.prj/reports/report.html",
            "build/design.prj/reports/resources/json/summary.ndjson",
            "build/design.prj/ipinterfaces.xml",
            "build/design.prj/design.sv",
            "build/flow.log",
        ],
        ["src/matmul_demo.cpp"],
        {
            ArtifactsAltera.REPORT: {
                "build/design.prj/reports/report.html",
                "build/design.prj/reports/resources/json/summary.ndjson",
            },
            ArtifactsAltera.INTERFACE: {"build/design.prj/ipinterfaces.xml"},
            ArtifactsAltera.HDL: {"build/design.prj/design.sv"},
            ArtifactsAltera.LOG: {"build/flow.log"},
        },
    ),
    "vitis_modern": (
        DataAggregatorVitisModern,
        ArtifactsVitisModern,
        [
            "work/hls/syn/report/csynth.xml",
            "work/hls/syn/verilog/accumulator.v",
        ],
        ["accumulator.cpp", "hls_config.cfg"],
        {
            ArtifactsVitisModern.REPORT: {"work/hls/syn/report/csynth.xml"},
            ArtifactsVitisModern.HDL: {"work/hls/syn/verilog/accumulator.v"},
        },
    ),
}


@pytest.fixture(params=sorted(CASES))
def case(request, tmp_path: Path):
    aggregator_cls, kinds, produced, ignored, expected = CASES[request.param]
    _touch(tmp_path, *produced, *ignored)
    return aggregator_cls(), kinds, Design("d", tmp_path), tmp_path, expected


def test_each_kind_finds_only_its_files(case) -> None:
    aggregator, kinds, design, root, expected = case
    data = aggregator.gather_hls_synthesis_artifacts_data(design)
    assert set(data) == {k.value for k in kinds}
    for kind, rel_files in expected.items():
        found = {p.relative_to(root).as_posix() for p in data[kind.value]}
        assert found == rel_files


def test_archive_keeps_relative_paths_by_kind(case) -> None:
    aggregator, _kinds, design, _root, expected = case
    archive = aggregator.gather_hls_synthesis_artifacts(design)
    with zipfile.ZipFile(archive) as zf:
        names = set(zf.namelist())
    assert names == {
        f"artifacts/{kind.value}/{rel}"
        for kind, rels in expected.items()
        for rel in rels
    }


def test_subset_of_kinds(case) -> None:
    aggregator, kinds, design, _root, expected = case
    first = next(iter(expected))
    data = aggregator.gather_hls_synthesis_artifacts_data(
        design,
        artifacts_to_extract={first},
    )
    assert data[first.value]
    assert all(v is None for k, v in data.items() if k != first.value)


def test_missing_kind_returns_empty_unless_asked_to_raise(
    case, tmp_path_factory
) -> None:
    aggregator, kinds, _design, _root, _expected = case
    empty = Design("empty", tmp_path_factory.mktemp("empty_design"))
    data = aggregator.gather_hls_synthesis_artifacts_data(empty)
    assert data == {k.value: [] for k in kinds}
    with pytest.raises(FileNotFoundError):
        aggregator.gather_hls_synthesis_artifacts_data(
            empty,
            error_if_missing_data=True,
        )


def test_foreign_kind_is_rejected(case) -> None:
    aggregator, _kinds, design, _root, _expected = case
    other = (
        ArtifactsCatapult
        if not isinstance(aggregator, DataAggregatorCatapult)
        else ArtifactsXLS
    )
    with pytest.raises(ValueError, match="artifact kind"):
        aggregator.gather_hls_synthesis_artifacts_data(
            design,
            artifacts_to_extract={next(iter(other))},
        )


def test_failed_synthesis_yields_no_artifacts(case) -> None:
    aggregator, _kinds, design, root, _expected = case
    update_execution_data_with_flow_results(
        design_dir=root,
        flow_name=aggregator.SYNTH_FLOW_NAME,
        status=ExecutionDataStatus.ERROR,
        t_start=0.0,
        t_end=1.0,
        error_message="boom",
    )
    assert aggregator.gather_hls_synthesis_artifacts_data(design) == {}


def test_gather_all_data_combines_json_and_artifacts(case, tmp_path_factory) -> None:
    aggregator, _kinds, design, root, _expected = case
    (root / "data_hls.json").write_text('{"latency": 7}')
    result = aggregator.gather_all_data(design)
    assert result.synthesis == {"latency": 7}
    assert result.artifacts is not None
    with pytest.raises(FileNotFoundError):
        aggregator.gather_all_data(
            Design("empty", tmp_path_factory.mktemp("empty_design")),
            error_if_missing_data=True,
        )


def _make_xilinx_solution(root: Path) -> Path:
    solution = root / "proj" / "solution1"
    _touch(
        root,
        "proj/solution1/proj.aps",
        "proj/solution1/.autopilot/db/a.o.3.bc",
        "proj/solution1/.autopilot/db/top.adb",
        "proj/solution1/.autopilot/db/top.adb.xml",
        "proj/solution1/syn/report/csynth.xml",
        "proj/solution1/syn/verilog/top.v",
    )
    (solution / "proj.aps").write_text(
        '<AutoPilot:solution xmlns:AutoPilot="com.autoesl.autopilot.solution">',
    )
    return solution


def test_xilinx_missing_artifacts_do_not_raise_by_default(tmp_path: Path) -> None:
    _make_xilinx_solution(tmp_path)  # no impl/ip, so the ip kind is missing
    design = Design("d", tmp_path)
    aggregator = DataAggregatorXilinx()

    data = aggregator.gather_hls_synthesis_artifacts_data(design)
    assert data["ip"] == []
    assert data["report"]
    assert data["hdl"]

    with pytest.raises(FileNotFoundError):
        aggregator.gather_hls_synthesis_artifacts_data(
            design,
            error_if_missing_data=True,
        )


def test_xilinx_without_any_solution_returns_empty_by_default(tmp_path: Path) -> None:
    design = Design("d", tmp_path)
    aggregator = DataAggregatorXilinx()
    assert aggregator.gather_hls_synthesis_artifacts_data(design) == {}
    with pytest.raises(FileNotFoundError):
        aggregator.gather_hls_synthesis_artifacts_data(
            design,
            error_if_missing_data=True,
        )
