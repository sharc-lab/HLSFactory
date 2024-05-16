# Install and Setup

HLSFactory is a Python library. Therefore, it only requires Python and other Python packages to run. HLSFactory also includes the built-in HLS source code dataset alongside the library code, so you do not need to download any additional files to use the built-in datasets.

## Installation

HLSFactory can be installed as a regular Python package using `pip`, or as a conda package using `conda` or `mamba`.

### `pip` Install (via GitHub)

```bash
pip install git+https://github.com/sharc-lab/HLSFactory
```

### `conda` Install

```bash
conda install --channel https://sharc-lab.github.io/HLSFactory/dist-conda hlsfactory
```

### `mamba` Install

```bash
mamba install --channel https://sharc-lab.github.io/HLSFactory/dist-conda hlsfactory
```

The repository also includes demo scripts and Jupyter notebooks that demonstrate how to use the library. These are not included in the package but can be found in the source code repository on GitHub. You can access them by cloning the repository and following the instructions in the respective notebooks and scripts.

```{todo}
Link to the path in the source repo.
```

## Vendor Tools

To run specific vendor flows (e.g., Xilinx Vitis HLS and Vivado, Intel HLS and Quartus), you will need to have the respective tools installed on your machine. It is the user's responsibility to install and set up these tools as needed. Once set up, you can pass the path of these vendor tools to classes and functions in HLSFactory that require them.


HLSFactory also includes some helper functions that can try to automatically find the vendor tool paths on your machine if you have included them in your environment `PATH` variable.

```{todo}
Link to the helper functions that can automatically find the vendor tool paths.
```

Additionally, HLSFactory supports auto-finding the vendor tool paths from a local `.env` file.

```{todo}
Add more details for the `.env` file, and supported keys for different vendor tools.
```
