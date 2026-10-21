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

See the [setup instructions](https://sharc-lab.github.io/HLSFactory/setup.html) in the docs for tool configuration and full installation details.

## Design Collections

The [HLSFactory HLS Design Collection](https://sharc-lab.github.io/HLSFactory/built_in_datasets.html) is a browsable, searchable index of every built-in dataset. It is a community-organized effort to bring curated HLS design corpora from across the research community into one place, and currently packages 19 datasets with over 300 designs. You can load any of them with `datasets_builder`:

```python
from pathlib import Path

from hlsfactory.datasets_builtin import datasets_builder

datasets = datasets_builder(Path("work_dir"), ["polybench", "machsuite"])
```

| Category | Datasets |
| --- | --- |
| Classic HLS benchmarks | `polybench`, `machsuite`, `chstone`, `mibench`, `pp4fpgas`, `spector_hls` |
| Vendor examples | `vitis_examples`, `altera_examples` |
| Research accelerators | `accelerators` (Sharc Lab), `soda`, `hp_fft`, `stream_hls`, `auto_ntt`, `forgebench` |
| Vendor toolchain designs | `test_designs_catapult`, `test_designs_stratus`, `test_designs_xls`, `test_designs_vitis_modern`, `xls_upstream` |

Most datasets target AMD/Xilinx Vitis HLS; the Catapult, Stratus, and XLS datasets are small designs for validating those toolchains. Datasets keep the licensing terms of their upstream repositories.

**Where to start:** use `polybench` for a first run or `vitis_examples` to validate your toolchain. For ML and QoR work, `polybench`, `machsuite`, and `chstone` are commonly used together.

The catalog page has per-dataset details, upstream links, and instructions to [contribute your own designs](https://sharc-lab.github.io/HLSFactory/built_in_datasets.html#contributing-your-hls-designs).

## Flows

Everything in HLSFactory is a **flow**: a step that takes a design and returns zero or more designs. Frontend flows expand one base design into many (for example, sampling an optimization design space), and tool flows run an external vendor tool and store the results alongside the design. Flows run in parallel across designs and datasets, so a full pipeline is a short chain:

```python
# frontend sampling -> HLS synthesis -> implementation
designs = OptDSLFrontend(...).execute_multiple_designs(base_designs, n_jobs=8)
designs = VitisHLSSynthFlow(...).execute_multiple_designs(designs, n_jobs=8)
designs = VitisHLSImplFlow(...).execute_multiple_designs(designs, n_jobs=8)
```

Custom flows and frontends plug into the same interface.

**Frontends** (design space sampling):

| Frontend | Purpose |
| --- | --- |
| OptDSL (Vitis and Intel variants) | Enumerate optimization directive combinations from a DSL template |
| OptDSL passthrough | Produce a baseline copy of each design with an empty `opt.tcl` (no directives) |
| Jinja template | Render design variants from Jinja templates and a list of parameter configs |
| C/C++ preprocessor | Generate one variant per set of preprocessor `#define` values |

**Tool flows:**

| Vendor / tool | Flows |
| --- | --- |
| AMD/Xilinx Vitis HLS + Vivado | Synthesis, C simulation, co-simulation (with setup), implementation and implementation reports, config-file ("modern") synthesis and C simulation |
| Intel HLS + Quartus | HLS synthesis, Quartus implementation |
| Altera HLS + Quartus | HLS synthesis, C simulation, co-simulation, Quartus implementation |
| Siemens Catapult HLS | Synthesis |
| Cadence Stratus HLS | Synthesis |
| Google XLS | DSLX to Verilog synthesis |
| Bambu | Synthesis |
| Analysis and ML | LightningSim, Balor graphs, HARP graphs, lines-of-code counting, AutoDSE |

Learn more in the [Flows guide](https://sharc-lab.github.io/HLSFactory/framework/flows.html), the [tutorials](https://sharc-lab.github.io/HLSFactory/tutorials/index.html), and the runnable demos in [`demos/`](demos/).

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
