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

## Vendor Tools

To run specific vendor flows (e.g., Xilinx Vitis HLS and Vivado, Intel HLS and Quartus), you will need to have the respective tools installed on your machine. It is the user's responsibility to install and set up these tools as needed. Once set up, you can pass the path of these vendor tools to classes and functions in HLSFactory that require them.


HLSFactory also includes some helper functions that can try to automatically find the vendor tool paths on your machine if you have included them in your environment `PATH` variable.

See the [`hlsfactory.utils`](./apidocs/index) API documentation for details on `get_work_dir` and `get_tool_paths`, including examples of how the functions search the filesystem, `.env` files, and environment variables.

Additionally, HLSFactory supports auto-finding the vendor tool paths from a local `.env` file located in your project tree. Create a `.env` file with content similar to the following:

```text
HLSFACTORY_WORK_DIR=/absolute/path/to/your/workdir
HLSFACTORY_VITIS_HLS_PATH=/opt/Xilinx/Vitis_HLS/2023.1
HLSFACTORY_VIVADO_PATH=/opt/Xilinx/Vivado/2023.1
```

`get_work_dir(DirSource.ENVFILE)` and `get_tool_paths(ToolPathsSource.ENVFILE)` will read these keys automatically. You can also provide the same keys as environment variables if you prefer not to use a `.env` file.
