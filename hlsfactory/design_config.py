import tomllib
from collections.abc import Iterable, Mapping
from dataclasses import dataclass, field
from enum import StrEnum
from pathlib import Path
from typing import Any, ClassVar


class DesignConfigError(ValueError):
    """Raised when a design configuration is invalid."""


class FlowConfigError(ValueError):
    """Raised when a flow configuration is invalid."""


class FlowName(StrEnum):
    OPT_DSL_V2 = "OptDSLv2"
    VITIS_HLS_SYNTH = "VitisHLSSynthFlow"
    VITIS_HLS_CSIM = "VitisHLSCsimFlow"
    LIGHTNING_SIM_V2 = "LightningSimV2Flow"


def _ensure_non_empty_str(
    value: Any,
    *,
    field_name: str,
    error_cls: type[Exception] = DesignConfigError,
) -> str:
    if not isinstance(value, str) or not value.strip():
        raise error_cls(f"`{field_name}` must be a non-empty string; got {value!r}.")
    return value.strip()


def _ensure_str_sequence(
    values: Iterable[Any],
    *,
    field_name: str,
    error_cls: type[Exception] = DesignConfigError,
) -> list[str]:
    result: list[str] = []
    for item in values:
        if not isinstance(item, str) or not item.strip():
            raise error_cls(
                f"Entries in `{field_name}` must be non-empty strings; got {item!r}.",
            )
        result.append(item.strip())
    return result


def _ensure_str_mapping(
    values: Mapping[Any, Any],
    *,
    field_name: str,
    error_cls: type[Exception] = DesignConfigError,
) -> dict[str, str]:
    result: dict[str, str] = {}
    for key, value in values.items():
        if not isinstance(key, str) or not key:
            raise error_cls(
                f"Keys in `{field_name}` must be non-empty strings; got {key!r}.",
            )
        if not isinstance(value, str):
            raise error_cls(
                f"Values in `{field_name}` must be strings; key {key!r} had value {value!r}.",
            )
        result[key] = value
    return result


def _normalise_flow_name(flow_name: str | FlowName) -> str:
    if isinstance(flow_name, FlowName):
        return flow_name.value
    if not isinstance(flow_name, str):
        raise FlowConfigError(f"Flow name must be a string; got {flow_name!r}.")
    value = flow_name.strip()
    if not value:
        raise FlowConfigError("Flow name must be a non-empty string.")
    return value


@dataclass(slots=True)
class FlowConfig:
    """Configuration for a single tool flow defined in ``hlsfactory.toml``."""

    flow_name: str
    flow_settings: dict[str, str] = field(default_factory=dict)

    _REQUIRED_SETTINGS: ClassVar[dict[str, frozenset[str]]] = {
        FlowName.OPT_DSL_V2.value: frozenset({"opt_dsl_file"}),
        FlowName.VITIS_HLS_SYNTH.value: frozenset({"synth_tcl"}),
        FlowName.VITIS_HLS_CSIM.value: frozenset({"synth_tcl"}),
    }

    def __post_init__(self) -> None:
        self.flow_name = _normalise_flow_name(self.flow_name)
        self.flow_settings = _ensure_str_mapping(
            self.flow_settings,
            field_name="flow_settings",
            error_cls=FlowConfigError,
        )
        self._validate_required_settings()

    @property
    def required_settings(self) -> frozenset[str]:
        return self._REQUIRED_SETTINGS.get(self.flow_name, frozenset())

    @property
    def is_known_flow(self) -> bool:
        try:
            FlowName(self.flow_name)
        except ValueError:
            return False
        return True

    def has_setting(self, key: str) -> bool:
        return key in self.flow_settings

    def get_setting(self, key: str, default: str | None = None) -> str | None:
        return self.flow_settings.get(key, default)

    def require_setting(self, key: str) -> str:
        if key not in self.flow_settings:
            raise FlowConfigError(
                f"Flow `{self.flow_name}` is missing required setting `{key}`.",
            )
        return self.flow_settings[key]

    def _validate_required_settings(self) -> None:
        missing = [
            key for key in self.required_settings if key not in self.flow_settings
        ]
        if missing:
            raise FlowConfigError(
                "Flow `{name}` is missing required setting(s): {missing}.".format(
                    name=self.flow_name,
                    missing=", ".join(sorted(missing)),
                ),
            )

    @classmethod
    def from_mapping(cls, data: Mapping[str, Any]) -> "FlowConfig":
        if "flow_name" not in data:
            raise FlowConfigError("Each flow entry must define `flow_name`.")

        raw_flow_name = data["flow_name"]
        if not isinstance(raw_flow_name, str):
            raise FlowConfigError(
                f"`flow_name` must be a string; got {raw_flow_name!r}.",
            )

        settings: dict[str, str] = {}
        flow_settings_value = data.get("flow_settings")
        if flow_settings_value is not None:
            if not isinstance(flow_settings_value, Mapping):
                raise FlowConfigError(
                    "`flow_settings` must be a table of string values."
                )
            settings.update(
                _ensure_str_mapping(
                    flow_settings_value,
                    field_name="flow_settings",
                    error_cls=FlowConfigError,
                ),
            )

        for key, value in data.items():
            if key in {"flow_name", "flow_settings"}:
                continue
            if not isinstance(value, str):
                raise FlowConfigError(
                    f"Additional flow setting `{key}` must be a string; got {value!r}.",
                )
            settings[key] = value

        return cls(flow_name=raw_flow_name, flow_settings=settings)


@dataclass(slots=True)
class DesignConfig:
    """In-memory representation of ``hlsfactory.toml``."""

    design_name: str
    dataset_name: str

    tags: list[str] = field(default_factory=list)
    env_vars: dict[str, str] = field(default_factory=dict)
    flow_configs: list[FlowConfig] = field(default_factory=list)

    def __post_init__(self) -> None:
        self.design_name = _ensure_non_empty_str(
            self.design_name, field_name="design_name"
        )
        self.dataset_name = _ensure_non_empty_str(
            self.dataset_name, field_name="dataset_name"
        )
        self.tags = _ensure_str_sequence(self.tags, field_name="tags")
        self.env_vars = _ensure_str_mapping(self.env_vars, field_name="env_vars")

        flow_configs: list[FlowConfig] = []
        for flow in self.flow_configs:
            if not isinstance(flow, FlowConfig):
                raise DesignConfigError(
                    f"Entries in `flow_configs` must be FlowConfig instances; got {flow!r}.",
                )
            flow_configs.append(flow)

        seen: set[str] = set()
        for flow in flow_configs:
            if flow.flow_name in seen:
                raise DesignConfigError(
                    f"Duplicate flow configuration for `{flow.flow_name}` is not allowed.",
                )
            seen.add(flow.flow_name)

        self.flow_configs = flow_configs

    def supports_flow(self, flow_name: str | FlowName) -> bool:
        try:
            name = _normalise_flow_name(flow_name)
        except FlowConfigError as exc:
            raise DesignConfigError(str(exc)) from exc
        return any(flow.flow_name == name for flow in self.flow_configs)

    def get_flow_config(self, flow_name: str | FlowName) -> FlowConfig | None:
        try:
            name = _normalise_flow_name(flow_name)
        except FlowConfigError as exc:
            raise DesignConfigError(str(exc)) from exc
        for flow in self.flow_configs:
            if flow.flow_name == name:
                return flow
        return None

    def require_flow_config(self, flow_name: str | FlowName) -> FlowConfig:
        try:
            name = _normalise_flow_name(flow_name)
        except FlowConfigError as exc:
            raise DesignConfigError(str(exc)) from exc
        flow = self.get_flow_config(name)
        if flow is None:
            raise DesignConfigError(f"Flow `{name}` is not configured.")
        return flow

    def get_flow_setting(
        self,
        flow_name: str | FlowName,
        setting_name: str,
        *,
        default: str | None = None,
    ) -> str | None:
        flow = self.require_flow_config(flow_name)
        return flow.get_setting(setting_name, default)

    def require_flow_setting(self, flow_name: str | FlowName, setting_name: str) -> str:
        flow = self.require_flow_config(flow_name)
        return flow.require_setting(setting_name)

    @property
    def flow_names(self) -> tuple[str, ...]:
        return tuple(flow.flow_name for flow in self.flow_configs)

    @classmethod
    def from_mapping(cls, data: Mapping[str, Any]) -> "DesignConfig":
        if not isinstance(data, Mapping):
            raise DesignConfigError("Design configuration must be a mapping.")

        try:
            design_name = data["design_name"]
        except KeyError as exc:
            raise DesignConfigError("`design_name` is required.") from exc

        try:
            dataset_name = data["dataset_name"]
        except KeyError as exc:
            raise DesignConfigError("`dataset_name` is required.") from exc

        raw_tags = data.get("tags", [])
        if not isinstance(raw_tags, list):
            raise DesignConfigError("`tags` must be a list of strings if provided.")

        raw_env_vars = data.get("env_vars", {})
        if not isinstance(raw_env_vars, Mapping):
            raise DesignConfigError(
                "`env_vars` must be a table of string values if provided."
            )

        raw_flow_configs = data.get("flow_configs", [])
        if not isinstance(raw_flow_configs, list):
            raise DesignConfigError("`flow_configs` must be an array of tables.")

        flow_configs: list[FlowConfig] = []
        for idx, entry in enumerate(raw_flow_configs):
            if not isinstance(entry, Mapping):
                raise DesignConfigError(
                    f"Entry {idx} in `flow_configs` must be a table, got {type(entry).__name__}.",
                )
            try:
                flow_configs.append(FlowConfig.from_mapping(entry))
            except FlowConfigError as exc:
                raise DesignConfigError(
                    f"Invalid flow configuration at index {idx}: {exc}",
                ) from exc

        return cls(
            design_name=design_name,
            dataset_name=dataset_name,
            tags=raw_tags,
            env_vars=raw_env_vars,
            flow_configs=flow_configs,
        )


def read_design_config(file_path: Path) -> DesignConfig:
    """Parse an ``hlsfactory.toml`` file into a :class:`DesignConfig`."""

    try:
        with file_path.open("rb") as fh:
            raw_config = tomllib.load(fh)
    except FileNotFoundError:
        raise
    except OSError as exc:
        raise DesignConfigError(
            f"Could not read config file {file_path}: {exc}"
        ) from exc

    return DesignConfig.from_mapping(raw_config)


def write_design_config(file_path: Path, config: DesignConfig) -> None:
    """Serialize a :class:`DesignConfig` to TOML."""

    content_lines: list[str] = []
    content_lines.append(f"design_name = {_quote_toml_string(config.design_name)}\n")
    content_lines.append(f"dataset_name = {_quote_toml_string(config.dataset_name)}\n")

    if config.tags:
        tags_literal = ", ".join(_quote_toml_string(tag) for tag in config.tags)
        content_lines.append(f"tags = [{tags_literal}]\n")

    if config.env_vars:
        content_lines.append("\n[env_vars]\n")
        for key in sorted(config.env_vars):
            value = config.env_vars[key]
            content_lines.append(f"{key} = {_quote_toml_string(value)}\n")

    for flow in config.flow_configs:
        content_lines.append("\n[[flow_configs]]\n")
        content_lines.append(f"flow_name = {_quote_toml_string(flow.flow_name)}\n")
        for key in sorted(flow.flow_settings):
            value = flow.flow_settings[key]
            content_lines.append(f"{key} = {_quote_toml_string(value)}\n")

    file_path.write_text("".join(content_lines))


def _quote_toml_string(value: str) -> str:
    escaped = value.replace("\\", "\\\\").replace('"', '\\"')
    return f'"{escaped}"'


__all__ = [
    "DesignConfig",
    "DesignConfigError",
    "FlowConfig",
    "FlowConfigError",
    "FlowName",
    "read_design_config",
    "write_design_config",
]
