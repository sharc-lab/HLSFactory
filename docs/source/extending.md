# Extending HLSFactory

HLSFactory is meant to be a shared platform for HLS research. You can extend it in three ways:

| I want to... | Go to |
|--------------|-------|
| Run my own designs through existing flows | {ref}`extend-own-designs` |
| Add a design set to the built-in datasets | {ref}`extend-datasets` |
| Support a new tool or vendor | {ref}`extend-flows` |

Questions or want help integrating something? Email the maintainers or open an issue on the HLSFactory GitHub repository.

(extend-own-designs)=
## Running Your Own Designs

A design is a directory of sources plus an `hlsfactory.toml` manifest that lists the flows it supports and the files each flow needs. See [Design Configuration](framework/design_config) for the format and the {ref}`flow-reference` for each flow's settings.

The minimum for each vendor:

| Vendor | Files in the design directory | `hlsfactory.toml` flow entries |
|--------|-------------------------------|--------------------------------|
| AMD Xilinx (Vitis HLS) | `dataset_hls.tcl`, `dataset_hls_ip_export.tcl`, sources | `VitisHLSSynthFlow` (`synth_tcl`), `VitisHLSImplFlow` (`impl_tcl`) |
| Intel | `<design>.c` or `.cpp`, named after the design | none; `IntelHLSSynthFlow` and `IntelQuartusImplFlow` don't read config settings |
| Altera | sources | `AlteraHLSSynthFlow` (see the {ref}`flow-reference` for settings) |
| Siemens Catapult | sources, `synth.tcl` | `CatapultHLSSynthFlow` (`synth_tcl`) |
| Google XLS | `.x` DSLX file | `XLSHLSSynthFlow` (`dslx_file`, `top`) |
| OptDSL (design-space exploration) | `opt_template.tcl` | `OptDSLv2` (`opt_dsl_file`) |

Intel and Altera device and clock options are flow constructor arguments, not config settings. Tool installation is covered in [Setup](setup).

A minimal Vitis HLS design looks like this:

```text
my_design/
    hlsfactory.toml
    dataset_hls.tcl              # create project, add sources, run csynth
    dataset_hls_ip_export.tcl    # open solution, export_design -flow impl
    src/
        kernel.cpp
        kernel.h
```

```toml
design_name = "my_design"
dataset_name = "my_dataset"

[[flow_configs]]
flow_name = "VitisHLSSynthFlow"
synth_tcl = "dataset_hls.tcl"

[[flow_configs]]
flow_name = "VitisHLSImplFlow"
impl_tcl = "dataset_hls_ip_export.tcl"
```

Base the Tcl scripts on a built-in design such as `hlsfactory/hls_dataset_sources/polybench/atax/`. Then load the design with `Design.from_dir_with_config()` or `DesignDataset.from_dir()` and pass it to a flow. See [Loading Custom Designs](tutorials/custom_designs).

For the other vendors, the matching tutorials show a complete working design: [Catapult](tutorials/catapult_flow), [Google XLS](tutorials/xls_flow), [Intel](tutorials/intel_flow) and [Altera](tutorials/altera_flow).

### Design-space exploration

To enumerate pragma variants automatically, add an `opt_template.tcl` and list the `OptDSLv2` flow. See the [OptDSL guide](framework/optdsl) for the syntax. It is optional; skip it for designs that are already separate variants or are vendor-specific.

(extend-datasets)=
## Contributing a Built-In Dataset

Built-in datasets ship with the package so everyone can load them by name.

1. **Prepare the designs.** One directory per design, each with its sources, scripts and `hlsfactory.toml`. Group them in one dataset directory, even for a single design.
2. **Add them to the repo.** Clone `sharc-lab/hlsfactory` and copy the dataset directory into `hlsfactory/hls_dataset_sources/`. Everything in it is packaged, so leave out large or unneeded files.
3. **Register the dataset** in `hlsfactory/datasets_builtin.py`: a path constant, a builder function, and an entry in `DATASET_STR_MAP`. Use one consistent key, with no aliases.

   ```python
   DIR_DATASET_MY_DATASET = HLS_DATASET_DIR / "my_dataset"


   def dataset_my_dataset_builder(name: str, work_dir: Path) -> DesignDataset:
       check_dataset_dir_exists(DIR_DATASET_MY_DATASET)
       new_dir = work_dir / name
       shutil.copytree(DIR_DATASET_MY_DATASET, new_dir)
       return DesignDataset.from_dir(name, new_dir)


   DATASET_STR_MAP = {
       ...,
       "my_dataset": dataset_my_dataset_builder,
   }
   ```

4. **Document it** in `docs/source/built_in_datasets.md`: add a row to the table and a section matching the others (design count, flow coverage, OptDSL templates, upstream links).
5. **Open a pull request.** The maintainers will review and discuss any changes.

(extend-flows)=
## Contributing a Flow

HLSFactory ships flows for AMD Xilinx, Intel, Altera, Siemens Catapult, Cadence Stratus and Google XLS, plus the OptDSL frontend. To support another tool, subclass `ToolFlow` (or `Frontend` for a design-generating step), which are both `Flow` subclasses.

A flow implements `execute`. It runs the tool in the design directory, writes its results next to the sources, and returns `[design]` on success or `[]` on failure or timeout:

```python
from hlsfactory.framework import Design, ToolFlow


class MyToolFlow(ToolFlow):
    name = "MyToolFlow"

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        config = design.require_config()
        script = config.require_flow_setting("MyToolFlow", "script_path")
        # run the tool in design.dir with `script`, passing `timeout` to the subprocess,
        # then write parsed results (for example data_hls.json) next to the sources
        return [design]
```

A design opts in with a matching `[[flow_configs]]` entry. Custom flows work without any registration. If you contribute the flow, also:

- add its name to `FlowName` and its required settings to `FlowConfig._REQUIRED_SETTINGS` in `hlsfactory/design_config.py`, so missing settings are caught when the config loads;
- add a row to the {ref}`flow-reference` and a vendor section to [Setup](setup);
- add a data aggregator in `hlsfactory/data_packaging.py` if its outputs should be packaged (see [Data](framework/data)).

Existing flows such as `flow_catapult.py` are good references for report parsing and error and timeout markers.
