# Flows

In HLSFactory, different `Flow` classes are defined to perform certain build steps, run external vendor tools on a design, or process designs with custom logic. These classes encapsulate the logic for executing these steps. The abstract base class `Flow` defines the interface for all flows and outlines a common set of functions to execute flows in parallel across multiple designs and datasets.

Each subclass of the `Flow` class defines an abstract method called `execute` with the following function signature:

```python
class Flow(ABC):
    ...
    @abstractmethod
    def execute(self, design: Design, timeout: float | None = None) -> list[Design]: ...
```

This method contains the core logic for each `Flow` subclass. For instance, the `VitisHLSSynthFlow(ToolFlow)` includes logic in its `execute` method to call the Vitis HLS synthesis tool on the design and return the resulting design object with the HLS synthesis results extracted and stored alongside the design source on disk.

The function signature might be confusing. For example, why would a flow that synthesizes a single design return a list of designs? There are two possible outcomes: 1) the synthesis is successful, and the design is returned as a list with a single element: `[design]`, or 2) the synthesis fails or times out and returns an empty list: `[]`. Other flows, like frontend flows, can elaborate a single base design into multiple new designs. In this case, the list of designs returned by the `execute` method would contain all the new designs created from the base design: `[design_new_1, design_new_2, ...]`. This pattern of returning a list of designs is used for all flows and has additional benefits, such as allowing ergonomic parallelization and combining the results of multiple designs and datasets executed in parallel.

We also include an optional timeout parameter in the `execute` method to allow user-defined flows to check for a timeout when calling and executing external tools, which may be long-running. For example, when exploring different combinations of optimization directives, a certain combination might lead a design to have unreasonably long synthesis times, such as by unrolling a huge loop or completely partitioning a large array. In this case, the user can set a timeout for the flow to stop the synthesis and return no results if the flow takes longer than the timeout.

Users will rarely use the `execute` function directly to call the flow on a single design. Instead, users will more commonly use the following helper functions of the `Flow` class to run the flow on a single design, a list of designs, or a dataset of designs:

```python
def execute_multiple_designs(
    self,
    designs: list[Design],
    n_jobs: int = 1,
    cpu_affinity: list[int] | None = None,
    timeout: float | None = None,
) -> list[Design]: ...

def execute_multiple_design_datasets_naive_parallel(
    self,
    design_datasets: DesignDatasetCollection,
    copy_dataset: bool,
    new_dataset_name_fn: Callable[[str], str] | None = None,
    n_jobs: int = 1,
    cpu_affinity: list[int] | None = None,
    timeout: float | None = None,
) -> DesignDatasetCollection: ...

def execute_multiple_design_datasets_fine_grained_parallel(
    self,
    design_datasets: DesignDatasetCollection,
    copy_dataset: bool,
    new_dataset_name_fn: Callable[[str], str] | None = None,
    n_jobs: int = 1,
    cpu_affinity: list[int] | None = None,
    par_chunksize: int | None = 1,
    timeout: float | None = None,
) -> DesignDatasetCollection: ...
```

However, users typically call these functions using the default values for the optional parameters:

```python
output_designs = my_flow.execute_multiple_designs(my_designs, n_jobs=N_JOBS)
my_datasets_v0 = my_flow.execute_multiple_design_datasets_naive_parallel(my_design_datasets, True, n_jobs=N_JOBS)
my_datasets_v1 = my_flow.execute_multiple_design_datasets_fine_grained_parallel(my_design_datasets, True, n_jobs=N_JOBS)
```

We always recommend using the `execute_multiple_design_datasets_fine_grained_parallel` function over the `execute_multiple_design_datasets_naive_parallel` function when running flows on multiple datasets, as it doesn't block the execution of the flow for each dataset independently and results in faster overall execution time.

## Frontend Flows vs. Tool Flows

In HLSFactory, we distinguish between `Frontend(Flow)` and `ToolFlow(Flow)`. These classes don't contain any logic themselves (literally just `...`) but are used for better code organization in case of more complex code structures and flow features in the future.

However, there are fundamental functional differences in the logic that these two types of flows implement in their `execute` methods. Frontend flows generally take a single design and elaborate it into multiple new designs, while tool flows generally take a single design, run a tool on it, and return the same design but with the output of the tool stored alongside the design source.

## Frontend Flows and OptDSL

As mentioned, frontend flows are used to generate multiple enumerated designs from a single base design. This can be as simple as templating a design with different values or as complex as custom source-to-source transformation of the HLS designs. The most common use case for frontend flows is to expand the design space of a single design by generating design variations of a base design with different combinations of optimization directives and values. For example, we may take a base design with a loop and generate four new designs, each with a loop unroll pragma and different unroll factors for the loop. This way, we have four different designs we can synthesize, each resulting in a different implementation.

An OptDSL frontend entry points at the design's template (see [OptDSL](./optdsl.md) for the syntax):

```toml
[[flow_configs]]
flow_name = "OptDSLv2"
opt_dsl_file = "opt_template.tcl"
```

(flow-reference)=
## Flow Reference

Every flow that runs from `hlsfactory.toml` is selected with a `[[flow_configs]]` entry whose `flow_name` is one of the names below (see [Design Configuration](./design_config.md) for the file format). A flow raises a clear error if one of its required settings is missing.

| Flow Name | Required Setting | Description |
|-----------|-----------------|-------------|
| `CatapultHLSSynthFlow` | `synth_tcl` | TCL script for Catapult HLS synthesis |
| `StratusHLSSynthFlow` | `project_tcl`, `hls_module`, `hls_config` | Stratus project and module/configuration to synthesize |
| `XLSHLSSynthFlow` | `dslx_file`, `top` | DSLX source file and top-level function to synthesize |
| `VitisHLSSynthFlow` | `synth_tcl` | TCL script for HLS synthesis |
| `VitisHLSImplFlow` | `impl_tcl` | TCL script for IP export/implementation |
| `VitisHLSCsimFlow` | `csim_tcl` | TCL script for C simulation |
| `VitisHLSCosimSetupFlow` | `cosim_setup_tcl` | TCL script for co-simulation setup |
| `VitisHLSCosimFlow` | `cosim_tcl` | TCL script for co-simulation |
| `OptDSLv2` | `opt_dsl_file` | Optimization DSL template file |
| `LightningSimFlow` | `solution_dir_name` | Existing Vitis HLS solution directory to simulate |
| `VitisHLSModernSynthFlow` | `hls_config` | Vitis HLS configuration file (`hls_config.cfg`) for synthesis |
| `VitisHLSModernCsimFlow` | `hls_config` | Vitis HLS configuration file (`hls_config.cfg`) for C simulation |
| `VitisHLSImplReportFlow` | None | Extracts implementation reports from an existing Vitis HLS implementation |
| `AlteraHLSSynthFlow` | `sources` | Space-separated source files for Altera HLS IP Gen synthesis |
| `AlteraHLSCsimFlow` | `sources` | Space-separated source files for C simulation |
| `AlteraHLSCosimFlow` | `sources` | Space-separated source files for co-simulation |
| `AlteraQuartusImplFlow` | `sources` | Space-separated source files; runs Quartus implementation |
| `BalorGraphFlow` | `top`, `source_file` | Top function and source file for Balor graph extraction |
| `HARPGraphFlow` | `source_file`, `top` | Source file and top function used to build the HARP graph |

Settings are always strings. The sections below describe each vendor's flows and show example entries.

## Xilinx Flows

The Xilinx flows wrap Vitis HLS and Vivado. Each reads its Tcl entry point (or, for the modern flows, a `hls_config.cfg`) from the design's `hlsfactory.toml`:

```toml
[[flow_configs]]
flow_name = "VitisHLSSynthFlow"      # runs vitis_hls and parses reports into data_hls.json
synth_tcl = "dataset_hls.tcl"

[[flow_configs]]
flow_name = "VitisHLSImplFlow"       # export_design -flow impl (Vivado implementation)
impl_tcl = "dataset_hls_ip_export.tcl"

[[flow_configs]]
flow_name = "VitisHLSCsimFlow"       # C simulation
csim_tcl = "dataset_hls_csim.tcl"

[[flow_configs]]
flow_name = "VitisHLSCosimSetupFlow" # co-simulation setup, then VitisHLSCosimFlow
cosim_setup_tcl = "dataset_hls_cosim_setup.tcl"

[[flow_configs]]
flow_name = "VitisHLSCosimFlow"
cosim_tcl = "dataset_hls_cosim.tcl"

[[flow_configs]]
flow_name = "VitisHLSModernSynthFlow" # config-file based; also VitisHLSModernCsimFlow
hls_config = "hls_config.cfg"

[[flow_configs]]
flow_name = "LightningSimFlow"       # simulates an existing Vitis HLS solution
solution_dir_name = "hls_k2mm/solution1"
```

`VitisHLSImplReportFlow` has no settings; it extracts reports from a completed implementation run.

Tool locations are resolved from the `vitis_hls_bin` constructor argument, then the environment (`HLSFACTORY_VITIS_HLS_PATH`, `HLSFACTORY_VIVADO_PATH`), and finally `PATH`. See the [Xilinx tutorial](../tutorials/xilinx_flow) for a complete example.

## Siemens Catapult Flow

`CatapultHLSSynthFlow` runs the synthesis Tcl file declared by the design's `CatapultHLSSynthFlow` configuration:

```toml
[[flow_configs]]
flow_name = "CatapultHLSSynthFlow"
synth_tcl = "synth.tcl"
```

The flow invokes `catapult -shell -file synth.tcl` in the design directory. A successful Tcl script must advance the solution through scheduling/assembly and extraction so Catapult produces `cycle.rpt` and `rtl.rpt`. HLSFactory parses those reports into `data_hls.json`, including:

- Clock period and uncertainty
- Latency, throughput, reset length, and initiation interval in cycles
- Critical-path delay and slack
- Operation count
- Total, combinational, sequential, datapath, register, functional-unit, MUX, logic, memory, ROM, and FSM area metrics

The flow supports the same dataset-level parallel execution and per-design timeout interface as the other `ToolFlow` implementations. It creates `error__CatapultHLSSynthFlow.txt` or `timeout__CatapultHLSSynthFlow.txt` when a design fails, and records runtime and completion status in `execution_data.json` when execution-time logging is enabled.

The Catapult executable is resolved from an explicit `catapult_bin`, then
`HLSFACTORY_CATAPULT_PATH`, and finally `PATH`. The environment setting accepts
the executable or an installation directory containing `bin/catapult` or
`Mgc_home/bin/catapult`. Source the Siemens environment before starting Python
so worker processes inherit the license settings. See the
[Catapult HLS tutorial](../tutorials/catapult_flow) for a complete example.

## Google XLS Flow

`XLSHLSSynthFlow` converts a DSLX function or proc to XLS IR, optimizes the IR,
and lowers it to synthesizable Verilog. It invokes `ir_converter_main`,
`opt_main`, and `codegen_main` in the design directory, using the top and source
declared in `hlsfactory.toml`:

```toml
[[flow_configs]]
flow_name = "XLSHLSSynthFlow"
dslx_file = "adder.x"
top = "add"
generator = "pipeline"
pipeline_stages = "1"
delay_model = "unit"
```

**XLS settings:**

| Setting | Required | Default | Description |
|---|---|---|---|
| `dslx_file` | Yes | — | DSLX source file relative to the design directory |
| `top` | Yes | — | Top-level DSLX function or proc |
| `generator` | No | `pipeline` | `pipeline` or `combinational` |
| `pipeline_stages` | No | `1` | Positive stage count used by the pipeline generator |
| `delay_model` | No | `unit` | XLS scheduling delay model used by the pipeline generator |
| `reset` | No | unset | Reset port name for pipeline codegen; required by the packaged stateful proc examples |
| `dump_optimizer_ir` | No | `false` | Write an IR snapshot after every optimizer pass |
| `dump_codegen_ir` | No | `false` | Write an IR snapshot after every scheduling/codegen pass |
| `profile_passes` | No | `false` | Write optimizer and codegen pprof pass profiles |

`pipeline_stages` controls scheduler partitioning, but it is not necessarily the
same as externally visible latency because XLS may add input and output
registers. Read `latency_cycles` from the generated `data_hls.json` for the
module-signature value. Combinational signatures do not express cycle latency
or initiation interval, so those JSON fields are `null` for combinational
designs.

The three diagnostic switches are string-valued booleans because all flow
settings are strings. Accepted true values are `true`, `yes`, `on`, and `1`;
accepted false values are `false`, `no`, `off`, and `0`. IR dumps can create
hundreds of files per design and are disabled by default.

The flow supports pipeline and combinational generators, stateful procs with an
explicit reset, per-design timeouts, parallel dataset execution, and the normal
HLSFactory error/timeout markers. Successful runs retain both IR forms, RTL, the
DSLX interface, module signature, schedule, scheduled and block IR, source-line
map, effective option snapshots, pass-pipeline metrics, and XLS block metrics.
`data_hls.json` records the configured codegen settings along with signature
latency/II, flop and feedthrough data, estimated path delays, aggregated
operation/BOM counts, and all artifact paths. Optimizer/codegen IR dumps and
pprof pass profiles are available through opt-in flow settings.

XLS is resolved from an explicit constructor argument, then `HLSFACTORY_XLS_PATH`,
and finally a built-in fallback path (`DEFAULT_XLS_PATH` in `hlsfactory/flow_xls.py`, which you can edit for your site). Both release-root
and Bazel `bazel-bin/xls/` executable layouts are supported. See the
[Google XLS tutorial](../tutorials/xls_flow) for a complete dataset run and
custom-design example.

## Intel Flows

`IntelHLSSynthFlow` runs the Intel HLS Compiler (`i++`) with a configurable target device `arch` and `clock`, and `IntelQuartusImplFlow` runs Quartus (`quartus_sh`) for implementation. Binaries are resolved from the `ipp_bin` / `quartus_bin` constructor arguments or `PATH`. See the [Intel tutorial](../tutorials/intel_flow) for a complete example.

## Altera Flows

`AlteraHLSSynthFlow`, `AlteraHLSCsimFlow`, `AlteraHLSCosimFlow`, and `AlteraQuartusImplFlow` drive Altera HLS IP Gen and Quartus. Each reads the design's `sources` setting from `hlsfactory.toml`, with optional `target`, `clock`, `cxxflags`, and `run_args` settings. See the [Altera tutorial](../tutorials/altera_flow).

```toml
[[flow_configs]]
flow_name = "AlteraHLSSynthFlow"
sources = "src/matmul_demo.cpp"
target = "Agilex7"
clock = "540MHz"
```

## Cadence Stratus Flow

`StratusHLSSynthFlow` builds a Stratus project from the design's `project_tcl`, `hls_module`, and `hls_config` settings and records latency, area, and clock metadata:

```toml
[[flow_configs]]
flow_name = "StratusHLSSynthFlow"
project_tcl = "project.tcl"
hls_module = "fir"
hls_config = "BASIC"
# makefile = "Makefile"  # Optional; this is the default.
```
