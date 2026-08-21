# Google XLS HLS Flow

This tutorial runs the built-in DSLX dataset through Google XLS, explains the
generated artifacts and metrics, and shows how to add a custom function or
stateful proc.

## Prerequisites

Install HLSFactory and an XLS build containing these executables:

- `ir_converter_main`
- `opt_main`
- `codegen_main`

Set `HLSFACTORY_XLS_PATH` to the installation root:

```bash
export HLSFACTORY_XLS_PATH=/path/to/xls
```

On the HLSFactory server, use:

```bash
export HLSFACTORY_XLS_PATH=/usr/scratch/common/xls
```

The flow accepts release bundles with executables at the installation root and
source builds with executables below `bazel-bin/xls/`. You can check a release
bundle directly with:

```bash
$HLSFACTORY_XLS_PATH/ir_converter_main --version
$HLSFACTORY_XLS_PATH/opt_main --version
$HLSFACTORY_XLS_PATH/codegen_main --version
```

Also set a writable work directory in `.env`:

```text
HLSFACTORY_WORK_DIR=/absolute/path/to/hlsfactory_work
```

`HLSFACTORY_XLS_PATH` is read from the process environment. If you also record it
in `.env` for convenience, export that file's values before starting Python;
HLSFactory does not currently load this particular setting from `.env`.

## Load the Built-In Dataset

The `test_designs_xls` dataset contains 11 small designs spanning arithmetic,
arrays, loops, parametric functions, structs, pattern matching, bit operations,
pipeline and combinational codegen, stateful procs, and a proc that calls pure
helper functions.

```python
from hlsfactory.datasets_builtin import dataset_test_designs_xls_builder
from hlsfactory.utils import get_work_dir, remove_and_make_new_dir_if_exists

work_dir = get_work_dir() / "xls_tutorial"
remove_and_make_new_dir_if_exists(work_dir)

dataset = dataset_test_designs_xls_builder(
    "test_designs_xls",
    work_dir,
)
datasets = {dataset.name: dataset}
```

The builder copies the packaged sources into the work directory. XLS and
HLSFactory outputs are written into those copies, leaving the packaged dataset
unchanged.

## Run Synthesis

Construct the flow from `HLSFACTORY_XLS_PATH` and synthesize the dataset:

```python
from hlsfactory.flow_xls import XLSHLSSynthFlow

flow = XLSHLSSynthFlow()
results = flow.execute_multiple_design_datasets_fine_grained_parallel(
    datasets,
    copy_dataset=False,
    n_jobs=4,
    timeout=12 * 60,
)
```

An explicit installation directory overrides the environment and server
default:

```python
flow = XLSHLSSynthFlow(xls_install_dir="/path/to/xls")
```

For each design, the flow performs three stages:

1. DSLX to XLS IR with `ir_converter_main`.
2. Standard IR optimization with `opt_main`.
3. Pipeline or combinational RTL generation with `codegen_main`.

## Validator Smoke Test

From the repository root, validate every packaged design with:

```bash
export HLSFACTORY_XLS_PATH=/usr/scratch/common/xls
uv run python tests/dataset_validator.py \
    hlsfactory/hls_dataset_sources/test_designs_xls \
    --flow XLSHLSSynthFlow \
    -j 4
```

The validator copies the dataset under
`$HLSFACTORY_WORK_DIR/dataset_validator/`, runs each design independently, and
fails if the flow produces no successful designs.

## Configuration

A feed-forward pipelined function can use:

```toml
design_name = "adder"
dataset_name = "my_xls_dataset"

[[flow_configs]]
flow_name = "XLSHLSSynthFlow"
dslx_file = "adder.x"
top = "add"
generator = "pipeline"
pipeline_stages = "1"
delay_model = "unit"
```

For combinational RTL, set `generator = "combinational"` and omit
`pipeline_stages`, `delay_model`, and `reset`.

Stateful procs use the same pipeline generator and normally require a reset:

```toml
[[flow_configs]]
flow_name = "XLSHLSSynthFlow"
dslx_file = "running_sum_proc.x"
top = "RunningSum"
generator = "pipeline"
pipeline_stages = "1"
delay_model = "unit"
reset = "rst"
```

## Generated Artifacts

For a DSLX file named `adder.x`, a successful run produces:

| File | Description |
|---|---|
| `adder.ir` | IR produced directly from DSLX |
| `adder.opt.ir` | Optimized IR passed to codegen |
| `adder.v` | Synthesizable Verilog/SystemVerilog |
| `adder.signature.textproto` | Module ports, channels, reset, pipeline latency, and II |
| `adder.block_metrics.textproto` | Flop count, estimated path delays, feedthrough status, and operation BOM |
| `adder.interface.pb` | Binary DSLX interface metadata passed into codegen |
| `adder.interface.textproto` | Human-readable DSLX interface metadata |
| `adder.ir_converter_options.textproto` | Effective converter configuration |
| `adder.optimizer_options.textproto` | Effective optimizer configuration |
| `adder.scheduling_options.textproto` | Effective scheduling configuration |
| `adder.codegen_options.textproto` | Effective codegen configuration |
| `adder.schedule.textproto` | Pipeline stages, timed nodes, and minimum estimated period; pipeline designs only |
| `adder.scheduled.ir` | Post-scheduling IR |
| `adder.block.ir` | Lowered block IR with ports and registers |
| `adder.verilog_line_map.textproto` | DSLX-to-Verilog source-line mapping |
| `adder.optimization_pass_metrics.textproto` | Optimizer pass timing and transformations |
| `adder.scheduling_pass_metrics.textproto` | Scheduling pass timing and transformations |
| `adder.codegen_pass_metrics.textproto` | Codegen pass timing and transformations |
| `data_hls.json` | Parsed settings, artifact names, and synthesis metrics |
| `execution_time_data.json` | Runtime and CPU information when logging is enabled |

Failures create `error__XLSHLSSynthFlow.txt`; timeouts create
`timeout__XLSHLSSynthFlow.txt`. Failed designs return no output design from the
flow.

## Output Data

`data_hls.json` contains:

| Metric group | Fields |
|---|---|
| Design and configuration | `dslx_file`, `top`, `generator`, `pipeline_stages`, `delay_model`, `reset` |
| Schedule | `latency_cycles`, `initiation_interval` |
| Structure | `flop_count`, `feedthrough_path_exists` |
| Estimated timing | `metrics_delay_model`, `max_reg_to_reg_delay_ps`, `max_input_to_reg_delay_ps`, `max_reg_to_output_delay_ps`, `max_feedthrough_path_delay_ps` |
| Operations | `operation_counts`, `operation_kind_counts` |
| Artifacts | Paths for IR, RTL, interfaces, signature, schedule, line map, option snapshots, pass metrics, block metrics, and enabled debug outputs |

Read selected results with standard Python:

```python
import json

for design in results["test_designs_xls"].designs:
    data = json.loads((design.dir / "data_hls.json").read_text())
    print(
        design.name,
        data["latency_cycles"],
        data["initiation_interval"],
        data["flop_count"],
    )
```

Pipeline latency and II are parsed from the generated module signature rather
than inferred from `pipeline_stages`. XLS may add input and output registers, so
these values can differ. Combinational modules have no cycle-based signature;
their latency and II fields are therefore `null`.

Block-metrics delays are XLS estimates. With the default `unit` model, they are
not technology-specific timing. Operation/BOM counts and flop counts describe
the generated block, but they are not FPGA resource utilization or
standard-cell area. Physical area and post-route timing require a separate
implementation flow with a target device or cell library.

## Optional Debug Artifacts

Interface metadata, effective option snapshots, scheduled and block IR,
source-line maps, and optimization/scheduling/codegen pass metrics are generated
by default. Pipeline designs also receive a schedule textproto; combinational
designs record `schedule_file` as `null`.

The much larger compiler dumps and profiles remain opt-in:

```toml
[[flow_configs]]
flow_name = "XLSHLSSynthFlow"
dslx_file = "adder.x"
top = "add"
generator = "pipeline"
pipeline_stages = "1"
delay_model = "unit"
dump_optimizer_ir = "true"
dump_codegen_ir = "true"
profile_passes = "true"
```

`dump_optimizer_ir` creates `<source-stem>.optimizer_ir_dumps/`, while
`dump_codegen_ir` creates `<source-stem>.codegen_ir_dumps/`. `profile_passes`
creates separate optimizer and codegen `.pprof` files. A trivial adder can
produce hundreds of optimizer snapshots, so these settings are intended for
compiler debugging rather than routine dataset generation.

See the official [codegen options](https://google.github.io/xls/codegen_options/),
[scheduling](https://google.github.io/xls/scheduling/), and
[delay estimation](https://google.github.io/xls/delay_estimation/)
documentation for the meaning of these reports and scheduling controls.

## Add a Custom Design

Create one directory containing the DSLX source and configuration:

```text
my_design/
    kernel.x
    hlsfactory.toml
```

Load it directly with `Design.from_dir_with_config()` or place several such
directories under a dataset root and use `DesignDataset.from_dir()`. No Tcl
entry point is required. Use the packaged
[`adder`](https://github.com/sharc-lab/HLSFactory/tree/main/hlsfactory/hls_dataset_sources/test_designs_xls/adder),
[`running_sum_proc`](https://github.com/sharc-lab/HLSFactory/tree/main/hlsfactory/hls_dataset_sources/test_designs_xls/running_sum_proc),
and
[`packet_checksum_proc`](https://github.com/sharc-lab/HLSFactory/tree/main/hlsfactory/hls_dataset_sources/test_designs_xls/packet_checksum_proc)
directories as function, proc, and mixed proc/function examples.
