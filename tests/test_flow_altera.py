import json
import os
import shlex
import subprocess
import sys
from argparse import Namespace
from unittest.mock import patch

import pytest

from hlsfactory.datasets_builtin import dataset_altera_examples_builder
from hlsfactory.design_config import DesignConfig, FlowConfig, FlowConfigError, FlowName
from hlsfactory.flow_altera import (
    AlteraHLSCosimFlow,
    AlteraHLSCsimFlow,
    AlteraHLSDesign,
    AlteraHLSSynthData,
    AlteraHLSSynthFlow,
    AlteraImplDesignResource,
    AlteraQuartusImplFlow,
    _QUESTA_INI_COMPAT_SCRIPT,
)
from hlsfactory.framework import Design
from hlsfactory.utils import CallToolResult, ExecutionDataStatus, read_execution_data

ALTERA_DESIGN_NAMES = {
    "anr", "cholesky", "cholesky_inversion", "convolution2d", "crr", "db",
    "decompress", "fft2d", "gzip", "matmul", "merge_sort", "mvdr_beamforming",
    "niosv", "pca", "qrd", "qri", "svd",
}


def write_ndjson(path, rows):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(json.dumps(row) for row in rows))


def reports(project):
    report_dir = project / "reports/resources/json"
    # Columns and rows deliberately differ from the legacy i++ ordering.
    write_ndjson(
        report_dir / "summary.ndjson",
        [
            {
                "summary_type": "estimatedResources",
                "columns": [
                    "Kernel Name",
                    "DSPs ",
                    "RAMs",
                    "FFs",
                    "MLABs",
                    "ALUTs",
                    "Frac. DSPs",
                ],
            },
            {
                "parent": "estimatedResources",
                "name": "Available",
                "data": [10, 20, "3,000", 40, 5000, 0],
            },
            {
                "parent": "estimatedResources",
                "name": "kernel",
                "data": [1, 1, 1, 1, 1, 0],
            },
            {
                "parent": "estimatedResources",
                "name": "Kernel Subtotal",
                "data": [1, 1, 1, 1, 1, 0],
            },
            {
                "parent": "estimatedResources",
                "name": "Total",
                "data": [2, 3, 4, 5, 6, 0],
            },
            {"parent": "performanceSummary", "name": "kernel"},
        ],
    )
    write_ndjson(
        report_dir / "info.ndjson",
        [
            {
                "compileInfo": {
                    "nodes": [
                        {
                            "name": "design",
                            "command": "aoc -clock=300MHz -rtl",
                            "family": "Agilex7",
                            "product": "SYCL",
                            "quartus": "",
                            "time": "today",
                            "version": "2026.1",
                        }
                    ]
                }
            }
        ],
    )
    (project / "kernel.sv").write_text("module kernel; endmodule")
    (project / "reports/report.html").write_text("<html></html>")
    return report_dir


def make_design(tmp_path, flow_name=FlowName.ALTERA_HLS_SYNTH, **settings):
    (tmp_path / "kernel with spaces.cpp").touch()
    config = DesignConfig(
        design_name="test",
        dataset_name="test",
        flow_configs=[
            FlowConfig(
                flow_name,
                {
                    "sources": "'kernel with spaces.cpp'",
                    **settings,
                },
            )
        ],
    )
    return Design("test", tmp_path, config=config)


def fake_synthesis(cmd, cwd, **kwargs):
    script = next(cwd.glob("altera_synth/run_*/run.sh"))
    argv = shlex.split(script.read_text().splitlines()[2])
    assert argv[1] == "kernel with spaces.cpp"
    assert "-Xshardware" in argv and "-fsycl-link=early" in argv
    output = cwd / argv[-1]
    output.touch()
    reports(output.with_suffix(".prj"))
    return CallToolResult.SUCCESS


def test_ndjson_reports(tmp_path):
    directory = reports(tmp_path / "design.prj")
    result = AlteraHLSSynthData.parse_from_synth_report_file(
        directory / "summary.ndjson"
    )
    assert result.resources_ALUTs_used == 6
    assert result.resources_DSPs_used == 2
    assert result.resources_FFs_avail == 3000
    result.to_json(tmp_path / "data_hls.json")
    assert (
        json.loads((tmp_path / "data_hls.json").read_text())["resources_RAMs_used"] == 3
    )
    design = AlteraHLSDesign.parse_from_synth_report_file(
        directory / "info.ndjson",
        directory / "summary.ndjson",
    )
    assert design.kernels == ["kernel"]
    assert design.target_clock == "300MHz"


def test_missing_total_is_not_kernel_subtotal(tmp_path):
    directory = reports(tmp_path / "design.prj")
    summary = directory / "summary.ndjson"
    rows = [json.loads(line) for line in summary.read_text().splitlines()]
    write_ndjson(summary, [row for row in rows if row.get("name") != "Total"])
    with pytest.raises(ValueError, match="Total"):
        AlteraHLSSynthData.parse_from_synth_report_file(summary)


@pytest.mark.parametrize("value", ["TBD", "1,234", "17330.7"])
def test_quartus_report_requires_fitter_results(tmp_path, value):
    report = tmp_path / "quartus.ndjson"
    write_ndjson(
        report,
        [
            {
                "quartusFitResourceUsageSummary": {
                    "nodes": [{"alm": value, "reg": "2345"}]
                },
                "quartusFitClockSummary": {"nodes": [{"kernel clock": "300"}]},
            }
        ],
    )
    if value == "TBD":
        with pytest.raises(ValueError, match="No completed"):
            AlteraImplDesignResource.parse_from_impl_report_file(report)
    else:
        data = AlteraImplDesignResource.parse_from_impl_report_file(report)
        assert data.resource_usage[0]["alm"] == value


def test_synthesis_collects_and_skips_success(tmp_path):
    design = make_design(tmp_path)
    flow = AlteraHLSSynthFlow(ahls_bin="ahls")
    with patch("hlsfactory.flow_altera.call_tool", side_effect=fake_synthesis) as call:
        assert flow.execute(design, timeout=13) == [design]
        assert flow.execute(design) == [design]
    assert call.call_count == 1
    assert call.call_args.kwargs["timeout"] == 13
    assert (
        read_execution_data(tmp_path, flow.name).status == ExecutionDataStatus.SUCCESS
    )
    artifacts = json.loads((tmp_path / flow.result_file).read_text())
    assert artifacts["rtl_files"] and (tmp_path / artifacts["archive"]).exists()
    assert (tmp_path / "data_design.json").exists()


@pytest.mark.parametrize("result", [CallToolResult.ERROR, CallToolResult.TIMEOUT])
def test_failure_can_be_retried_without_old_artifacts(tmp_path, result):
    design = make_design(tmp_path)
    flow = AlteraHLSSynthFlow(ahls_bin="ahls")
    with patch("hlsfactory.flow_altera.call_tool", return_value=result) as call:
        assert flow.execute(design, timeout=1) == []
        assert flow.execute(design, timeout=1) == []
    assert call.call_count == 2
    assert len(list(tmp_path.glob("altera_synth/run_*"))) == 2
    expected = "timeout" if result == CallToolResult.TIMEOUT else "error"
    assert (tmp_path / f"{expected}__{flow.name}.txt").exists()
    assert read_execution_data(tmp_path, flow.name).status.value == expected
    assert not (tmp_path / "data_hls.json").exists()


def test_missing_output_is_error(tmp_path):
    flow = AlteraHLSSynthFlow(ahls_bin="ahls")
    with patch("hlsfactory.flow_altera.call_tool", return_value=CallToolResult.SUCCESS):
        assert flow.execute(make_design(tmp_path)) == []
    assert read_execution_data(tmp_path, flow.name).status == ExecutionDataStatus.ERROR


def test_missing_rtl_is_error(tmp_path):
    def incomplete(cmd, cwd, **kwargs):
        result = fake_synthesis(cmd, cwd, **kwargs)
        next(cwd.glob("altera_synth/run_*/design.prj/kernel.sv")).unlink()
        return result

    flow = AlteraHLSSynthFlow(ahls_bin="ahls")
    with patch("hlsfactory.flow_altera.call_tool", side_effect=incomplete):
        assert flow.execute(make_design(tmp_path)) == []
    assert not (tmp_path / "data_hls.json").exists()


@pytest.mark.parametrize("exit_code", [0, 7])
def test_csim_checks_testbench_exit_code(tmp_path, exit_code):
    # Exercise the actual shell, including successful compilation followed by
    # a failing testbench. All paths and arguments include spaces.
    compiler = tmp_path / "fake compiler"
    compiler.write_text(
        "#!/bin/bash\n"
        'output="${@: -1}"\n'
        f"printf '#!/bin/bash\\nexit {exit_code}\\n' > \"$output\"\n"
        'chmod +x "$output"\n'
    )
    compiler.chmod(0o755)
    flow = AlteraHLSCsimFlow(ahls_bin=compiler)
    design = make_design(
        tmp_path, FlowName.ALTERA_HLS_CSIM, run_args="'argument with spaces'"
    )
    assert flow.execute(design, timeout=5) == ([design] if exit_code == 0 else [])
    expected = (
        ExecutionDataStatus.SUCCESS if exit_code == 0 else ExecutionDataStatus.ERROR
    )
    assert read_execution_data(tmp_path, flow.name).status == expected
    assert next(tmp_path.glob("altera_csim/run_*/design.fpga_emu")).exists()


@pytest.mark.parametrize("install_env", [None, ""])
def test_tool_resolution_requires_configuration(monkeypatch, install_env):
    if install_env is None:
        monkeypatch.delenv("HLSFACTORY_ALTERA_HLS_PATH", raising=False)
    else:
        monkeypatch.setenv("HLSFACTORY_ALTERA_HLS_PATH", install_env)
    monkeypatch.setenv("PATH", "")
    with pytest.raises(FileNotFoundError, match="HLSFACTORY_ALTERA_HLS_PATH"):
        AlteraHLSSynthFlow()


@pytest.mark.parametrize("tool", ["ahls", "ahls-sh"])
def test_tool_resolution_from_path(tmp_path, monkeypatch, tool):
    executable = tmp_path / tool
    executable.touch()
    executable.chmod(0o755)
    monkeypatch.delenv("HLSFACTORY_ALTERA_HLS_PATH", raising=False)
    monkeypatch.setenv("PATH", str(tmp_path))
    flow = AlteraHLSSynthFlow()
    assert (flow.ahls_bin if tool == "ahls" else flow.ahls_sh) == str(executable)


def test_tool_resolution(tmp_path, monkeypatch):
    launcher = tmp_path / "bin/ahls-sh"
    launcher.parent.mkdir()
    launcher.touch()
    monkeypatch.setenv("HLSFACTORY_ALTERA_HLS_PATH", str(tmp_path))
    assert AlteraHLSSynthFlow().ahls_sh == str(launcher)
    assert AlteraHLSSynthFlow(ahls_bin="explicit").ahls_bin == "explicit"
    with pytest.raises(ValueError, match="either"):
        AlteraHLSSynthFlow(ahls_bin="ahls", ahls_sh=launcher)
    with pytest.raises(FileNotFoundError):
        AlteraHLSSynthFlow(altera_install_dir=tmp_path / "missing")


@pytest.mark.parametrize(
    "flow",
    [
        FlowName.ALTERA_HLS_SYNTH,
        FlowName.ALTERA_HLS_CSIM,
        FlowName.ALTERA_HLS_COSIM,
        FlowName.ALTERA_QUARTUS_IMPL,
    ],
)
def test_required_sources(flow):
    with pytest.raises(FlowConfigError, match="sources"):
        FlowConfig(flow, {})


def test_builtin_dataset(tmp_path):
    dataset = dataset_altera_examples_builder("altera", tmp_path)
    assert {design.name for design in dataset.designs} == ALTERA_DESIGN_NAMES
    for design in dataset.designs:
        flows = [AlteraHLSSynthFlow, AlteraQuartusImplFlow]
        if design.name in {"matmul", "merge_sort"}:
            flows += [AlteraHLSCsimFlow, AlteraHLSCosimFlow]
        for cls in flows:
            settings = design.require_config().require_flow_config(cls.flow_name)
            for source in shlex.split(settings.require_setting("sources")):
                assert (design.dir / source).is_file()
            for flag in shlex.split(settings.get_setting("cxxflags", "")):
                if flag.startswith("-I"):
                    assert (design.dir / flag[2:]).is_dir()
        assert (design.dir / "License.txt").exists()
        config = design.require_config()
        synth = config.require_flow_config(FlowName.ALTERA_HLS_SYNTH)
        impl = config.require_flow_config(FlowName.ALTERA_QUARTUS_IMPL)
        for key in ("sources", "cxxflags", "target", "clock", "fpga_flags"):
            assert synth.get_setting(key) == impl.get_setting(key)


def test_implementation_collects_separate_fitter_data(tmp_path):
    def compile_image(cmd, cwd, **kwargs):
        script = next(cwd.glob("altera_impl/run_*/run.sh"))
        argv = shlex.split(script.read_text().splitlines()[2])
        assert "-fsycl-link=image" in argv
        assert "-fsycl-link=early" not in argv
        output = cwd / argv[-1]
        output.touch()
        directory = reports(output.with_suffix(".prj"))
        write_ndjson(
            directory / "quartus.ndjson",
            [
                {
                    "quartusFitResourceUsageSummary": {"nodes": [{"alm": "1234"}]},
                    "quartusFitClockSummary": {"nodes": [{"kernel clock": "300"}]},
                }
            ],
        )
        return CallToolResult.SUCCESS

    flow = AlteraQuartusImplFlow(ahls_bin="ahls")
    design = make_design(tmp_path, FlowName.ALTERA_QUARTUS_IMPL)
    with patch("hlsfactory.flow_altera.call_tool", side_effect=compile_image):
        assert flow.execute(design) == [design]
    assert json.loads((tmp_path / "data_implementation.json").read_text())[
        "resource_usage"
    ] == [{"alm": "1234"}]
    assert not (tmp_path / "data_hls.json").exists()


@pytest.mark.parametrize("flow_cls", [AlteraHLSSynthFlow, AlteraHLSCosimFlow])
@pytest.mark.parametrize("timeout", [None, 123])
@pytest.mark.parametrize("selected", [None, ["matmul", "merge_sort"]])
def test_validator_rejects_partial_success(tmp_path, flow_cls, timeout, selected):
    from tests import dataset_validator
    from hlsfactory.datasets_builtin import DIR_DATASET_ALTERA_EXAMPLES
    from hlsfactory.framework import DesignDataset

    args = Namespace(
        dataset_source_directory=DIR_DATASET_ALTERA_EXAMPLES,
        work_dir=tmp_path,
        n_jobs=1,
        name=None,
        flow=flow_cls.name,
        altera_install_dir=None,
        timeout=timeout,
        designs=selected,
    )

    def partial(datasets, *args, **kwargs):
        default_timeout = 3600 if flow_cls is AlteraHLSCosimFlow else 720
        assert kwargs["timeout"] == (default_timeout if timeout is None else timeout)
        dataset = datasets["original"]
        assert {d.name for d in dataset.designs} == (
            set(selected) if selected else ALTERA_DESIGN_NAMES
        )
        return {
            "original": DesignDataset(
                "original", dataset.dataset_dir, dataset.designs[:1]
            )
        }

    with (
        patch.object(flow_cls, "__init__", return_value=None),
        patch.object(
            flow_cls,
            "execute_multiple_design_datasets_fine_grained_parallel",
            side_effect=partial,
        ),
    ):
        expected_count = len(selected) if selected else len(ALTERA_DESIGN_NAMES)
        with pytest.raises(RuntimeError, match=f"1/{expected_count}"):
            dataset_validator.main(args)


@pytest.mark.parametrize("selected", [["typo"], ["matmul", "typo"]])
def test_validator_rejects_unknown_design_names(tmp_path, selected):
    from tests import dataset_validator
    from hlsfactory.datasets_builtin import DIR_DATASET_ALTERA_EXAMPLES

    args = Namespace(
        dataset_source_directory=DIR_DATASET_ALTERA_EXAMPLES,
        work_dir=tmp_path,
        n_jobs=1,
        name=None,
        designs=selected,
    )
    with pytest.raises(ValueError, match="Unknown designs: typo"):
        dataset_validator.main(args)


@pytest.mark.parametrize("exit_code", [0, 7])
@pytest.mark.parametrize("waveform_depth", [None, "0", "1"])
@pytest.mark.parametrize("setup_source", [None, "argument", "environment"])
def test_cosim_runtime_and_artifacts(
    tmp_path, monkeypatch, exit_code, waveform_depth, setup_source
):
    monkeypatch.delenv("HLSFACTORY_ALTERA_QUESTA_SETUP", raising=False)
    monkeypatch.delenv("HLSFACTORY_TEST_QUESTA", raising=False)
    monkeypatch.delenv("HLSFACTORY_ALTERA_QUESTA_INI_COMPAT", raising=False)
    monkeypatch.setenv("INTELFPGA_SIM_DEVICE_SPEC_DIR", "/stale/project")
    monkeypatch.setenv("DISPLAY", "unreachable:99")
    setup = tmp_path / "questa setup.sh"
    simulator_bin = tmp_path / "simulator bin"
    simulator_bin.mkdir()
    simulator = simulator_bin / "vsim"
    simulator.write_text("#!/bin/bash\nprintf '[Library]\\n' > questa.ini\n")
    simulator.chmod(0o755)
    setup.write_text(
        "export HLSFACTORY_TEST_QUESTA=ready\nexport DISPLAY=unreachable:98\n"
        "export HLSFACTORY_ALTERA_QUESTA_INI_COMPAT=1\n"
        f"export PATH={shlex.quote(str(simulator_bin))}:\"$PATH\"\n"
    )
    runtime = (
        "#!/bin/bash\nset -e\n"
        '[[ "$PWD" == */altera_cosim/run_* ]] || exit 80\n'
        '[[ "$INTELFPGA_SIM_DEVICE_SPEC_DIR" == "$PWD/design.prj" ]] || exit 81\n'
        '[[ "$1" == "argument with spaces" ]] || exit 82\n'
        "printf 'simulation completed' > design.prj/transcript.log\n"
    )
    if setup_source is not None:
        runtime += '[[ "$HLSFACTORY_TEST_QUESTA" == ready ]] || exit 83\n'
    if waveform_depth is not None:
        runtime += "touch design.prj/vsim.wlf\n"
    runtime += f"exit {exit_code}\n"
    compiler = tmp_path / "fake compiler"
    compiler.write_text(
        f"#!{sys.executable}\n"
        "import os, sys, subprocess\nfrom pathlib import Path\n"
        "assert 'DISPLAY' not in os.environ\n"
        f"assert os.environ.get('HLSFACTORY_TEST_QUESTA') == "
        f"{'ready' if setup_source is not None else None!r}\n"
        "args = sys.argv[1:]\n"
        "assert '-Xssimulation' in args and '-DFPGA_SIMULATOR' in args\n"
        "assert '-Xshardware' not in args and '-Xsemulator' not in args\n"
        "assert not any(a.startswith('-fsycl-link=') for a in args)\n"
        "assert '-Xstarget=Agilex7' in args and '-Xsclock=300MHz' in args\n"
        f"assert [a for a in args if a.startswith('-Xsghdl')] == "
        f"{[f'-Xsghdl={waveform_depth}'] if waveform_depth is not None else []!r}\n"
        "output = Path(args[-1])\nproject = output.with_suffix('.prj')\n"
        "(project / 'reports').mkdir(parents=True)\n"
        "(project / 'ipinterfaces.xml').write_text('<system/>')\n"
        "(project / 'reports/report.html').write_text('<html/>')\n"
        "stats = project / 'reports/resources/json/sim_stats.ndjson'\n"
        "stats.parent.mkdir(parents=True)\n"
        "stats.write_text('{}\\n')\n"
        "if os.environ.get('HLSFACTORY_ALTERA_QUESTA_INI_COMPAT') == '1':\n"
        "    subprocess.run(['vsim', '-c'], cwd=project, check=True)\n"
        "    assert (project / 'modelsim.ini').read_text() == '[Library]\\n'\n"
        f"output.write_text({runtime!r})\n"
        "output.chmod(0o755)\n"
    )
    compiler.chmod(0o755)
    settings = {"run_args": "'argument with spaces'", "clock": "300MHz"}
    if waveform_depth is not None:
        settings["waveform_depth"] = waveform_depth
    design = make_design(tmp_path, FlowName.ALTERA_HLS_COSIM, **settings)
    if setup_source == "environment":
        monkeypatch.setenv("HLSFACTORY_ALTERA_QUESTA_SETUP", str(setup))
    flow = AlteraHLSCosimFlow(
        ahls_bin=compiler,
        questa_setup_script=setup if setup_source == "argument" else None,
    )
    assert flow.execute(design, timeout=5) == ([design] if exit_code == 0 else [])
    assert read_execution_data(tmp_path, flow.name).status.value == (
        "success" if exit_code == 0 else "error"
    )
    assert read_execution_data(tmp_path, flow.name).dt > 0
    if exit_code == 0:
        result = json.loads((tmp_path / flow.result_file).read_text())
        assert result["status"] == "success"
        assert (tmp_path / result["interface_spec"]).is_file()
        assert any(p.endswith("transcript.log") for p in result["log_files"])
        assert len(result["waveform_files"]) == (0 if waveform_depth is None else 1)
        assert len(result["statistics_files"]) == 1
        assert (tmp_path / result["statistics_files"][0]).is_file()
    else:
        assert not (tmp_path / flow.result_file).exists()
    assert not (tmp_path / "data_hls.json").exists()
    # The simulator environment is scoped to the child, not the Python process.
    assert os.environ["INTELFPGA_SIM_DEVICE_SPEC_DIR"] == "/stale/project"
    assert os.environ["DISPLAY"] == "unreachable:99"
    assert "HLSFACTORY_TEST_QUESTA" not in os.environ


@pytest.mark.parametrize("exit_code", [0, 9])
@pytest.mark.parametrize("existing_ini", [False, True])
def test_questa_ini_compat_preserves_status_and_existing_file(
    tmp_path, exit_code, existing_ini
):
    simulator = tmp_path / "real vsim"
    simulator.write_text(
        "#!/bin/bash\n"
        '[[ "$1" == "argument with spaces" ]] || exit 82\n'
        "printf '[Library]\\n' > questa.ini\n"
        f"exit {exit_code}\n"
    )
    simulator.chmod(0o755)
    wrapper = tmp_path / "vsim"
    wrapper.write_text(_QUESTA_INI_COMPAT_SCRIPT)
    if existing_ini:
        (tmp_path / "modelsim.ini").write_text("original")
    result = subprocess.run(
        ["bash", str(wrapper), "argument with spaces"],
        cwd=tmp_path,
        env={**os.environ, "HLSFACTORY_ALTERA_REAL_VSIM": str(simulator)},
        check=False,
    )
    assert result.returncode == exit_code
    ini = tmp_path / "modelsim.ini"
    if existing_ini:
        assert ini.read_text() == "original"
        assert not ini.is_symlink()
    elif exit_code == 0:
        assert ini.is_symlink() and ini.read_text() == "[Library]\n"
    else:
        assert not ini.exists()


def test_cosim_setup_script_validation_and_precedence(tmp_path, monkeypatch):
    monkeypatch.setenv("HLSFACTORY_ALTERA_QUESTA_SETUP", str(tmp_path / "missing.sh"))
    with pytest.raises(FileNotFoundError):
        AlteraHLSCosimFlow(ahls_bin="ahls")
    setup = tmp_path / "setup.sh"
    setup.touch()
    flow = AlteraHLSCosimFlow(ahls_bin="ahls", questa_setup_script=setup)
    assert flow.questa_setup_script == setup


@pytest.mark.parametrize("depth", ["-1", "all", "1.5", ""])
def test_cosim_invalid_waveform_depth(tmp_path, depth):
    design = make_design(tmp_path, FlowName.ALTERA_HLS_COSIM, waveform_depth=depth)
    with pytest.raises(ValueError, match="waveform_depth"):
        AlteraHLSCosimFlow(ahls_bin="ahls").execute(design)


def test_cosim_timeout_is_retryable(tmp_path):
    design = make_design(tmp_path, FlowName.ALTERA_HLS_COSIM)
    flow = AlteraHLSCosimFlow(ahls_bin="ahls")
    with patch(
        "hlsfactory.flow_altera.call_tool", return_value=CallToolResult.TIMEOUT
    ) as run:
        assert flow.execute(design, timeout=2) == []
        assert flow.execute(design, timeout=2) == []
    assert run.call_count == 2
    assert (
        read_execution_data(tmp_path, flow.name).status == ExecutionDataStatus.TIMEOUT
    )
    assert not (tmp_path / flow.result_file).exists()


@pytest.mark.parametrize("missing", ["ipinterfaces.xml", "reports/report.html"])
def test_cosim_missing_artifact_is_error(tmp_path, missing):
    design = make_design(tmp_path, FlowName.ALTERA_HLS_COSIM)
    flow = AlteraHLSCosimFlow(ahls_bin="ahls")

    def incomplete_compile(cmd, cwd, **kwargs):
        attempt = next(cwd.glob("altera_cosim/run_*"))
        (attempt / "design").touch()
        project = attempt / "design.prj"
        (project / "reports").mkdir(parents=True)
        for artifact in ("ipinterfaces.xml", "reports/report.html"):
            if artifact != missing:
                (project / artifact).touch()
        return CallToolResult.SUCCESS

    with patch("hlsfactory.flow_altera.call_tool", side_effect=incomplete_compile):
        assert flow.execute(design) == []
    assert read_execution_data(tmp_path, flow.name).status == ExecutionDataStatus.ERROR
    assert not (tmp_path / flow.result_file).exists()
    assert missing in (tmp_path / f"error__{flow.name}.txt").read_text()
