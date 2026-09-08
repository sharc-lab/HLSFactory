# Altera HLS IP Gen flow

HLSFactory supports the 2026 Altera HLS IP Gen compiler (`ahls`) and its
SYCL designs. This is a separate toolchain from the legacy Intel HLS
`i++` flow.

## Installation

Install Altera HLS IP Gen using Altera's installation helper. Pass the
installation directory containing `bin/ahls-sh` to the flow constructor,
or export:

```bash
export HLSFACTORY_ALTERA_HLS_PATH=/path/to/altera/hls
```

The flow uses the installer's Apptainer environment for compilation and
execution. There is no default installation path.
Explicit `ahls_sh` or `ahls_bin` constructor arguments take precedence over
the installation directory and environment variable. Without an installation
override, the flow also looks for `ahls` and `ahls-sh` on PATH. If neither is
available, construction fails with instructions to configure the tool location.
For native `ahls` installations, initialize the vendor environment before
running Python. Container use requires permission to launch Apptainer.

## Flow stages

| Class | Compiler mode | Results |
| --- | --- | --- |
| `AlteraHLSSynthFlow` | `-Xshardware -fsycl-link=early` | RTL IP, optimization reports, estimated resources |
| `AlteraHLSCsimFlow` | `-Xsemulator`, then execute | Host testbench running on the FPGA emulator |
| `AlteraHLSCosimFlow` | `-Xssimulation`, then execute | Native host testbench driving cycle-accurate RTL in Questa |
| `AlteraQuartusImplFlow` | `-Xshardware -fsycl-link=image` | Hardware device image and Quartus fitter results |

Each stage compiles independently. Synthesis does not run place and route;
implementation requires the appropriate Quartus installation, device support
and license. C simulation checks the executable's exit code, so the host
testbench must return nonzero on incorrect results. It is software emulation,
not RTL simulation with Questa. Co-simulation requires a working Questa-Altera
or QuestaSim installation and the appropriate simulator license. Compilation
and execution use the same vendor environment.

## Built-in reference designs

`altera_examples` packages all 17 reference-design folders from
[Altera's ReferenceDesigns](https://github.com/altera-fpga/hls-samples/tree/0abae6d78af5daca3fe5d67e617ab037e58aff89/ReferenceDesigns).
Each design includes its sources, required headers, license and configuration.
Matrix multiplication uses upstream's 64×64 matrices with 8×8 tiles; merge
sort uses two merge units with sort width two. Both include numerical
checks against a software reference. The other 15 entries retain upstream's
default Agilex7 synthesis variants (with Gzip reduced to its documented
single-engine setting), including database query 1, Snappy
decompression and the Nios V sample's HLS DMA IP. The surrounding Nios V
processor system is outside this HLS flow. Runtime datasets and board projects
are not included. Every entry has synthesis and Quartus implementation
configuration; only `matmul` and `merge_sort` have simulation configurations.

Run HLS synthesis for all 17 designs from the repository root:

```bash
uv run python tests/dataset_validator.py \
  hlsfactory/hls_dataset_sources/altera_examples \
  --flow AlteraHLSSynthFlow --timeout 3600 \
  --work-dir /tmp/hlsfactory-altera-validation -j 4
```

The validator copies designs into its output directory before compilation.
It recreates that dataset's validation directory on each invocation and fails
if any design fails a stage. Omit `--work-dir` to use the existing
`HLSFACTORY_WORK_DIR` setting in `.env`.
Use `--designs matmul merge_sort --flow AlteraHLSSynthFlow+AlteraHLSCsimFlow`
to synthesize and emulate the two simulation-enabled entries.
Select `--flow AlteraQuartusImplFlow` to run the longer Quartus stage.
The default implementation timeout is 1800 seconds per design; use
`--timeout 7200`, for example, to allow two hours. The existing HLSFactory
tool runner pins each job to one CPU core, so fitting can take substantially
longer than RTL generation.

The dataset is also available through the Python API:

```python
from pathlib import Path
from hlsfactory.datasets_builtin import datasets_builder
from hlsfactory.flow_altera import AlteraHLSSynthFlow

work_dir = Path("/tmp/my_altera_experiment")
work_dir.mkdir(parents=True, exist_ok=True)
datasets = datasets_builder(work_dir, ["altera_examples"])
datasets = AlteraHLSSynthFlow().execute_multiple_design_datasets_fine_grained_parallel(
    datasets, False, n_jobs=4, timeout=3600,
)
```

## RTL co-simulation

Run the existing host tests against the generated RTL:

```bash
uv run python tests/dataset_validator.py \
  hlsfactory/hls_dataset_sources/altera_examples \
  --flow AlteraHLSCosimFlow \
  --designs matmul merge_sort \
  --work-dir /tmp/hlsfactory-altera-cosim-validation --timeout 3600 -j 2
```

Each of these two designs has a separate `AlteraHLSCosimFlow` TOML entry with `sources`,
`cxxflags`, `target`, optional `clock`/`fpga_flags`, and `run_args`.
The flow supplies `-DFPGA_SIMULATOR -Xssimulation`; the host code must select
`sycl::ext::altera::fpga_simulator_selector_v` when that macro is defined.
No preceding HLS synthesis or Quartus implementation run is required.
Compiling the simulator's FPGA libraries can take substantially longer than
RTL generation, especially with the tool runner's single-core affinity.
The validator allows one hour per design by default for this stage;
`--timeout` overrides that limit.

If the vendor environment selects an unlicensed simulator, pass
`questa_setup_script=Path("/absolute/path/questa_setup.sh")` to
`AlteraHLSCosimFlow`, or set `HLSFACTORY_ALTERA_QUESTA_SETUP` to that path.
This optional, trusted Bash script is sourced after the vendor environment
initializes and before compilation. Use it to export the intended simulator's
`bin` directory at the front of `PATH` and set its `SALT_LICENSE_SERVER`.
The same environment is used for testbench execution; the parent Python
environment is unchanged. The script must exist at the same absolute path
inside the container. Additional tool installations can be mounted read-only
using Apptainer's `APPTAINER_BINDPATH`; paths exported by the setup script
must refer to those container mounts. License values are not written into
the generated flow script.

For Siemens editions that generate `questa.ini` while Altera's packager
expects `modelsim.ini`, add `export HLSFACTORY_ALTERA_QUESTA_INI_COMPAT=1`
to the setup script. This opt-in wrapper creates a local `modelsim.ini`
alias only after a successful simulator command, preserving its library
mappings and exit status. Existing files are never replaced. It does not
modify the shared simulator installation.

The runtime executes inside its attempt directory and sets
`INTELFPGA_SIM_DEVICE_SPEC_DIR` to that attempt's `design.prj`, avoiding
ambiguous interface-file discovery. Testbench input-file paths must therefore
be absolute or relative to that attempt directory. The batch script clears
`DISPLAY` to prevent Qsys from connecting to a stale SSH display; it does not
change the parent Python environment.

Waveform capture is off by default. Add `waveform_depth = "1"` to log the
top-level signals, or `waveform_depth = "0"` to log all hierarchy levels
using Altera's `-Xsghdl` option. Full waveform capture can consume substantial
disk space and simulation time. The merge-sort entry enables depth 1 and uses
16 integers with two runs; matrix multiplication uses one matrix and one
repetition in its simulation-specific host path.

## Design configuration

Each desired stage needs its own entry in `hlsfactory.toml`:

```toml
design_name = "example"
dataset_name = "custom_altera"

[[flow_configs]]
flow_name = "AlteraHLSSynthFlow"
sources = "src/main.cpp"
cxxflags = "-Iinclude"
target = "Agilex7"
clock = "300MHz"
fpga_flags = ""

[[flow_configs]]
flow_name = "AlteraHLSCsimFlow"
sources = "src/main.cpp"
cxxflags = "-Iinclude"
run_args = "128"
```

All settings are strings. `sources` is required; it contains one or more
source filenames parsed with shell-style quoting. Paths are relative to the
design directory. `cxxflags`, `fpga_flags` and `run_args` also use shell-style
quoting but are passed as arguments, without shell expansion.
`target` defaults to `Agilex7` and accepts an Altera family or part number;
`clock` is optional and accepts the compiler's frequency or period syntax.
The flow supplies `FPGA_HARDWARE`, `FPGA_EMULATOR` or `FPGA_SIMULATOR`, the stage flags and the
output path. Use `cxxflags` for includes, defines and common compiler options;
use `fpga_flags` for additional hardware backend options.

## Artifacts and failures

Every attempt has a fresh directory under `altera_synth/`, `altera_csim/`,
`altera_cosim/` or `altera_impl/`, containing `run.sh` and `flow.log`. Old attempts remain
available for diagnosis. The timeout covers compilation and testbench
execution together.

Synthesis writes `data_hls.json` with estimated ALUT, FF, RAM, DSP and MLAB
usage/capacity, and `data_design.json` with compiler, target and kernel
information. The parser uses the compiler's NDJSON reports and selects the
resource table's Total and Available rows by name. Estimates are not
post-fit resource measurements.

`data_altera_synth.json` and `data_altera_impl.json` list relative paths to
the archive, project, HTML report and generated RTL. Implementation writes
`data_implementation.json` with the fitter's resource and clock tables.
`data_altera_csim.json` records successful testbench execution.
`data_altera_cosim.json` records successful RTL testbench execution and paths
to the simulation executable, interface XML, HTML report, simulator logs and
any captured `.wlf` waveforms. The generated report directory remains intact.
Its `statistics_files` list indexes the runtime-generated `sim_stats.ndjson`
and `simulation_raw.ndjson` reports when present; these are preserved raw,
not normalized into hardware-cycle latency metrics.
Both simulation stages determine success from the testbench's exit code.
Their elapsed time in `execution_data.json` includes compilation and execution;
it is not a measurement of kernel latency in hardware clock cycles.
Status and timing use the standard `execution_data.json`; failures return
an empty design list and write an error or timeout marker. Failed attempts
can be retried. Successful recorded stages are skipped when their result
manifest remains present.

## Server validation

All 17 built-in reference-design configurations passed standalone HLS synthesis
with HLS IP Gen 2026.1.0 on September 7, 2026. Gzip uses the upstream-supported
single-engine high-bandwidth variant and completed in approximately 600 seconds;
the other designs completed within 134 seconds each. This validation generated
RTL and resource estimates, without running Quartus implementation or simulation
for the 15 newly imported designs.

With HLS IP Gen 2026.1.0 and Quartus Pro 26.1, `matmul` and `merge_sort` passed
the synthesis-plus-emulation validator. Merge sort also completed Quartus
compilation; its generated fitter report was used to verify extraction of
fractional ALM counts and clock frequencies. Matrix multiplication completed
routing but reached the 1800-second timeout during fitter finalization.
Its full implementation result therefore remains unverified.

For RTL simulation, the server's bundled Questa Starter did not check out a
license. The standalone Siemens Questa 2026.1_2 installation did start with
the license setting from `/tools/software/siemens/setup.csh`. To select it,
save this Bash setup as an absolute path accessible inside the container:

```bash
export PATH="/siemens/questa/2026.1_2/questasim/bin:$PATH"
export SALT_LICENSE_SERVER="$(awk '/^setenv SALT_LICENSE_SERVER / {gsub(/"/, "", $3); print $3; exit}' /siemens/setup.csh)"
export HLSFACTORY_ALTERA_QUESTA_INI_COMPAT=1
```

Then set `HLSFACTORY_ALTERA_QUESTA_SETUP` to the saved script and
`APPTAINER_BINDPATH=/tools/software/siemens:/siemens:ro` before running the
co-simulation validator. This mounts the existing Siemens installation
read-only and does not modify either vendor installation.

These two reference designs passed RTL co-simulation with this simulator; merge
sort also produced `vsim.wlf`. The successful validation reused device
libraries compiled earlier by the same Questa/Quartus pair, while compiling
each design's RTL and running its host testbench in a fresh attempt. The
generated Quartus simulation scripts support this via
`QSYS_SIM_SCRIPT_QUESTASIM_OPTIONS_FILE`: point it to a Tcl file setting
`PRECOMP_DEVICE_LIB_FILE` to device-library mapping Tcl or `modelsim.ini`.
Do not reuse device libraries across different tool versions or targets.
Keep any referenced libraries available for subsequent simulation runs;
omit this optional override to compile them locally again.
