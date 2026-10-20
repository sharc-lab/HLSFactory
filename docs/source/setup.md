# Install and Setup

HLSFactory is a Python library. Therefore, it only requires Python 3.12 or newer and other Python packages to run. HLSFactory also includes the built-in HLS source code dataset alongside the library code, so you do not need to download any additional files to use the built-in datasets.

## Installation

We recommend managing your environment with [`uv`](https://docs.astral.sh/uv/). When working from a cloned repository, simply run:

```bash
uv sync
```

This creates a project-specific virtual environment and installs the package in editable mode along with the extras declared in `pyproject.toml`. Subsequent commands can be run through `uv run`, for example:

```bash
uv run pytest
uv run python demos/demo_full_flow_xilinx/full_flow_xilinx.py
```

If you prefer to install directly from GitHub without cloning, use:

```bash
uv pip install git+https://github.com/sharc-lab/HLSFactory
```

Or with plain pip:

```bash
pip install git+https://github.com/sharc-lab/HLSFactory
```

The repository also includes demo scripts and Jupyter notebooks that demonstrate how to use the library. These are not included in the package but can be found in the source code repository on GitHub. You can access them by cloning the repository and exploring the `demos/` directory (for example, `demos/demo_full_flow_xilinx/` for the complete Xilinx walkthrough and `demos/demo_full_flow_intel/` for the Intel variant).

## Quickstart (5-Minute Run)

To verify your setup and run a minimal end-to-end flow:

1. **Prerequisites:** Python (via `uv`), Vitis HLS and Vivado installed, and a `.env` file in the project root (or current directory) with:

   ```text
   HLSFACTORY_WORK_DIR=/absolute/path/to/your/workdir
   HLSFACTORY_VITIS_HLS_PATH=/path/to/Vitis_HLS/2023.1
   HLSFACTORY_VIVADO_PATH=/path/to/Vivado/2023.1
   ```

   Copy `.env.template` from the repository root and fill in the paths.

2. **Run the full Xilinx demo** (from the repository root):

   ```bash
   uv run python demos/demo_full_flow_xilinx/full_flow_xilinx.py
   ```

3. **Expected output:** After synthesis and implementation complete, each design directory under `$HLSFACTORY_WORK_DIR/demo_full_flow_xilinx/` will contain `data_hls.json`, `data_implementation.json`, and `execution_data.json` with latency, resource usage, timing, power, and flow-status data.

For a step-by-step walkthrough of each stage (dataset setup, OptDSL frontend, synthesis, implementation, reporting), see the [Xilinx Flow tutorial](tutorials/xilinx_flow).

## Vendor Tools

HLSFactory drives vendor tools; it does not install them. You need the tools for the flows you run, and it is your responsibility to install them and set up their licenses. Each section below says which tools a flow needs and how HLSFactory finds them. Flows you don't use need nothing.

| Vendor | Flows | Tools needed | How HLSFactory finds them |
|--------|-------|--------------|---------------------------|
| AMD Xilinx | `VitisHLS*Flow` | Vitis HLS, Vivado | `.env` / environment: `HLSFACTORY_VITIS_HLS_PATH`, `HLSFACTORY_VIVADO_PATH` |
| AMD Vitis (unified) | `VitisHLSModern*Flow` | `v++` from the merged Vitis toolchain | `v++` on `PATH` |
| Intel | `IntelHLSSynthFlow`, `IntelQuartusImplFlow` | `i++`, `quartus_sh` | `PATH` or constructor arguments |
| Altera | `AlteraHLS*Flow`, `AlteraQuartusImplFlow` | `ahls` or `ahls-sh` | `HLSFACTORY_ALTERA_HLS_PATH`, `PATH`, or constructor arguments |
| Siemens Catapult | `CatapultHLSSynthFlow` | `catapult` | `HLSFACTORY_CATAPULT_PATH`, then `PATH` |
| Cadence Stratus | `StratusHLSSynthFlow` | `stratus`, `bdw_makegen`, `make` | `stratus_install_dir` argument or `PATH` |
| Google XLS | `XLSHLSSynthFlow` | `ir_converter_main`, `opt_main`, `codegen_main` | `HLSFACTORY_XLS_PATH` |

Helpers in [`hlsfactory.utils`](apidocs/index) (`get_work_dir`, `get_tool_paths`) read a `.env` file or the process environment. Create a `.env` in your project tree (copy `.env.template` from the repository root); the keys each vendor uses are listed in its section.

### AMD Xilinx: Vitis HLS and Vivado

Used by `VitisHLSSynthFlow`, `VitisHLSImplFlow`, and the C/co-simulation flows. Tested with versions such as 2021.1 and 2023.1. Set the install roots in `.env` (or as environment variables):

```text
HLSFACTORY_WORK_DIR=/absolute/path/to/your/workdir
HLSFACTORY_VITIS_HLS_PATH=/opt/Xilinx/Vitis_HLS/2023.1
HLSFACTORY_VIVADO_PATH=/opt/Xilinx/Vivado/2023.1
```

Typical Linux locations are `/opt/Xilinx/Vitis_HLS/<version>` and `/opt/Xilinx/Vivado/<version>`. `get_work_dir(DirSource.ENVFILE)` and `get_tool_paths(ToolPathsSource.ENVFILE)` read these keys. A flow can also be given `vitis_hls_bin` directly, and it falls back to `vitis_hls` on `PATH`. See the [Xilinx tutorial](tutorials/xilinx_flow).

### AMD Vitis (unified toolchain)

Used by `VitisHLSModernSynthFlow` and `VitisHLSModernCsimFlow`, which run `v++` with a `hls_config.cfg` instead of a Tcl script. This is the merged Vitis toolchain (for example `2026.1/Vitis`), a separate install root from the classic Vitis HLS and Vivado pair above. Source the Vitis settings script so `v++` is on `PATH` (or pass `vpp_bin`). `.env.template` also defines `HLSFACTORY_VITIS_MODERN_PATH`, which `get_tool_path_vitis_modern` reads if you want the install root in your `.env`. Validate with the `test_designs_vitis_modern` dataset.

### Intel HLS Compiler and Quartus

Used by `IntelHLSSynthFlow` (`i++`) and `IntelQuartusImplFlow` (`quartus_sh`). Put both on `PATH` by sourcing the Intel HLS and Quartus environment scripts, or pass `ipp_bin` and `quartus_bin` to the flows. There is no HLSFactory-specific environment variable for these. The target device and clock are flow constructor arguments (`arch`, `clock`). See the [Intel tutorial](tutorials/intel_flow).

### Altera HLS IP Gen

Used by the `AlteraHLS*Flow` classes and `AlteraQuartusImplFlow`. This is the 2026 `ahls` SYCL compiler, separate from the legacy Intel `i++` flow. HLSFactory looks for the tool in this order: explicit `ahls_bin` or `ahls_sh` arguments, `altera_install_dir` or `HLSFACTORY_ALTERA_HLS_PATH`, then `ahls` / `ahls-sh` on `PATH`. There is no default install path.

```bash
export HLSFACTORY_ALTERA_HLS_PATH=/path/to/altera/hls   # directory containing bin/ahls-sh
```

The installer's `ahls-sh` launcher runs inside an Apptainer container, so container use needs permission to launch Apptainer. A native `ahls` install needs its vendor environment initialized before you start Python. See the [Altera tutorial](tutorials/altera_flow).

### Cadence Stratus

Used by `StratusHLSSynthFlow`. The flow needs `stratus`, `bdw_makegen`, and `make`. Pass `stratus_install_dir` (a directory containing `bin/stratus` and `bin/bdw_makegen`) to the flow, or put the tools on `PATH`. Source your site's Cadence environment script first so the license server settings are inherited. The validator accepts the same install directory:

```bash
uv run python tests/dataset_validator.py \
    hlsfactory/hls_dataset_sources/test_designs_stratus \
    --flow StratusHLSSynthFlow \
    --stratus-install-dir /path/to/stratus \
    -j 1
```

### Google XLS

`XLSHLSSynthFlow` reads `HLSFACTORY_XLS_PATH` directly from the environment. Set it to
your XLS installation directory. The directory may contain
release binaries at its root or a source build's binaries under `bazel-bin/xls/`.
The flow requires `ir_converter_main`, `opt_main`, and `codegen_main`; it checks
both layouts for each executable. An explicit `xls_install_dir` argument passed
to `XLSHLSSynthFlow` takes precedence over the environment variable.

```bash
export HLSFACTORY_XLS_PATH=/path/to/xls
```

If you keep `HLSFACTORY_XLS_PATH` in `.env`, export that file's values into the
process environment before running the flow; HLSFactory does not automatically
load this particular setting from `.env`.

Validate the installation against all packaged DSLX examples with:

```bash
uv run python tests/dataset_validator.py \
    hlsfactory/hls_dataset_sources/test_designs_xls \
    --flow XLSHLSSynthFlow \
    -j 4
```

See the [Google XLS tutorial](tutorials/xls_flow) for the Python API, design
configuration, generated RTL and report artifacts, and metric semantics.

### Siemens Catapult Setup

`CatapultHLSSynthFlow` first uses an explicit `catapult_bin` constructor
argument, then `HLSFACTORY_CATAPULT_PATH`, and finally searches `PATH`.
`HLSFACTORY_CATAPULT_PATH` may point directly to the executable, to the
`Mgc_home` installation root containing `bin/catapult`, or to its parent
containing `Mgc_home/bin/catapult`:

```bash
export HLSFACTORY_CATAPULT_PATH=/path/to/catapult/Mgc_home
```

The validator's `--catapult-bin` option provides the same explicit override.
Catapult must also inherit the vendor license environment and any
library-related variables required by the synthesis Tcl script.

Source the setup script supplied by your Siemens administrator (or your site's equivalent) before starting HLSFactory:

```bash
source /path/to/siemens/setup.sh   # or setup.csh from a C shell
```

Such a script typically configures `MGLS_LICENSE_FILE`, `SALT_LICENSE_SERVER`, `MGC_CATAPULT_HOME`, and `PATH`. Setting `HLSFACTORY_CATAPULT_PATH` selects the executable but does not replace this license and library setup.

If your setup script is written for C shell but you work in Bash, the complete built-in validation can be launched through C shell:

```bash
csh -c 'source /path/to/siemens/setup.csh; uv run python tests/dataset_validator.py hlsfactory/hls_dataset_sources/test_designs_catapult --flow CatapultHLSSynthFlow -j 1'
```

Make sure `catapult` is on `PATH` or `HLSFACTORY_CATAPULT_PATH` is set. See the [Catapult HLS tutorial](tutorials/catapult_flow) for the Python API, design configuration, reports, and output fields.
