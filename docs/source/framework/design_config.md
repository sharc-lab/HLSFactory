# Design Configuration System (`hlsfactory.toml`)

This document describes the `hlsfactory.toml` design metadata specification, which provides a structured way for HLS designs to declare their supported flows and flow-specific configuration.

## Overview

Each HLS design in HLSFactory can include an `hlsfactory.toml` file that serves as a single, authoritative source of metadata. This file enables:

- **Flow Declaration**: Explicitly declare which HLSFactory flows a design supports
- **Script Configuration**: Specify paths to flow-specific scripts (TCL files, etc.)
- **Design Metadata**: Store canonical name, dataset association, tags, and environment variables
- **Early Validation**: Fail fast with clear errors if required configuration is missing

## Configuration File Format

### Basic Structure

```toml
design_name = "k2mm"
dataset_name = "polybench"
tags = ["linalg", "matrix"]

[env_vars]
CUSTOM_VAR = "value"

[[flow_configs]]
flow_name = "VitisHLSSynthFlow"
synth_tcl = "dataset_hls.tcl"

[[flow_configs]]
flow_name = "VitisHLSImplFlow"
impl_tcl = "dataset_hls_ip_export.tcl"

[[flow_configs]]
flow_name = "OptDSLv2"
opt_dsl_file = "opt_template.tcl"
```

### Required Fields

| Field | Type | Description |
|-------|------|-------------|
| `design_name` | string | Canonical name of the design |
| `dataset_name` | string | Name of the parent dataset |

### Optional Fields

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `tags` | list[string] | `[]` | Descriptive tags for categorization |
| `env_vars` | table | `{}` | Environment variables for flow execution |
| `flow_configs` | array of tables | `[]` | Flow-specific configurations |

## Supported Flows and Required Settings

Each flow type has specific required settings that must be provided:

| Flow Name | Required Setting | Description |
|-----------|-----------------|-------------|
| `VitisHLSSynthFlow` | `synth_tcl` | TCL script for HLS synthesis |
| `VitisHLSImplFlow` | `impl_tcl` | TCL script for IP export/implementation |
| `VitisHLSCsimFlow` | `csim_tcl` | TCL script for C simulation |
| `VitisHLSCosimSetupFlow` | `cosim_setup_tcl` | TCL script for co-simulation setup |
| `VitisHLSCosimFlow` | `cosim_tcl` | TCL script for co-simulation |
| `OptDSLv2` | `opt_dsl_file` | Optimization DSL template file |
| `LightningSimV2Flow` | *(none required)* | No required settings |

### Flow Configuration Examples

```toml
# Vitis HLS Synthesis
[[flow_configs]]
flow_name = "VitisHLSSynthFlow"
synth_tcl = "dataset_hls.tcl"

# Vitis HLS Implementation (IP Export + Vivado)
[[flow_configs]]
flow_name = "VitisHLSImplFlow"
impl_tcl = "dataset_hls_ip_export.tcl"

# C Simulation
[[flow_configs]]
flow_name = "VitisHLSCsimFlow"
csim_tcl = "dataset_hls_csim.tcl"

# Co-Simulation Setup
[[flow_configs]]
flow_name = "VitisHLSCosimSetupFlow"
cosim_setup_tcl = "dataset_hls_cosim_setup.tcl"

# Co-Simulation
[[flow_configs]]
flow_name = "VitisHLSCosimFlow"
cosim_tcl = "dataset_hls_cosim.tcl"

# Optimization DSL Frontend
[[flow_configs]]
flow_name = "OptDSLv2"
opt_dsl_file = "opt_template.tcl"

# LightningSim (no required settings)
[[flow_configs]]
flow_name = "LightningSimV2Flow"
```

## Python API

### Reading and Writing Configs

```python
from pathlib import Path
from hlsfactory.design_config import (
    DesignConfig,
    FlowConfig,
    FlowName,
    read_design_config,
    write_design_config,
)

# Read a config file
config = read_design_config(Path("design_dir/hlsfactory.toml"))

# Access design metadata
print(config.design_name)      # "k2mm"
print(config.dataset_name)     # "polybench"
print(config.tags)             # ["linalg"]

# Check flow support
if config.supports_flow("VitisHLSSynthFlow"):
    synth_tcl = config.require_flow_setting(
        FlowName.VITIS_HLS_SYNTH, "synth_tcl"
    )
    print(f"Synthesis TCL: {synth_tcl}")

# Get flow config object
flow_config = config.get_flow_config("VitisHLSSynthFlow")
if flow_config:
    print(flow_config.flow_settings)

# Create and write a new config
new_config = DesignConfig(
    design_name="my_design",
    dataset_name="my_dataset",
    tags=["example"],
    flow_configs=[
        FlowConfig(
            flow_name=FlowName.VITIS_HLS_SYNTH.value,
            flow_settings={"synth_tcl": "run_synth.tcl"},
        ),
    ],
)
write_design_config(Path("output/hlsfactory.toml"), new_config)
```

### Design and DesignDataset Integration

```python
from hlsfactory.framework import Design, DesignDataset

# Load a single design with its config
design = Design.from_dir_with_config(Path("path/to/design"))
print(design.config.design_name)

# Access config from design (raises if not loaded)
config = design.require_config()
synth_tcl = config.require_flow_setting("VitisHLSSynthFlow", "synth_tcl")

# Load a dataset with automatic config loading (default behavior)
dataset = DesignDataset.from_dir("polybench", Path("path/to/dataset"))
for design in dataset.designs:
    print(f"{design.name}: {design.config.flow_names}")

# Load without requiring configs (for migration/compatibility)
dataset = DesignDataset.from_dir(
    "polybench",
    Path("path/to/dataset"),
    require_config=False
)
```

### Using Config in Custom Flows

```python
from hlsfactory.framework import Design, ToolFlow
from hlsfactory.design_config import FlowName

class MyCustomFlow(ToolFlow):
    name = "MyCustomFlow"

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        # Get config from design
        config = design.require_config()

        # Get flow-specific settings
        my_script = config.require_flow_setting("MyCustomFlow", "script_path")

        # Optional settings with defaults
        optional_param = config.get_flow_setting(
            "MyCustomFlow", "optional_param", default="default_value"
        )

        # Execute flow logic...
        return [design]
```

## Data Classes

### DesignConfig

The main configuration class representing an `hlsfactory.toml` file.

**Attributes:**
- `design_name: str` - Design identifier (required)
- `dataset_name: str` - Parent dataset name (required)
- `tags: list[str]` - Descriptive tags (optional)
- `env_vars: dict[str, str]` - Environment variables (optional)
- `flow_configs: list[FlowConfig]` - Flow configurations (optional)

**Methods:**
- `supports_flow(flow_name)` - Check if design supports a flow
- `get_flow_config(flow_name)` - Get FlowConfig or None
- `require_flow_config(flow_name)` - Get FlowConfig or raise error
- `get_flow_setting(flow_name, setting, default=None)` - Get setting value
- `require_flow_setting(flow_name, setting)` - Get setting or raise error
- `flow_names` - Property returning tuple of all flow names

### FlowConfig

Configuration for a single flow.

**Attributes:**
- `flow_name: str` - Name of the flow
- `flow_settings: dict[str, str]` - Flow-specific key-value settings

**Methods:**
- `has_setting(key)` - Check if setting exists
- `get_setting(key, default=None)` - Get setting value
- `require_setting(key)` - Get setting or raise error
- `is_known_flow` - Property indicating if flow is in FlowName enum
- `required_settings` - Property returning required settings for this flow type

### FlowName Enum

Enumeration of known flow names:

```python
class FlowName(StrEnum):
    OPT_DSL_V2 = "OptDSLv2"
    VITIS_HLS_SYNTH = "VitisHLSSynthFlow"
    VITIS_HLS_CSIM = "VitisHLSCsimFlow"
    VITIS_HLS_IMPL = "VitisHLSImplFlow"
    VITIS_HLS_COSIM = "VitisHLSCosimFlow"
    VITIS_HLS_COSIM_SETUP = "VitisHLSCosimSetupFlow"
    VITIS_HLS_IMPL_REPORT = "VitisHLSImplReportFlow"
    LIGHTNING_SIM_V2 = "LightningSimV2Flow"
```

## Migration Script

A CLI tool is provided to generate `hlsfactory.toml` files for existing designs.

### Usage

```bash
# Preview what would be created (recommended first step)
uv run python -m hlsfactory.scripts.generate_design_configs --dry-run

# Preview with verbose output showing each design
uv run python -m hlsfactory.scripts.generate_design_configs --dry-run --verbose

# Generate configs for all designs
uv run python -m hlsfactory.scripts.generate_design_configs

# Generate configs for a specific dataset only
uv run python -m hlsfactory.scripts.generate_design_configs --dataset polybench

# Generate configs for a specific dataset with preview
uv run python -m hlsfactory.scripts.generate_design_configs \
    --dataset machsuite --dry-run
```

### TCL File Detection

The migration script automatically detects existing TCL files:

| Detected File | Flow | Setting |
|---------------|------|---------|
| `dataset_hls.tcl` | VitisHLSSynthFlow | `synth_tcl` |
| `dataset_hls_ip_export.tcl` | VitisHLSImplFlow | `impl_tcl` |
| `dataset_hls_csim.tcl` | VitisHLSCsimFlow | `csim_tcl` |
| `dataset_hls_cosim_setup.tcl` | VitisHLSCosimSetupFlow | `cosim_setup_tcl` |
| `dataset_hls_cosim.tcl` | VitisHLSCosimFlow | `cosim_tcl` |
| `opt_template.tcl` | OptDSLv2 | `opt_dsl_file` |

## Error Handling

### DesignConfigError

Raised for configuration-level errors:
- Missing required fields (`design_name`, `dataset_name`)
- Invalid field types
- Duplicate flow configurations
- Missing config file when required

### FlowConfigError

Raised for flow configuration errors:
- Missing required flow settings
- Invalid flow setting types
- Empty flow name

### Example Error Messages

```
DesignConfigError: Required config file hlsfactory.toml not found in /path/to/design

DesignConfigError: Design 'my_design' at /path has no loaded configuration.
Ensure hlsfactory.toml exists and was loaded.

FlowConfigError: Flow `VitisHLSSynthFlow` is missing required setting(s): synth_tcl.
```

## Directory Structure

```
design_directory/
├── hlsfactory.toml          # Design configuration (required)
├── dataset_hls.tcl          # Synthesis script
├── dataset_hls_ip_export.tcl # Implementation script
├── opt_template.tcl         # OptDSL template
├── hls_template.tcl         # HLS project template
└── src/
    ├── kernel.cpp
    └── kernel.h
```

## Best Practices

1. **Always include `hlsfactory.toml`** - All designs should have a config file
2. **Use descriptive tags** - Help categorize designs by domain, features, etc.
3. **Validate with dry-run** - Use `--dry-run` before generating configs
4. **Keep TCL names consistent** - Follow naming conventions for easy migration
5. **Check flow support** - Use `supports_flow()` before accessing flow settings

## Extending the System

### Adding a New Flow Type

1. Add the flow name to `FlowName` enum in `design_config.py`:
   ```python
   class FlowName(StrEnum):
       MY_NEW_FLOW = "MyNewFlow"
   ```

2. Add required settings to `_REQUIRED_SETTINGS` in `FlowConfig`:
   ```python
   _REQUIRED_SETTINGS = {
       FlowName.MY_NEW_FLOW.value: frozenset({"my_script"}),
   }
   ```

3. Update flow implementation to read from config:
   ```python
   def execute(self, design: Design, timeout=None):
       config = design.require_config()
       my_script = config.require_flow_setting("MyNewFlow", "my_script")
       # ...
   ```

4. Update migration script if needed (`TCL_PATTERNS` dict)

## File References

| File | Purpose |
|------|---------|
| `hlsfactory/design_config.py` | Core config classes and parsing |
| `hlsfactory/framework.py` | Design/DesignDataset integration |
| `hlsfactory/flow_vitis.py` | Vitis flow implementations |
| `hlsfactory/opt_dsl_frontend_v2.py` | OptDSL frontend implementation |
| `hlsfactory/scripts/generate_design_configs.py` | Migration CLI tool |
| `tests/config_tests/` | Test suite for config system |
