# Built-In HLS Design Datasets

HLSFactory ships with a collection of curated HLS design corpora that can be copied into your working directory through `hlsfactory.datasets_builtin.datasets_builder`. The table below gives a quick summary of what is currently packaged. Totals are derived from the sources in `hlsfactory/hls_dataset_sources/`.

| Key | Dataset | Designs | Flow Coverage | OptDSL Templates | Notes |
| --- | --- | --- | --- | --- | --- |
| `polybench` | PolyBench | 9 | Xilinx (Vitis HLS + Vivado) | All designs | Classic polyhedral kernels with rich pragma spaces. |
| `machsuite` | MachSuite | 18 | Xilinx (Vitis HLS + Vivado) | All designs | Diverse compute kernels representative of embedded workloads. |
| `chstone` | CHStone | 2 | Xilinx (Vitis HLS + Vivado) | All designs | Control-heavy benchmarks popular in HLS studies. |
| `pp4fpgas` | Parallel Programming for FPGAs | 5 | Xilinx (Vitis HLS + Vivado) | Not provided | Examples aligned with the PP4FPGAs textbook. |
| `vitis_examples` | Vitis Introductory Examples | 23 | Xilinx (Vitis HLS + Vivado) | Not provided | Direct import of AMD's getting-started designs. |
| `accelerators` | Sharc Lab Accelerators | 7 | Xilinx (Vitis HLS + Vivado) | Not provided | Custom accelerator kernels spanning FlowGNN, DGNN-Booster, MaskNet, SkyNet, and Edge-MoE. |
| `soda` | SODA | 11 | Xilinx (Vitis HLS + Vivado) | Not provided | Data-flow streaming designs from the SODA project. |
| `hp_fft` | HP-FFT | 16 | Xilinx (Vitis HLS + Vivado) | Not provided | Parameterized FFT accelerators with a range of unroll factors and design points. |
| `stream_hls` | Stream-HLS | 77 | Xilinx (Vitis HLS + Vivado) | Not provided | Transformer-inspired dataflow workloads and classic kernels from the Stream-HLS suite. |
| `auto_ntt` | AutoNTT | Varies | Xilinx (Vitis HLS + Vivado) | Not provided | Automated NTT generator with reference kernels and scripts for producing new design variants. |
| `forgebench` | ForgeBench | 45 | Xilinx (Vitis HLS + Vivado) | Not provided | Proposed machine-learning benchmark suite spanning GEMM kernels, DNN blocks, and transformer components for stressing next-gen flows. |

The keys in the first column are the values you pass to `datasets_builder`. Intel Quartus support is under active development; the datasets above currently ship only with Xilinx entry-point scripts.

## Using the Datasets

- **How to load** – `datasets_builder(work_dir, ["polybench", "machsuite"])` copies the selected sources into `work_dir` and returns a `DesignDatasetCollection` that you can feed into flows.
- **Structure** – Each design directory contains the original HLS sources plus flow entry points such as `dataset_hls.tcl` (for Vitis HLS) and, when available, `opt_template.tcl` (for OptDSL enumeration).
- **Licensing** – Datasets preserve the upstream licensing terms of their original repositories. Review the source links below if you plan to redistribute derivatives.

## Dataset Details

### PolyBench (`polybench`)

- **Upstream**: [Project page](https://web.cs.ucla.edu/~pouchet/software/polybench/)
- **Best for**: Evaluating pragma exploration; every kernel includes an OptDSL template and Vitis-compatible Tcl scripts.
- **Notes**: Designs are mirrored from the PolyBench/C suite and adapted for HLSFactory. The dataset offers both “abstract” designs (enumerated through OptDSL) and ready-to-run concrete variants.

### MachSuite (`machsuite`)

- **Upstream**: [Website](https://breagen.github.io/MachSuite/), [Repository](https://github.com/breagen/MachSuite), [Publication](https://ieeexplore.ieee.org/document/6983050)
- **Best for**: Memory- and compute-bound kernels spanning stencil, dense linear algebra, and streaming workloads.
- **Notes**: Most designs include OptDSL templates. The `common/` directory from the upstream repo is skipped when copying so that each design remains self-contained.

### CHStone (`chstone`)

- **Upstream**: [Paper](https://ieeexplore.ieee.org/document/4541637), [Mirror](https://github.com/ferrandi/CHStone)
- **Best for**: Control-oriented examples (JPEG, AES) that complement the more DSP-heavy suites.
- **Notes**: Small but useful for regression tests. OptDSL templates cover the full dataset.

### Parallel Programming for FPGAs (`pp4fpgas`)

- **Upstream**: [Book](https://arxiv.org/abs/1805.03648), [Repository](https://github.com/KastnerRG/pp4fpgas)
- **Best for**: Educational kernels aligned with the PP4FPGAs text.
- **Notes**: Designs ship as concrete implementations with Xilinx Tcl scripts; OptDSL scaffolding is not yet supplied.

### Vitis Introductory Examples (`vitis_examples`)

- **Upstream**: [AMD GitHub repository](https://github.com/Xilinx/Vitis-HLS-Introductory-Examples)
- **Best for**: Validating toolchain setups and demonstrating basic flow usage.
- **Notes**: Mirrors the official AMD/Xilinx introductory designs. No OptDSL coverage; each kernel is ready to synthesize as-is.

### Sharc Lab Accelerators (`accelerators`)

- **Upstream**:
  - [FlowGNN (HPCA 2023)](https://doi.org/10.1109/HPCA56546.2023.10071015)
  - [DGNN-Booster (FCCM 2023)](https://doi.org/10.1109/FCCM57271.2023.00029)
  - [Edge-MoE (ICCAD 2023)](https://doi.org/10.1109/ICCAD57390.2023.10323651)
  - [Mask-Net (ASAP 2022)](https://doi.org/10.1109/ASAP54787.2022.00030)
  - [SkyNet (MLSys 2020)](https://proceedings.mlsys.org/paper/2020/hash/2bb232c0b13c774965ef8558f0fbd615-Abstract.html)
- **Best for**: Exploring machine-learning oriented accelerator blocks.
- **Notes**: Each design includes the required Xilinx Tcl entry points; OptDSL and Intel scripts are not yet published.

### SODA (`soda`)

- **Upstream**: [SODA project](https://github.com/SODA-Project) (mirrored snapshot)
- **Best for**: Streaming-style applications and dataflow pipeline studies.
- **Notes**: Designs focus on concrete Xilinx flows with representative stencil and image-processing pipelines from the SODA benchmark. For more specialized FFT and NTT generators, see the HP-FFT, Stream-HLS, and AutoNTT datasets below.

### HP-FFT (`hp_fft`)

- **Upstream**: [HP-FFT (FCCM 2025)](https://doi.org/10.1109/FCCM62733.2025.00026)
- **Best for**: Evaluating high-throughput FFT architectures and unrolling strategies.
- **Notes**: Includes 16 parameterized designs across multiple transform sizes and unroll factors, each with ready-to-run Vitis HLS projects.

### Stream-HLS (`stream_hls`)

- **Upstream**: [Stream-HLS (FPGA 2025)](https://doi.org/10.1145/3706628.3708878)
- **Best for**: Studying transformer-style dataflow accelerators alongside adapted PolyBench kernels built with the Stream-HLS compiler.
- **Notes**: Contains 77 designs spanning neural network blocks, residual units, and dense linear algebra kernels synthesized for streaming dataflow execution.

### AutoNTT (`auto_ntt`)

- **Upstream**: [AutoNTT (FCCM 2025)](https://doi.org/10.1109/FCCM62733.2025.00024)
- **Best for**: Exploring automated generation of Number Theoretic Transform accelerators with configurable architecture templates.
- **Notes**: Ships with the AutoNTT generator, sample generated kernels, and supporting scripts so you can synthesize new design variants directly from the toolkit.

### ForgeBench (`forgebench`)

- **Upstream**: Internal Sharc Lab collection combining academic and production HLS kernels.
- **Best for**: Stress-testing flow robustness across a large, heterogeneous set of designs.
- **Notes**: Contains 45 standalone designs with Vitis Tcl entry points and targets next-generation HLS tooling with GEMM operators, deep neural network blocks, and transformer-ready pipelines. OptDSL templates and Intel support are planned.

## Roadmap

- Intel HLS and Quartus scripts are in progress for the datasets above, starting with PolyBench and MachSuite.
- Recent additions such as HP-FFT, Stream-HLS, and AutoNTT broaden the dataflow coverage, with further suites in the integration queue.
- Additional datasets (e.g., FlowGNN, Rosetta, expanded course collections) live under `hlsfactory/hls_dataset_sources/` but are not wired into `datasets_builder` yet. Follow the repository issues for availability updates.
