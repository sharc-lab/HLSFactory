# Siemens Catapult HLS Flow

This tutorial runs the built-in Catapult smoke-test dataset, explains the generated synthesis data, and shows the files required to add a custom Catapult design.

## Prerequisites

Install HLSFactory and make sure Siemens Catapult is licensed and available on `PATH`. Catapult worker processes inherit their environment from the Python process, so source the Siemens setup before starting HLSFactory.

On the HLSFactory server:

```csh
source /tools/software/siemens/setup.csh
catapult -version
```

The setup script defines the Catapult installation and license-server variables. If you normally use Bash, you can run the validator through C shell as shown in {ref}`catapult-validator-smoke-test` below.

Set a writable HLSFactory work directory in `.env`:

```text
HLSFACTORY_WORK_DIR=/absolute/path/to/hlsfactory_work
```

## Load the Built-In Dataset

The `test_designs_catapult` dataset contains two deliberately small kernels: `dot_product` and `vector_add`.

```python
from pathlib import Path

from hlsfactory.datasets_builtin import dataset_test_designs_catapult_builder
from hlsfactory.utils import get_work_dir, remove_and_make_new_dir_if_exists

work_dir = get_work_dir() / "catapult_tutorial"
remove_and_make_new_dir_if_exists(work_dir)

dataset = dataset_test_designs_catapult_builder(
    "test_designs_catapult",
    work_dir,
)
datasets = {dataset.name: dataset}
```

The builder copies the packaged sources into the work directory. Catapult project outputs and HLSFactory JSON files are written into these copies, leaving the packaged dataset sources unchanged.

## Run Synthesis

When `catapult` is on `PATH`, construct the flow without a binary argument:

```python
from hlsfactory.flow_catapult import CatapultHLSSynthFlow

flow = CatapultHLSSynthFlow()
results = flow.execute_multiple_design_datasets_fine_grained_parallel(
    datasets,
    copy_dataset=False,
    n_jobs=1,
    timeout=12 * 60,
)
```

You can also select an executable explicitly:

```python
flow = CatapultHLSSynthFlow(
    catapult_bin=(
        "/tools/software/siemens/catapult/latest/"
        "Mgc_home/bin/catapult"
    ),
)
```

Start with `n_jobs=1` if your license pool is limited. Each worker launches an independent Catapult process.

## Output Data

For each successful design, the flow finds Catapult's `rtl.rpt` and `cycle.rpt` and writes `data_hls.json` in the design root. The output includes:

| Metric group | Fields |
|---|---|
| Tool and clock | `tool_version`, `clock_signal`, `clock_edge`, `clock_period`, `clock_period_ns`, `clock_uncertainty_ns`, `clock_allocation_percent` |
| Schedule | `real_operation_count`, `latency_cycles`, `latency_seconds`, `throughput_cycles`, `throughput_seconds`, `reset_length_cycles`, `initiation_interval_cycles` |
| Timing | `critical_path_delay_ns`, `critical_path_slack_ns` |
| Aggregate area | `total_area`, `total_area_score`, `combinational_area`, `sequential_area`, `raw_area`, `total_register_area` |
| Area breakdown | `datapath_area`, `mux_area`, `functional_unit_area`, `logic_area`, `buffer_area`, `memory_area`, `rom_area`, `register_area`, `fsm_area`, `fsm_register_area`, `fsm_combinational_area` |

The built-in scripts use Nangate 45 nm standard-cell libraries. Consequently, their area values are Catapult ASIC area scores, not FPGA LUT, flip-flop, BRAM, or DSP counts.

Read the results with standard Python:

```python
import json

for design in results["test_designs_catapult"].designs:
    data = json.loads((design.dir / "data_hls.json").read_text())
    print(
        design.name,
        data["latency_cycles"],
        data["throughput_cycles"],
        data["total_area"],
    )
```

When execution-time logging is enabled (the default), `execution_time_data.json` records the runtime and CPU core. Failed and timed-out designs receive `error__CatapultHLSSynthFlow.txt` and `timeout__CatapultHLSSynthFlow.txt`, respectively, and are omitted from the returned dataset.

(catapult-validator-smoke-test)=
## Validator Smoke Test

From a C shell with the Siemens setup sourced:

```csh
source /tools/software/siemens/setup.csh
uv run python tests/dataset_validator.py \
    hlsfactory/hls_dataset_sources/test_designs_catapult \
    --flow CatapultHLSSynthFlow \
    --catapult-bin /tools/software/siemens/catapult/latest/Mgc_home/bin/catapult \
    -j 1
```

From Bash, run the same validation in a configured C-shell subprocess:

```bash
csh -c 'source /tools/software/siemens/setup.csh; uv run python tests/dataset_validator.py hlsfactory/hls_dataset_sources/test_designs_catapult --flow CatapultHLSSynthFlow -j 1'
```

The validator copies the dataset into `$HLSFACTORY_WORK_DIR/dataset_validator/`, synthesizes every design, and fails if no design completes successfully.

As a representative sanity check, the HLSFactory server produced the following values with the bundled Tcl scripts. Exact area and timing can vary with the Catapult version and installed libraries.

| Design | Latency (cycles) | Throughput (cycles) | Critical path (ns) | Total area |
|---|---:|---:|---:|---:|
| `dot_product` | 8 | 10 | 5.209135 | 2081.009 |
| `vector_add` | 15 | 18 | 2.971534 | 2602.620 |

## Add a Custom Design

Each design requires this basic structure:

```text
my_design/
    kernel.cpp
    synth.tcl
    hlsfactory.toml
```

Configure the flow in `hlsfactory.toml`:

```toml
design_name = "kernel"
dataset_name = "my_dataset"

[[flow_configs]]
flow_name = "CatapultHLSSynthFlow"
synth_tcl = "synth.tcl"
```

The Tcl script must create the Catapult project, add source files, set the top function, select technology libraries, configure the clock, and run through `go extract`. HLSFactory runs the script as:

```text
catapult -shell -file synth.tcl
```

See the built-in [`dot_product` design](https://github.com/sharc-lab/HLSFactory/tree/main/hlsfactory/hls_dataset_sources/test_designs_catapult/dot_product) for a complete minimal example, [Extending HLSFactory](../extending) for design-packaging guidance, and the [Catapult API reference](../apidocs/hlsfactory/hlsfactory.flow_catapult) for class and parser details.
