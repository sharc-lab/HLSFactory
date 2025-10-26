from __future__ import annotations

from pathlib import Path
from textwrap import dedent

import pytest

from hlsfactory.design_config import (
    DesignConfig,
    DesignConfigError,
    FlowConfig,
    read_design_config,
    write_design_config,
)

CONFIG_K2MM = """
design_name = "k2mm"
dataset_name = "polybench"
tags = ["linalg"]

[[flow_configs]]
flow_name = "OptDSLv2"
opt_dsl_file = "opt_handcrafted.txt"

[[flow_configs]]
flow_name = "VitisHLSSynthFlow"
synth_tcl = "run_synth_xilinx.tcl"

[[flow_configs]]
flow_name = "VitisHLSCsimFlow"
synth_tcl = "run_csim_xilinx.tcl"
"""


CONFIG_LLAMA = """
design_name = "Llama_GPT_module"
dataset_name = "forgebench"
tags = ["deep_learning"]

[[flow_configs]]
flow_name = "OptDSLv2"
opt_dsl_file = "opt_generated.txt"

[[flow_configs]]
flow_name = "VitisHLSSynthFlow"
synth_tcl = "run_synth_xilinx.tcl"

[[flow_configs]]
flow_name = "LightningSimV2Flow"
"""


CONFIG_LEGACY_SETTINGS = """
design_name = "legacy"
dataset_name = "legacy_dataset"

[[flow_configs]]
flow_name = "OptDSLv2"
flow_settings = { opt_dsl_file = "legacy_opt.txt" }
"""


def _write_config(
    tmp_path: Path, content: str, filename: str = "hlsfactory.toml"
) -> Path:
    path = tmp_path / filename
    path.write_text(dedent(content).strip() + "\n")
    return path


def test_read_design_config_supports_examples(tmp_path: Path) -> None:
    path = _write_config(tmp_path, CONFIG_K2MM)
    config = read_design_config(path)

    assert config.design_name == "k2mm"
    assert config.dataset_name == "polybench"
    assert config.tags == ["linalg"]
    assert config.env_vars == {}

    assert config.supports_flow("OptDSLv2")
    assert (
        config.require_flow_setting("VitisHLSSynthFlow", "synth_tcl")
        == "run_synth_xilinx.tcl"
    )
    assert (
        config.get_flow_setting("VitisHLSCsimFlow", "synth_tcl")
        == "run_csim_xilinx.tcl"
    )


def test_read_second_example(tmp_path: Path) -> None:
    path = _write_config(tmp_path, CONFIG_LLAMA)
    config = read_design_config(path)

    assert config.supports_flow("LightningSimV2Flow")
    assert (
        config.require_flow_config("OptDSLv2").require_setting("opt_dsl_file")
        == "opt_generated.txt"
    )
    assert config.flow_names == (
        "OptDSLv2",
        "VitisHLSSynthFlow",
        "LightningSimV2Flow",
    )


def test_missing_required_vitis_setting_raises(tmp_path: Path) -> None:
    bad_config = """
    design_name = "bad"
    dataset_name = "polybench"

    [[flow_configs]]
    flow_name = "VitisHLSSynthFlow"
    """
    path = _write_config(tmp_path, bad_config)

    with pytest.raises(DesignConfigError):
        read_design_config(path)


def test_legacy_inline_flow_settings_supported(tmp_path: Path) -> None:
    path = _write_config(tmp_path, CONFIG_LEGACY_SETTINGS)
    config = read_design_config(path)

    assert config.require_flow_setting("OptDSLv2", "opt_dsl_file") == "legacy_opt.txt"


def test_write_roundtrip(tmp_path: Path) -> None:
    config = DesignConfig(
        design_name="roundtrip",
        dataset_name="dataset",
        tags=["example"],
        env_vars={"VAR1": "value1", "VAR2": "value2"},
        flow_configs=[
            FlowConfig(
                flow_name="OptDSLv2", flow_settings={"opt_dsl_file": "file.txt"}
            ),
            FlowConfig(
                flow_name="VitisHLSSynthFlow", flow_settings={"synth_tcl": "run.tcl"}
            ),
        ],
    )

    path = tmp_path / "out.toml"
    write_design_config(path, config)

    loaded = read_design_config(path)
    assert loaded == config
