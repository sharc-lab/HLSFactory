<div>
  <h1 align="center">HLSFactory 🏭</h1>
  <p align="center">A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond</p>
</div>

## Quick Start

Documentation & Tutorials: [https://sharc-lab.github.io/HLSFactory/docs/](https://sharc-lab.github.io/HLSFactory/docs/) (🚧 under construction)

`pip` Install (via GitHub):

```bash
pip install git+https://github.com/sharc-lab/HLSFactory
```

`conda` Install:

```bash
conda install --channel https://sharc-lab.github.io/HLSFactory/dist-conda hlsfactory
```

`mamba` Install:

```bash
mamba install --channel https://sharc-lab.github.io/HLSFactory/dist-conda hlsfactory
```

<hr>

Some of the documentation and packaging is still under construction. However, you can install the HLSFactory package via `pip` and get started running the demos, importing your own HLS designs into the HLSFactory framework, and using HLSFactory for your own research and experiments.

To be able to run these demos you can install the package using the following command:

```bash
pip install git+https://github.com/sharc-lab/HLSFactory
```

This will install the package and its dependencies. No other dependencies are required to run the demos.

You can then run the demos by following the instructions in the respective notebooks and scripts. We also include rendered PDFs for any Jupyter notebooks. 

### Full Flow Demo

However, you can still get started with a demo script and Jupyter notebook to run a full "Frontend Design Space Sampling-> Vitis HLS -> Vivado Implementation -> Output Data" flow on the built-in PolyBench, Machsuite, and CHStone datasets. The script and notebook for this demo are located in `demo/demo_full_flow/`.

### Your Custom Design Dataset Demo

We also provide a demo Jupyter notebook, with detailed instructions on how to get started importing your own HLS designs into the HLSFactory framework to run your own flows and collect data for your own experiments and research. The notebook for this demo is located in `demo/demo_custom_datasets/`.


## About

HLSFactory is a framework for High-Level Synthesis (HLS) design datasets. It provides both the facilities to collect and build custom HLS datasets using various frontends, supported HLS tools, and data aggregation as well as provides built-in design dataset sources for users who want to run their own experiments out of the box. We also provide easy facilities for new users to contribute their own HLS designs to the existing design datasets as well as augment the existing tool flows to support custom flows such as new front ends for design space sampling and new vendor tool support.

We currently include the following HLS design sources as built-in datasets:

- PolyBench: A polyhedral compilation benchmark suite adapted for HLS
- Machsuite: A HLS benchmark suite
- CHStoneL A HLS benchmark suite
- Rosetta (🚧 under construction): A HLS benchmark suite with kernels targeting machine learning, deep learning, and signal processing applications.
- Parallel Programming for FPGAs: Kernels from the PP4FPGA textbook
- Vitis HLS Examples: A collection of examples from the Vitis HLS tool
- FlowGNN: A collection of GNN accelerator designs
- DGNBooster + Gaussian Process + MaskNet + SkyNet: Selected accelerator kernels from Sharc Lab

We currenly support the following vendor tool flows:

- AMD/Xilinx Vitis HLS and Vivado
- Intel HLS Compiler and Quartus (🚧 under construction)

## Demos

We provide documented demos, python scripts and Jupyter notebooks, to get you started with the HLSFactory framework. These demos are located in the `demo/` directory.

## How to Cite

(🚧 under construction)

```bibtex
@article{hlsfactory,
  title={HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond},
  author={},
  journal={},
  year={},
  publisher={}
}
```

## License

This project is currently licensed under the GNU AGPLv3 license. We plan to  move to a more permissive license in the future, such as BSD-3-Clause or MIT.

## Contact

You can contact Stefan Abi-Karam as the corresponding author at [stefanabikaram@gatech.edu](mailto:stefanabikaram@gatech.edu) for any inquiries.

Please also feel free to submit GitHub issues for any bugs or feature requests.
