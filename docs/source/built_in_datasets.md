# HLS Design Collection

The HLS Design Collection is a community-organized effort to build the largest collection of HLS designs available in one place. It is a major effort within HLSFactory, bringing together curated HLS design corpora from across the research community. Designs can be copied into your working directory through `hlsfactory.datasets_builtin.datasets_builder`. The table below gives a quick summary of what is currently packaged. Totals are derived from the sources in `hlsfactory/hls_dataset_sources/`.


**Getting started:** For a first run, use `polybench` (small, OptDSL-ready) or `vitis_examples` (validates your toolchain). For ML/QoR work, `polybench` + `machsuite` + `chstone` are commonly used together.

| Key | Dataset | Designs | Flow Coverage | OptDSL Templates |
| --- | --- | --- | --- | --- |
| `polybench` | PolyBench | 9 | Xilinx (Vitis HLS + Vivado, co-simulation setup) | All designs |
| `machsuite` | MachSuite | 18 | Xilinx (Vitis HLS + Vivado) | All designs |
| `chstone` | CHStone | 2 | Xilinx (Vitis HLS + Vivado, co-simulation setup) | All designs |
| `mibench` | MiBench | 21 | Xilinx (Vitis HLS, C simulation) | Not provided |
| `pp4fpgas` | Parallel Programming for FPGAs | 5 | Xilinx (Vitis HLS + Vivado) | Not provided |
| `vitis_examples` | Vitis Introductory Examples | 23 | Xilinx (Vitis HLS + Vivado, C simulation) | 1 design (fixed-point sqrt) |
| `test_designs_catapult` | Catapult Test Designs | 2 | Siemens Catapult HLS | Not provided |
| `test_designs_stratus` | Stratus Test Designs | 2 | Cadence Stratus HLS | Not provided |
| `test_designs_xls` | XLS Test Designs | 11 | Google XLS | Not provided |
| `test_designs_vitis_modern` | Vitis Modern Test Designs | 2 | Xilinx (Vitis HLS config-file flow) | Not provided |
| `xls_upstream` | XLS Upstream Designs | 25 | Google XLS | Not provided |
| `spector_hls` | SPECTOR | 10 | Xilinx (Vitis HLS, C simulation) | All designs |
| `altera_examples` | Altera Reference Designs | 17 | Altera HLS IP Gen + Quartus (2 designs also C/co-simulation) | Not provided |
| `accelerators` | Sharc Lab Accelerators | 7 | Xilinx (Vitis HLS) | Not provided |
| `soda` | SODA | 11 | Xilinx (Vitis HLS + Vivado, C simulation) | Not provided |
| `hp_fft` | HP-FFT | 16 | Xilinx (Vitis HLS + Vivado, C simulation) | Not provided |
| `stream_hls` | Stream-HLS | 77 | Xilinx (Vitis HLS, C/co-simulation, LightningSim) | Not provided |
| `auto_ntt` | AutoNTT | 16 | Xilinx (Vitis HLS, C simulation) | Not provided |
| `forgebench` | ForgeBench | 45 | Xilinx (Vitis HLS + Vivado) | Not provided |

The keys in the first column are the values you pass to `datasets_builder`.
Most production datasets currently target Xilinx; the small Catapult, Stratus,
and XLS datasets are intended for validating those toolchains.

## Using the Datasets

- **How to load** – `datasets_builder(work_dir, ["polybench", "machsuite"])` copies the selected sources into `work_dir` and returns a `DesignDatasetCollection` that you can feed into flows.
- **Structure** – Each design directory contains the original HLS sources, an `hlsfactory.toml` flow configuration, and tool entry points such as `dataset_hls.tcl` for Vitis HLS or `synth.tcl` for Catapult. Some datasets also include `opt_template.tcl` for OptDSL enumeration.
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

### MiBench (`mibench`)

- **Upstream**: [Website](http://vhosts.eecs.umich.edu/mibench/), [Publication](https://ieeexplore.ieee.org/document/990739)
- **Best for**: Embedded and DSP-style kernels (crypto, image processing, telecom codecs, graph and string algorithms) with self-checking testbenches.
- **Notes**: Kernels are extracted from the MiBench programs rather than the full applications. File I/O, `malloc`, recursion, and function pointers are removed and data is processed in fixed-size buffers so the top function is synthesizable. Each design runs Vitis HLS synthesis and C simulation. The original archives are kept alongside the designs, and each design retains the copyright and licence notices of its source. See the dataset `README` for the per-design adaptations.

**Citation:**
```
M. R. Guthaus, J. S. Ringenberg, D. Ernst, T. M. Austin, T. Mudge, and R. B. Brown. 2001. MiBench: A free, commercially representative embedded benchmark suite. In Proceedings of the Fourth Annual IEEE International Workshop on Workload Characterization (WWC-4). 3–14. https://doi.org/10.1109/WWC.2001.990739
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
- **Notes**: Mirrors the official AMD/Xilinx introductory designs. Only the fixed-point sqrt example includes an OptDSL template; every other kernel is ready to synthesize as-is.

**Citation:**
```
Xilinx [n. d.]. Vitis-HLS-Introductory-Examples. https://github.com/Xilinx/Vitis-HLS-Introductory-Examples
```

### Catapult Test Designs (`test_designs_catapult`)

- **Best for**: Quickly validating a Siemens Catapult installation and license setup.
- **Notes**: Contains standalone vector-add and dot-product kernels targeting the bundled Nangate 45 nm libraries. The resulting `data_hls.json` files contain latency, throughput, critical-path timing, and detailed standard-cell area breakdowns. These are ASIC-library area scores rather than FPGA LUT/FF/DSP counts. Source your Siemens setup script before running Catapult so the license-server environment is available.

```csh
source /path/to/siemens/setup.csh
setenv HLSFACTORY_CATAPULT_PATH /path/to/catapult/Mgc_home
uv run python tests/dataset_validator.py \
    hlsfactory/hls_dataset_sources/test_designs_catapult \
    --flow CatapultHLSSynthFlow \
    -j 1
```

### Cadence Stratus Test Designs (`test_designs_stratus`)

- **Best for**: Quickly validating a Cadence Stratus installation, license, and SystemC synthesis environment.
- **Notes**: Contains streaming increment and multiply-accumulate modules that use `cynw_p2p` channels. Both projects target the GPDK045 standard-cell library with a 5 ns clock. The flow generates RTL and records latency, sequential and combinational area, the Stratus version, and clock metadata in `data_hls.json` and `data_design.json`.

Point the validator at a Stratus installation containing `bin/stratus` and
`bin/bdw_makegen`:

```bash
uv run python tests/dataset_validator.py \
    hlsfactory/hls_dataset_sources/test_designs_stratus \
    --flow StratusHLSSynthFlow \
    --stratus-install-dir /path/to/stratus \
    -j 1
```

### Google XLS Test Designs (`test_designs_xls`)

- **Upstream**: [Google XLS documentation](https://google.github.io/xls/)
- **Best for**: Validating DSLX-to-Verilog setup and exercising pipeline and combinational code generation.
- **Notes**: Includes arithmetic and control kernels plus examples of arrays, `map`, counted `for` expressions, bit slicing and concatenation, parametric functions, pattern matching, struct-update syntax, stateful `proc`s communicating over channels, and a proc that calls pure tuple-processing helper functions. These designs follow the official [XLS tools quick start](https://google.github.io/xls/tools_quick_start/), [DSLX language reference](https://google.github.io/xls/dslx_reference/), and [`proc` tutorial](https://google.github.io/xls/tutorials/how_to_use_procs/).

Run all 11 designs through the same validator used for toolchain checks:

```bash
export HLSFACTORY_XLS_PATH=/path/to/xls
uv run python tests/dataset_validator.py \
    hlsfactory/hls_dataset_sources/test_designs_xls \
    --flow XLSHLSSynthFlow \
    -j 4
```

The validator copies the packaged sources into `$HLSFACTORY_WORK_DIR`, leaving
the originals unchanged. A complete run produces IR, optimized IR, Verilog,
interface/module signatures, schedules, lowered IR, option snapshots,
source-line maps, pass metrics, block metrics, and structured HLS data for every
design.
See the [Google XLS tutorial](tutorials/xls_flow) for the complete flow and
output-field reference.

### Vitis Modern Test Designs (`test_designs_vitis_modern`)

- **Best for**: Validating the config-file-based Vitis HLS flow (`VitisHLSModernSynthFlow`, `VitisHLSModernCsimFlow`).
- **Notes**: Contains accumulator and vector-add kernels, each driven by an `hls_config.cfg` file instead of a Tcl script.

### XLS Upstream Designs (`xls_upstream`)

- **Upstream**: [Google XLS repository](https://github.com/google/xls)
- **Best for**: Larger, realistic DSLX workloads beyond the small `test_designs_xls` kernels.
- **Notes**: 25 designs (for example `crc32`, `sha256`, `aes_encrypt`, `fpexp_32`, `riscv_simple`) configured for `XLSHLSSynthFlow` with a one-stage pipeline generator and unit delay model by default.

### SPECTOR (`spector_hls`)

- **Upstream**: [Paper (FPT 2016)](https://kastner.ucsd.edu/wp-content/uploads/2013/08/admin/fpt16-spector.pdf), [Repository](https://github.com/KastnerRG/spector)
- **Best for**: Design-space exploration on parameterized image-processing and linear-algebra kernels.
- **Notes**: 10 designs (`dct`, `fir`, `histogram`, `matrix_multiplication`, `mergesort`, `normals`, `sobel_filter_x`, `sobel_filter_y`, `spmv`, `template_matching`), each with an OptDSL template plus Vitis synthesis and C simulation configurations.

**Citation:**
```
Quentin Gautier, Alric Althoff, Pingfan Meng, and Ryan Kastner. 2016. Spector: An OpenCL FPGA Benchmark Suite. In 2016 International Conference on Field-Programmable Technology (FPT).
```

### Altera Reference Designs (`altera_examples`)

- **Upstream**: [Altera hls-samples ReferenceDesigns](https://github.com/altera-fpga/hls-samples/tree/0abae6d78af5daca3fe5d67e617ab037e58aff89/ReferenceDesigns) (commit `0abae6d`)
- **Best for**: Exercising the Altera HLS IP Gen and Quartus flows on realistic reference designs.
- **Notes**: All 17 upstream reference-design folders are vendored unchanged, with HLSFactory TOML configuration replacing the upstream CMake build. Each directory keeps Altera's MIT-style `License.txt`. Matmul and merge sort also support simulation. See the [Altera tutorial](tutorials/altera_flow).

### Sharc Lab Accelerators (`accelerators`)

- **Upstream**:
  - [FlowGNN (HPCA 2023)](https://doi.org/10.1109/HPCA56546.2023.10071015), [Repository](https://github.com/sharc-lab/FlowGNN)
  - [DGNN-Booster (FCCM 2023)](https://doi.org/10.1109/FCCM57271.2023.00029)
  - [Edge-MoE (ICCAD 2023)](https://doi.org/10.1109/ICCAD57390.2023.10323651), [Repository](https://github.com/sharc-lab/Edge-MoE)
  - [Mask-Net (ASAP 2022)](https://doi.org/10.1109/ASAP54787.2022.00030)
  - [SkyNet (MLSys 2020)](https://proceedings.mlsys.org/paper/2020/hash/2bb232c0b13c774965ef8558f0fbd615-Abstract.html), [Repository](https://github.com/TomG008/SkyNet)
- **Best for**: Exploring machine-learning oriented accelerator blocks.
- **Notes**: 7 designs (two DGNN-Booster designs, fixed- and floating-point GP selection kernels, Mask-Net, SkyNet, and Edge-MoE), each configured for Vitis HLS synthesis only; OptDSL and Intel scripts are not yet published.

**Citation:**
```
Rishov Sarkar, Stefan Abi-Karam, Yuqi He, Lakshmi Sathidevi, and Cong Hao. 2023. FlowGNN: A Dataflow Architecture for Real-Time Workload-Agnostic Graph Neural Network Inference. In 2023 IEEE International Symposium on High-Performance Computer Architecture (HPCA). IEEE, Montreal, QC, Canada, 1099–1112. https://doi.org/10.1109/HPCA56546.2023.10071015
Hanqiu Chen and Cong Hao. 2023. DGNN-booster: A Generic FPGA Accelerator Framework for Dynamic Graph Neural Network Inference. In 2023 IEEE 31st Annual International Symposium on Field-Programmable Custom Computing Machines (FCCM). IEEE, Marina Del Rey, CA, USA, 195–201. https://doi.org/10.1109/FCCM57271.2023.00029
Rishov Sarkar, Hanxue Liang, Zhiwen Fan, Zhangyang Wang, and Cong Hao. 2023. Edge-MoE: Memory-Efficient Multi-Task Vision Transformer Architecture with Task-Level Sparsity via Mixture-of-Experts. In 2023 IEEE/ACM International Conference on Computer Aided Design (ICCAD). IEEE, San Francisco, CA, USA, 01–09. https://doi.org/10.1109/ICCAD57390.2023.10323651
Xiaofan Zhang, Haoming Lu, Cong Hao, Jiachen Li, Bowen Cheng, Yuhong Li, Kyle Rupnow, Jinjun Xiong, Thomas Huang, Honghui Shi, Wen-Mei Hwu, and Deming Chen. 2020. SkyNet: A Hardware-Efficient Method for Object Detection and Tracking on Embedded Systems. Proceedings of Machine Learning and Systems 2 (March 2020), 216–229.
```

### SODA (`soda`)

- **Upstream**: [Repository](https://github.com/UCLA-VAST/soda), [Paper](https://doi.org/10.1145/3240765.3240850) (mirrored snapshot)
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

- **Upstream**: [Stream-HLS (FPGA 2025)](https://doi.org/10.1145/3706628.3708878), [arXiv](https://arxiv.org/abs/2501.09118), [Repository](https://github.com/UCLA-VAST/Stream-HLS)
- **Best for**: Studying transformer-style dataflow accelerators alongside adapted PolyBench kernels built with the Stream-HLS compiler.
- **Notes**: Contains 77 designs spanning neural network blocks, residual units, and dense linear algebra kernels synthesized for streaming dataflow execution.

**Citation:**
```
Suhail Basalama and Jason Cong. 2025. Stream-HLS: Towards Automatic Dataflow Acceleration. In Proceedings of the 2025 ACM/SIGDA International Symposium on Field Programmable Gate Arrays. ACM, Monterey CA USA, 103–114. https://doi.org/10.1145/3706628.3708878
```

### AutoNTT (`auto_ntt`)

- **Upstream**: [AutoNTT (FCCM 2025)](https://doi.org/10.1109/FCCM62733.2025.00024), [Repository](https://github.com/SFU-HiAccel/AutoNTT)
- **Best for**: Exploring automated generation of Number Theoretic Transform accelerators with configurable architecture templates.
- **Notes**: Ships with the AutoNTT generator, sample generated kernels, and supporting scripts so you can synthesize new design variants directly from the toolkit.

**Citation:**
```
Dilshan Kumarathunga, Qilin Hu, and Zhenman Fang. 2025. AutoNTT: Automatic Architecture Design and Exploration for Number Theoretic Transform Acceleration on FPGAs. In 2025 IEEE 33rd Annual International Symposium on Field-Programmable Custom Computing Machines (FCCM). IEEE, Fayetteville, AR, USA, 1–9. https://doi.org/10.1109/FCCM62733.2025.00024
```

### ForgeBench (`forgebench`)

- **Upstream**: [Paper (arXiv:2504.15185)](https://arxiv.org/abs/2504.15185), [Repository](https://github.com/hchen799/ForgeBench)
- **Best for**: Stress-testing flow robustness across a large, heterogeneous set of designs.
- **Notes**: Contains 45 standalone designs with Vitis Tcl entry points and targets next-generation HLS tooling with GEMM operators, deep neural network blocks, and transformer-ready pipelines. OptDSL templates and Intel support are planned.

**Citation:**
```
Andy Wanna, Hanqiu Chen, and Cong Hao. 2025. ForgeBench: A Machine Learning Benchmark Suite and Auto-Generation Framework for Next-Generation HLS Tools. https://doi.org/10.48550/arXiv.2504.15185 arXiv:2504.15185 [cs]
```
