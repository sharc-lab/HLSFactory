# Install and Setup

HLSFactory is a Python library. Therefore, it only requires Python and other Python packages to run. HLSFactory also includes the built-in HLS source code dataset alongside the library code, so you do not need to download any additional files to use the built-in datasets.

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

Legacy alternatives remain available:

- **pip**:

  ```bash
  pip install git+https://github.com/sharc-lab/HLSFactory
  ```

- **conda**:

  ```bash
  conda install --channel https://sharc-lab.github.io/HLSFactory/dist-conda hlsfactory
  ```

- **mamba**:

  ```bash
  mamba install --channel https://sharc-lab.github.io/HLSFactory/dist-conda hlsfactory
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

3. **Expected output:** After synthesis and implementation complete, each design directory under `$HLSFACTORY_WORK_DIR/demo_full_flow_xilinx/` will contain `data_hls.json`, `data_implementation.json`, and `execution_time_data.json` with latency, resource usage, timing, and power data.

For a step-by-step walkthrough of each stage (dataset setup, OptDSL frontend, synthesis, implementation, reporting), see the [Xilinx Flow tutorial](tutorials/xilinx_flow).

## Vendor Tools

You need the appropriate vendor tools for the flows you run:

- **Xilinx flows** — Vitis HLS and Vivado (e.g., 2021.1, 2023.1)
- **Intel flows** — Intel HLS Compiler and Quartus

It is the user's responsibility to install and set up these tools. Once installed, HLSFactory can discover them via a `.env` file or environment variables.

HLSFactory includes helper functions to find tool paths from your `PATH`, `.env` file, or environment variables. See the [`hlsfactory.utils`](apidocs/index) API documentation for `get_work_dir` and `get_tool_paths`.

Create a `.env` file in your project tree (you can copy from `.env.template` in the repository root) with content like:

```text
HLSFACTORY_WORK_DIR=/absolute/path/to/your/workdir
HLSFACTORY_VITIS_HLS_PATH=/opt/Xilinx/Vitis_HLS/2023.1
HLSFACTORY_VIVADO_PATH=/opt/Xilinx/Vivado/2023.1
```

Typical installation paths: `/opt/Xilinx/Vitis_HLS/<version>` and `/opt/Xilinx/Vivado/<version>` on Linux. `get_work_dir(DirSource.ENVFILE)` and `get_tool_paths(ToolPathsSource.ENVFILE)` read these keys automatically. You can also set the same keys as environment variables instead of using a `.env` file.
