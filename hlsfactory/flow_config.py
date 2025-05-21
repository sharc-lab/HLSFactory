from dataclasses import field
from pathlib import Path

import tomllib
from pydantic.dataclasses import dataclass


@dataclass
class FlowConfig:
    flow_name: str
    flow_settings: dict[str, str] = field(default_factory=dict)


@dataclass
class DesignConfig:
    design_name: str
    dataset_name: str

    env_vars: dict[str, str] = field(default_factory=dict)

    flow_configs: list[FlowConfig] = field(default_factory=list)


def read_design_config(file_path: Path) -> DesignConfig:
    config = tomllib.loads(file_path.read_text())

    return DesignConfig(
        design_name=config["design_name"],
        dataset_name=config["dataset_name"],
        env_vars=config.get("env_vars", {}),
        flow_configs=[
            FlowConfig(flow_name=flow["flow_name"], flow_settings=flow["flow_settings"])
            for flow in config.get("flow_configs", [])
        ],
    )


def write_design_config(file_path: Path, config: DesignConfig) -> None:
    config_dict = {
        "design_name": config.design_name,
        "dataset_name": config.dataset_name,
        "env_vars": config.env_vars,
        "flow_configs": [
            {"flow_name": flow.flow_name, "flow_settings": flow.flow_settings}
            for flow in config.flow_configs
        ],
    }

    file_path.write_text(tomllib.dumps(config_dict))
