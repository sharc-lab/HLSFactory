<div>
  <h1 align="center">HLSFactory 🏭</h1>
  <p align="center">A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond</p>
</div>

## Quick Start

Documentation & Tutorials: [https://sharc-lab.github.io/HLSFactory/](https://sharc-lab.github.io/HLSFactory/)

**Recommended (uv)** – in a cloned checkout:

```bash
uv sync            # create a virtual env and install dependencies from pyproject.toml
uv run python -c "import hlsfactory; print(hlsfactory.__version__)"
```

**Install from Git (uv pip)**:

```bash
uv pip install git+https://github.com/sharc-lab/HLSFactory
```

**Legacy pip**:

```bash
pip install git+https://github.com/sharc-lab/HLSFactory
```

<hr>

Some documentation and packaging are still under construction. However, you can install the HLSFactory package via `uv pip install git+https://github.com/sharc-lab/HLSFactory` (or the legacy `pip install` shown above) and get started running the demos, importing your own HLS designs into the HLSFactory framework, and using HLSFactory for your own research and experiments.

If you clone the repository, `uv sync` will install the package in editable mode along with the test and docs extras. You can then run the demos by following the instructions in the respective notebooks and scripts. We also include rendered PDFs for any Jupyter notebooks. 

### Full Flow Demo

However, you can still get started with a demo script and Jupyter notebook to run a full "Frontend Design Space Sampling-> Vitis HLS -> Vivado Implementation -> Output Data" flow on the built-in PolyBench, MachSuite, and CHStone datasets. The script and notebook for this demo are located in `demos/demo_full_flow_xilinx/`.

### Your Custom Design Dataset Demo

We also provide a demo Jupyter notebook, with detailed instructions on how to get started importing your own HLS designs into the HLSFactory framework to run your own flows and collect data for your own experiments and research. The notebook for this demo is located in `demos/demo_custom_datasets/`.

### Catapult HLS Smoke Test

The built-in `test_designs_catapult` dataset contains two small C++ kernels for validating a Siemens Catapult installation. On the HLSFactory server, run the validator from an environment that has sourced the Siemens setup:

```bash
export HLSFACTORY_CATAPULT_PATH=/tools/software/siemens/catapult/latest/Mgc_home
csh -c 'source /tools/software/siemens/setup.csh; uv run python tests/dataset_validator.py hlsfactory/hls_dataset_sources/test_designs_catapult --flow CatapultHLSSynthFlow -j 1'
```

Successful designs receive a `data_hls.json` containing latency, throughput, clock, critical-path timing, and detailed standard-cell area metrics. See the [Catapult HLS tutorial](docs/source/tutorials/catapult_flow.md) for Python API and custom-design examples.

### Google XLS Smoke Test

The built-in `test_designs_xls` dataset contains 11 DSLX designs covering
combinational and pipelined functions, arrays, loops, parametrics, structs,
pattern matching, stateful `proc`s, and procs that call pure helper functions.
Point `HLSFACTORY_XLS_PATH` at an XLS release bundle or Bazel build and run:

```bash
export HLSFACTORY_XLS_PATH=/usr/scratch/common/xls
uv run python tests/dataset_validator.py \
    hlsfactory/hls_dataset_sources/test_designs_xls \
    --flow XLSHLSSynthFlow \
    -j 4
```

Each successful design produces unoptimized and optimized XLS IR,
synthesizable Verilog, interface and module signatures, schedule and lowered IR,
source-line mappings, effective tool-option snapshots, pass-pipeline metrics,
block metrics, and `data_hls.json`. The JSON includes reported latency and
initiation interval, flop count, feedthrough information, XLS delay estimates,
operation/BOM counts, and paths to every report. Large compiler IR dumps and
pass profiles remain opt-in. See the
[Google XLS tutorial](docs/source/tutorials/xls_flow.md) for setup, Python API,
configuration, output files, and custom DSLX designs.


## About

HLSFactory is a framework for High-Level Synthesis (HLS) design datasets. It provides both the facilities to collect and build custom HLS datasets using various frontends, supported HLS tools, and data aggregation as well as provides built-in design dataset sources for users who want to run their own experiments out of the box. We also provide easy facilities for new users to contribute their own HLS designs to the existing design datasets as well as augment the existing tool flows to support custom flows such as new front ends for design space sampling and new vendor tool support.

We currently include the following HLS design sources as built-in datasets:

- PolyBench: A polyhedral compilation benchmark suite adapted for HLS
- MachSuite: An HLS benchmark suite
- CHStone: An HLS benchmark suite
- Rosetta (🚧 under construction): An HLS benchmark suite with kernels targeting machine learning, deep learning, and signal processing applications.
- Parallel Programming for FPGAs: Kernels from the PP4FPGA textbook
- Vitis HLS Examples: A collection of examples from the Vitis HLS tool
- Catapult Test Designs: Small vector-add and dot-product kernels for validating Siemens Catapult HLS
- XLS Test Designs: DSLX kernels covering functions, arrays, loops, parametrics, structs, pattern matching, and stateful procs
- FlowGNN: A collection of GNN accelerator designs
- DGNN Booster + Gaussian Process + MaskNet + SkyNet: Selected accelerator kernels from Sharc Lab

We currently support the following vendor tool flows:

- AMD/Xilinx Vitis HLS and Vivado
- Intel HLS Compiler and Quartus
- Siemens Catapult HLS
- Google XLS (DSLX to synthesizable Verilog)

## Demos

We provide documented demos, Python scripts, and Jupyter notebooks to get you started with the HLSFactory framework. These demos are located in the `demos/` directory.

## How to Cite

```bibtex
@inproceedings{hlsfactoryc,
  title = {{{HLSFactory}}: {{A Framework Empowering High-Level Synthesis Datasets}} for {{Machine Learning}} and {{Beyond}}},
  shorttitle = {{{HLSFactory}}},
  booktitle = {2024 {{ACM}}/{{IEEE}} 6th {{Symposium}} on {{Machine Learning}} for {{CAD}} ({{MLCAD}})},
  author = {{Abi-Karam}, Stefan and Sarkar, Rishov and Seigler, Allison and Lowe, Sean and Wei, Zhigang and Chen, Hanqiu and Rao, Nanditha and John, Lizy and Arora, Aman and Hao, Cong},
  year = {2024},
  month = sep,
  pages = {1--9},
  publisher = {IEEE},
  address = {Salt Lake City (Snowbird), UT, USA},
  doi = {10.1109/MLCAD62225.2024.10740213},
  urldate = {2025-08-21},
  copyright = {https://doi.org/10.15223/policy-029},
  isbn = {979-8-3503-6356-2}
}
```

## License

This project is currently licensed under the GNU AGPLv3 license. We plan to move to a more permissive license in the future, such as BSD-3-Clause or MIT.

## Contact

You can contact Stefan Abi-Karam as the corresponding author at [stefanabikaram@gatech.edu](mailto:stefanabikaram@gatech.edu) for any inquiries.

Please also feel free to submit GitHub issues for any bugs or feature requests.
