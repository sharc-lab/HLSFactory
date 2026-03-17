# HLS Design Collection

The HLS Design Collection is a community-organized effort to build the largest collection of HLS designs available in one place. It is a major effort within HLSFactory, bringing together curated HLS design corpora from across the research community. Designs can be copied into your working directory through `hlsfactory.datasets_builtin.datasets_builder`. The table below gives a quick summary of what is currently packaged. Totals are derived from the sources in `hlsfactory/hls_dataset_sources/`.


**Getting started:** For a first run, use `polybench` (small, OptDSL-ready) or `vitis_examples` (validates your toolchain). For ML/QoR work, `polybench` + `machsuite` + `chstone` are commonly used together.

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

## Contributing Your HLS Designs

HLSFactory welcomes open-source HLS designs from the community. If you have designs you would like to add to the collection, or need help integrating them, please reach out—we are more than happy to assist.

## Dataset Details

### PolyBench (`polybench`)

- **Upstream**: [Project page](https://web.cs.ucla.edu/~pouchet/software/polybench/)
- **Best for**: Evaluating pragma exploration; every kernel includes an OptDSL template and Vitis-compatible Tcl scripts.
- **Notes**: Designs are mirrored from the PolyBench/C suite and adapted for HLSFactory. The dataset offers both “abstract” designs (enumerated through OptDSL) and ready-to-run concrete variants.

**Citation:**
```
[n. d.]. PolyBench. https://web.cse.ohio-state.edu/~pouchet.2/software/polybench/
```

### MachSuite (`machsuite`)

- **Upstream**: [Website](https://breagen.github.io/MachSuite/), [Repository](https://github.com/breagen/MachSuite), [Publication](https://ieeexplore.ieee.org/document/6983050)
- **Best for**: Memory- and compute-bound kernels spanning stencil, dense linear algebra, and streaming workloads.
- **Notes**: Most designs include OptDSL templates. The `common/` directory from the upstream repo is skipped when copying so that each design remains self-contained.

**Citation:**
```
Brandon Reagen, Robert Adolf, Yakun Sophia Shao, Gu-Yeon Wei, and David Brooks. [n. d.]. MachSuite: Benchmarks for Accelerator Design and Customized Architectures. In 2014 IEEE International Symposium on Workload Characterization (IISWC) (2014-10). 110–119. https://doi.org/10.1109/IISWC.2014.6983050
```

### CHStone (`chstone`)

- **Upstream**: [Paper](https://ieeexplore.ieee.org/document/4541637), [Mirror](https://github.com/ferrandi/CHStone)
- **Best for**: Control-oriented examples (JPEG, AES) that complement the more DSP-heavy suites.
- **Notes**: Small but useful for regression tests. OptDSL templates cover the full dataset.

**Citation:**
```
Yuko Hara, Hiroyuki Tomiyama, Shinya Honda, Hiroaki Takada, and Katsuya Ishii. [n. d.]. CHStone: A Benchmark Program Suite for Practical C-based High-Level Synthesis. In 2008 IEEE International Symposium on Circuits and Systems (ISCAS) (2008-05). 1192–1195. https://doi.org/10.1109/ISCAS.2008.4541637
```

### Parallel Programming for FPGAs (`pp4fpgas`)

- **Upstream**: [Book](https://arxiv.org/abs/1805.03648), [Repository](https://github.com/KastnerRG/pp4fpgas)
- **Best for**: Educational kernels aligned with the PP4FPGAs text.
- **Notes**: Designs ship as concrete implementations with Xilinx Tcl scripts; OptDSL scaffolding is not yet supplied.

**Citation:**
```
Ryan Kastner, Janarbek Matai, and Stephen Neuendorffer. 2018. Parallel Programming for FPGAs. https://doi.org/10.48550/arXiv.1805.03648 arXiv:1805.03648 [cs]
```

### Vitis Introductory Examples (`vitis_examples`)

- **Upstream**: [AMD GitHub repository](https://github.com/Xilinx/Vitis-HLS-Introductory-Examples)
- **Best for**: Validating toolchain setups and demonstrating basic flow usage.
- **Notes**: Mirrors the official AMD/Xilinx introductory designs. No OptDSL coverage; each kernel is ready to synthesize as-is.

**Citation:**
```
Xilinx [n. d.]. Vitis-HLS-Introductory-Examples. https://github.com/Xilinx/Vitis-HLS-Introductory-Examples
```

### Sharc Lab Accelerators (`accelerators`)

- **Upstream**:
  - [FlowGNN (HPCA 2023)](https://doi.org/10.1109/HPCA56546.2023.10071015)
  - [DGNN-Booster (FCCM 2023)](https://doi.org/10.1109/FCCM57271.2023.00029)
  - [Edge-MoE (ICCAD 2023)](https://doi.org/10.1109/ICCAD57390.2023.10323651)
  - [Mask-Net (ASAP 2022)](https://doi.org/10.1109/ASAP54787.2022.00030)
  - [SkyNet (MLSys 2020)](https://proceedings.mlsys.org/paper/2020/hash/2bb232c0b13c774965ef8558f0fbd615-Abstract.html)
- **Best for**: Exploring machine-learning oriented accelerator blocks.
- **Notes**: Each design includes the required Xilinx Tcl entry points; OptDSL and Intel scripts are not yet published.

**Citation:**
```
Rishov Sarkar, Stefan Abi-Karam, Yuqi He, Lakshmi Sathidevi, and Cong Hao. 2023. FlowGNN: A Dataflow Architecture for Real-Time Workload-Agnostic Graph Neural Network Inference. In 2023 IEEE International Symposium on High-Performance Computer Architecture (HPCA). IEEE, Montreal, QC, Canada, 1099–1112. https://doi.org/10.1109/HPCA56546.2023.10071015
Hanqiu Chen and Cong Hao. 2023. DGNN-booster: A Generic FPGA Accelerator Framework for Dynamic Graph Neural Network Inference. In 2023 IEEE 31st Annual International Symposium on Field-Programmable Custom Computing Machines (FCCM). IEEE, Marina Del Rey, CA, USA, 195–201. https://doi.org/10.1109/FCCM57271.2023.00029
Rishov Sarkar, Hanxue Liang, Zhiwen Fan, Zhangyang Wang, and Cong Hao. 2023. Edge-MoE: Memory-Efficient Multi-Task Vision Transformer Architecture with Task-Level Sparsity via Mixture-of-Experts. In 2023 IEEE/ACM International Conference on Computer Aided Design (ICCAD). IEEE, San Francisco, CA, USA, 01–09. https://doi.org/10.1109/ICCAD57390.2023.10323651
Xiaofan Zhang, Haoming Lu, Cong Hao, Jiachen Li, Bowen Cheng, Yuhong Li, Kyle Rupnow, Jinjun Xiong, Thomas Huang, Honghui Shi, Wen-Mei Hwu, and Deming Chen. 2020. SkyNet: A Hardware-Efficient Method for Object Detection and Tracking on Embedded Systems. Proceedings of Machine Learning and Systems 2 (March 2020), 216–229.
```

### SODA (`soda`)

- **Upstream**: [SODA project](https://github.com/SODA-Project) (mirrored snapshot)
- **Best for**: Streaming-style applications and dataflow pipeline studies.
- **Notes**: Designs focus on concrete Xilinx flows with representative stencil and image-processing pipelines from the SODA benchmark. For more specialized FFT and NTT generators, see the HP-FFT, Stream-HLS, and AutoNTT datasets below.

**Citation:**
```
Yuze Chi, Jason Cong, Peng Wei, and Peipei Zhou. 2018. SODA: Stencil with Optimized Dataflow Architecture. In Proceedings of the International Conference on Computer-Aided Design. ACM, San Diego California, 1–8. https://doi.org/10.1145/3240765.3240850
```

### HP-FFT (`hp_fft`)

- **Upstream**: [HP-FFT (FCCM 2025)](https://doi.org/10.1109/FCCM62733.2025.00026)
- **Best for**: Evaluating high-throughput FFT architectures and unrolling strategies.
- **Notes**: Includes 16 parameterized designs across multiple transform sizes and unroll factors, each with ready-to-run Vitis HLS projects.

**Citation:**
```
Chengyue Wang, Jiahao Zhang, Yingquan Wu, and Jason Cong. 2025. HP-FFT: A General High-Performance FFT Generator Using High-Level Synthesis. In 2025 IEEE 33rd Annual International Symposium on Field-Programmable Custom Computing Machines (FCCM). IEEE, Fayetteville, AR, USA, 19–23. https://doi.org/10.1109/FCCM62733.2025.00026
```

### Stream-HLS (`stream_hls`)

- **Upstream**: [Stream-HLS (FPGA 2025)](https://doi.org/10.1145/3706628.3708878)
- **Best for**: Studying transformer-style dataflow accelerators alongside adapted PolyBench kernels built with the Stream-HLS compiler.
- **Notes**: Contains 77 designs spanning neural network blocks, residual units, and dense linear algebra kernels synthesized for streaming dataflow execution.

**Citation:**
```
Suhail Basalama and Jason Cong. 2025. Stream-HLS: Towards Automatic Dataflow Acceleration. In Proceedings of the 2025 ACM/SIGDA International Symposium on Field Programmable Gate Arrays. ACM, Monterey CA USA, 103–114. https://doi.org/10.1145/3706628.3708878
```

### AutoNTT (`auto_ntt`)

- **Upstream**: [AutoNTT (FCCM 2025)](https://doi.org/10.1109/FCCM62733.2025.00024)
- **Best for**: Exploring automated generation of Number Theoretic Transform accelerators with configurable architecture templates.
- **Notes**: Ships with the AutoNTT generator, sample generated kernels, and supporting scripts so you can synthesize new design variants directly from the toolkit.

**Citation:**
```
Dilshan Kumarathunga, Qilin Hu, and Zhenman Fang. 2025. AutoNTT: Automatic Architecture Design and Exploration for Number Theoretic Transform Acceleration on FPGAs. In 2025 IEEE 33rd Annual International Symposium on Field-Programmable Custom Computing Machines (FCCM). IEEE, Fayetteville, AR, USA, 1–9. https://doi.org/10.1109/FCCM62733.2025.00024
```

### ForgeBench (`forgebench`)

- **Upstream**: Internal Sharc Lab collection combining academic and production HLS kernels.
- **Best for**: Stress-testing flow robustness across a large, heterogeneous set of designs.
- **Notes**: Contains 45 standalone designs with Vitis Tcl entry points and targets next-generation HLS tooling with GEMM operators, deep neural network blocks, and transformer-ready pipelines. OptDSL templates and Intel support are planned.

**Citation:**
```
Andy Wanna, Hanqiu Chen, and Cong Hao. 2025. ForgeBench: A Machine Learning Benchmark Suite and Auto-Generation Framework for Next-Generation HLS Tools. https://doi.org/10.48550/arXiv.2504.15185 arXiv:2504.15185 [cs]
```

## Roadmap

- Intel HLS and Quartus scripts are in progress for the datasets above, starting with PolyBench and MachSuite.
- Recent additions such as HP-FFT, Stream-HLS, and AutoNTT broaden the dataflow coverage, with further suites in the integration queue.
- Additional datasets (e.g., FlowGNN, Rosetta, expanded course collections) live under `hlsfactory/hls_dataset_sources/` but are not wired into `datasets_builder` yet. Follow the repository issues for availability updates.
