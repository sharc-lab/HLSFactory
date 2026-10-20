# Design Configuration (`hlsfactory.toml`)

Every design in an HLSFactory dataset is a directory of source files and scripts. The file `hlsfactory.toml` in that directory is the design's manifest: it tells HLSFactory what the design is called, which dataset it belongs to, and **which flows can be run on it and with which input files**.

This page explains what the file contains, how it is validated, how flows read it, and how to use it from Python. For what each flow does and which settings it accepts, see the {ref}`flow-reference` in [Flows](./flows.md).

## Why a config file exists

Different tools need different entry points: Vitis HLS wants a Tcl script, XLS wants a DSLX file and a top function, Catapult wants its own Tcl script, and so on. Without a manifest, each flow would have to guess file names or rely on conventions that differ between datasets.

With `hlsfactory.toml`:

- A flow never guesses. It asks the design's config for the setting it needs (for example, `synth_tcl`) and uses that file.
- A design opts in to a flow by listing it. If a flow isn't listed, the design doesn't support it.
- Mistakes surface when the file is loaded, not halfway through a multi-hour tool run: a missing required setting raises an error immediately.

Loading a dataset requires the file by default. `DesignDataset.from_dir` fails on any design directory without an `hlsfactory.toml` unless you pass `require_config=False`.

## File format

A config has a few top-level fields followed by one `[[flow_configs]]` table per flow:

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

### Top-level fields

| Field | Required | Type | Meaning |
|-------|----------|------|---------|
| `design_name` | Yes | string | Name of the design. Must be non-empty. |
| `dataset_name` | Yes | string | Name of the dataset the design belongs to. Must be non-empty. |
| `tags` | No | list of strings | Free-form labels such as `["linalg", "stencil"]`, for filtering and bookkeeping. |
| `env_vars` | No | table of strings | Environment variables associated with the design. They are stored and round-tripped by the config, but the built-in flows do not currently apply them to tool processes. |
| `flow_configs` | No | array of tables | The flows this design supports. Defaults to none. |

### Flow entries

Each `[[flow_configs]]` entry has one required key, `flow_name`, and any number of settings. Settings are plain key/value pairs next to `flow_name`; they can also be nested under a `flow_settings` table, and both forms are merged.

```toml
[[flow_configs]]
flow_name = "XLSHLSSynthFlow"
dslx_file = "adder.x"
top = "add"
```

Rules to keep in mind:

- **All setting values are strings**, including numbers and booleans (`pipeline_stages = "1"`, not `1`). A non-string value is rejected.
- **Each flow appears at most once** per design. Listing the same `flow_name` twice is an error.
- **Required settings are enforced per flow.** For example, `VitisHLSSynthFlow` requires `synth_tcl`, and `XLSHLSSynthFlow` requires `dslx_file` and `top`. The table of required settings lives in the {ref}`flow-reference`.
- **Unknown flow names are allowed.** A flow that isn't built in has no required settings, which lets you define custom flows without modifying HLSFactory (see "Custom flows and new flow names" below).
- **File paths are relative to the design directory**, because flows run with the design directory as their working directory.

## How a flow uses the config

A flow's `execute` method receives a `Design`. The design carries its parsed config, and the flow pulls out what it needs. This is the pattern used by the built-in Vitis flows:

```python
config = design.require_config()                       # error if no hlsfactory.toml was loaded
synth_tcl = config.require_flow_setting("VitisHLSSynthFlow", "synth_tcl")
# ... run vitis_hls on design.dir / synth_tcl ...
```

Two lookup styles cover most needs:

| Style | Behavior when missing | Use for |
|-------|----------------------|---------|
| `require_*` | Raises an error | Settings the flow cannot run without |
| `get_*` | Returns a default (`None` unless given) | Optional tuning settings |

Note that `get_flow_setting` still raises if the *flow itself* isn't configured for the design; the default only applies to a missing *setting* within a configured flow. Use `supports_flow` first if the flow is optional.

Copying a design (which flows do when `copy_dataset=True`) carries its config along, so a flow running on a copied dataset sees the same settings.

## Using the config from Python

### Read, inspect, and write

```python
from pathlib import Path
from hlsfactory.design_config import (
    DesignConfig,
    FlowConfig,
    FlowName,
    read_design_config,
    write_design_config,
)

config = read_design_config(Path("design_dir/hlsfactory.toml"))

config.design_name            # "k2mm"
config.flow_names             # ("VitisHLSSynthFlow", "VitisHLSImplFlow", ...)
config.supports_flow("VitisHLSSynthFlow")   # True

synth_tcl = config.require_flow_setting(FlowName.VITIS_HLS_SYNTH, "synth_tcl")
```

Creating a config programmatically and writing it back to TOML:

```python
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

Constructing a `FlowConfig` or `DesignConfig` runs the same validation as reading a file, so invalid configs fail at construction.

### Loading designs and datasets

```python
from hlsfactory.framework import Design, DesignDataset

design = Design.from_dir_with_config(Path("path/to/design"))   # requires hlsfactory.toml
design.config.design_name                                      # design.config is None if no config was loaded
design.require_config()                                        # raises if no config was loaded

dataset = DesignDataset.from_dir("polybench", Path("path/to/dataset"))   # configs required
legacy = DesignDataset.from_dir("old", Path("path/to/dataset"), require_config=False)
```

Use `require_config=False` only for directories that don't have manifests yet; flows that need settings will raise when they call `require_config()`.

### Custom flows and new flow names

A custom flow reads its own settings the same way built-in flows do. It does not need to be registered anywhere:

```python
from hlsfactory.framework import Design, ToolFlow

class MyCustomFlow(ToolFlow):
    name = "MyCustomFlow"

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        config = design.require_config()
        script = config.require_flow_setting("MyCustomFlow", "script_path")
        mode = config.get_flow_setting("MyCustomFlow", "mode", default="fast")
        # ... run the tool ...
        return [design]
```

with a matching entry in the design's `hlsfactory.toml`:

```toml
[[flow_configs]]
flow_name = "MyCustomFlow"
script_path = "run.sh"
```

To also get automatic validation of required settings for a new flow, contribute it to HLSFactory itself:

1. Add the name to the `FlowName` enum in `hlsfactory/design_config.py`.
2. Add its required setting names to `FlowConfig._REQUIRED_SETTINGS` in the same file.
3. Read the settings in the flow with `require_flow_setting`.

## Example design directory

```
polybench/k2mm/
├── hlsfactory.toml            # the manifest described on this page
├── dataset_hls.tcl            # referenced by synth_tcl
├── dataset_hls_ip_export.tcl  # referenced by impl_tcl
├── opt_template.tcl           # referenced by opt_dsl_file
└── src/
    ├── kernel.cpp
    └── kernel.h
```

The names of the Tcl files are a convention, not a requirement: whatever the config points at is what runs.

## Best practices

- Include an `hlsfactory.toml` in every design, and list only flows you have actually run. A listed flow is a promise that the design works with it.
- Keep script names consistent within a dataset so designs stay easy to compare and review.
- Call `supports_flow` before reading settings for an optional flow, and use `require_*` for anything the flow cannot run without.
- Use `tags` for properties you'll want to filter on later (domain, kernel type, source suite).
