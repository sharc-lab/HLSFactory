# HLSFactory

***HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond***

HLSFactory is a framework for High-Level Synthesis (HLS) design datasets. It provides both the facilities to collect and build custom HLS datasets using various frontends, supported HLS tools, and data aggregation as well as provides built-in design dataset sources for users who want to run their own experiments out of the box. We also provide easy facilities for new users to contribute their own HLS designs to the existing design datasets as well as augment the existing tool flows to support custom flows such as new front ends for design space sampling and new vendor tool support.

## New to HLSFactory?

If you have HLS experience but are new to this framework: start with [Concepts](concepts) for the mental model, then [Setup](setup), followed by the tutorial for your toolchain: [Xilinx](tutorials/xilinx_flow), [Intel](tutorials/intel_flow), [Altera](tutorials/altera_flow), [Google XLS](tutorials/xls_flow), or [Siemens Catapult](tutorials/catapult_flow).

## HLS Design Collection

The [HLS Design Collection](built_in_datasets) is a community-organized effort to build the largest collection of HLS designs available in one place. It brings together curated HLS design corpora from across the research community, including PolyBench, MachSuite, CHStone, MiBench, SPECTOR, Stream-HLS, ForgeBench and more, with designs for AMD Xilinx, Intel, Altera, Siemens Catapult, Cadence Stratus and Google XLS. Load any dataset by key with `hlsfactory.datasets_builtin.datasets_builder`.

The collection page has a summary table with design counts, flow coverage and OptDSL support, plus a section per dataset with upstream links. Any contributed design is cited and linked to credit the original authors.

### Open to Community Contributions!

HLSFactory welcomes open-source HLS designs from the community. If you have designs you would like to add, or need help integrating them, see the [Extending](extending) guide or reach out. We are more than happy to assist!

## Quick Guide

**Run experiments:** Use the [Setup](setup) guide, then follow the [Xilinx](tutorials/xilinx_flow), [Intel](tutorials/intel_flow), [Altera](tutorials/altera_flow), [Google XLS](tutorials/xls_flow), or [Siemens Catapult](tutorials/catapult_flow) tutorial.

**Contribute designs:** Add your own designs and flows via the [Extending](extending) guide.

- [Setup](setup): Install and configure HLSFactory
- [Concepts](concepts): Core abstractions and value proposition
- [Framework](framework/index): Frontend parameterization, backend tool flows, data aggregation
- [Tutorials](tutorials/index): Guided tutorials and demos
- [HLS Design Collection](built_in_datasets): Community-curated HLS design corpora
- [Extending](extending): Contribute designs, datasets, and custom flows
- [API Reference](apidocs/index): Python API documentation

## Publications and Citing

HLSFactory won the **Best Paper Award** and was nominated for the **Artifact Evaluation Award** at the 2024 ACM/IEEE 6th Symposium on Machine Learning for CAD (MLCAD). It has been published in:

- MLCAD 2024: [HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond](https://doi.org/10.1109/MLCAD62225.2024.10740213) ([arXiv](https://arxiv.org/abs/2405.00820))
- OSCAR Workshop at ISCA 2024: [An Open Source Framework for High-Level Synthesis Dataset Generation for Machine Learning](https://oscar-workshop.github.io)

If you use HLSFactory in your research, please cite:

```bibtex
@inproceedings{hlsfactory,
  title = {{{HLSFactory}}: {{A Framework Empowering High-Level Synthesis Datasets}} for {{Machine Learning}} and {{Beyond}}},
  booktitle = {2024 {{ACM}}/{{IEEE}} 6th {{Symposium}} on {{Machine Learning}} for {{CAD}} ({{MLCAD}})},
  author = {{Abi-Karam}, Stefan and Sarkar, Rishov and Seigler, Allison and Lowe, Sean and Wei, Zhigang and Chen, Hanqiu and Rao, Nanditha and John, Lizy and Arora, Aman and Hao, Cong},
  year = {2024},
  month = sep,
  pages = {1--9},
  publisher = {IEEE},
  doi = {10.1109/MLCAD62225.2024.10740213}
}
```

## Source Code

The source code repository is hosted on GitHub: [https://github.com/sharc-lab/hlsfactory](https://github.com/sharc-lab/hlsfactory)

## License

The source code is hosted under the *AGPLv3* license. Please see the LICENSE file in the source code repository for more details.

```{toctree}
:hidden:

self

setup

concepts

framework/index

tutorials/index

built_in_datasets

extending

apidocs/index
```
