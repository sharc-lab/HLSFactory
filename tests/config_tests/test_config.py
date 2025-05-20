from pathlib import Path
from pprint import pp

from hlsfactory.flow_config import (
    DesignConfig,
    FlowConfig,
    read_design_config,
    write_design_config,
)

DIR_CURRENT = Path(__file__).parent

FP_TEST_CONFIG = DIR_CURRENT / "test_config.toml"


def test_config_read() -> None:
    from hlsfactory.flow_config import read_design_config

    config = read_design_config(FP_TEST_CONFIG)
    assert config
    pp(config)


def test_config_write(tmp_path: Path) -> None:
    config = DesignConfig(
        design_name="test_design_write",
        dataset_name="test_dataset_write",
        env_vars={"VAR1": "value1", "VAR2": "value2"},
        flow_configs=[
            FlowConfig(flow_name="flow1", flow_settings={"setting1": "value1"}),
            FlowConfig(flow_name="flow2", flow_settings={"setting2": "value2"}),
        ],
    )
    write_design_config(tmp_path / "test_config_write.toml", config)
    config_read = read_design_config(tmp_path / "test_config_write.toml")
    assert config == config_read
