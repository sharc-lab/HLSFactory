# HLSFactory

***HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond***

HLSFactory is a framework for High-Level Synthesis (HLS) design datasets. It provides both the facilities to collect and build custom HLS datasets using various frontends, supported HLS tools, and data aggregation as well as provides built-in design dataset sources for users who want to run their own experiments out of the box. We also provide easy facilities for new users to contribute their own HLS designs to the existing design datasets as well as augment the existing tool flows to support custom flows such as new front ends for design space sampling and new vendor tool support.

## Quick Guide

*If you want to jump right in and run some code, we recommend the [](./tutorials/xilinx_flow.md) tutorial along with the [](setup) guide.*

*If you want to contribute to the dataset as soon as possible, we recommend the [](extending) guide.*

- [](setup): Details on how to install and setup HLSFactory
- [](framework/index): Details on the HLSFactory build framework: 1) frontend parameterization, 2) backend tool flows, 3) data aggregation and formatting
- [](tutorials/index): Details on how to use the HLSFactory framework with guided tutorials and demos
- [](built_in_datasets): Details on the built-in HLS design datasets
- [](extending): Details on how to contribute to HLSFactory to add your own designs and datasets to the built-in datasets, add custom flows, and general contributions to the framework
- [](apidocs/index): Python API documentation for the HLSFactory framework

## Publications

HLSFactory has been published in the following places:

- 2024 ACM/IEEE 6th Symposium on Machine Learning for CAD (MLCAD)
  - [HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond](https://doi.org/10.1109/MLCAD62225.2024.10740213)
- ArXiv
  - [HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond](https://arxiv.org/abs/2405.00820)
- Open-Source Computer Architecture Research (OSCAR) Workshop at ISCA 2024
  - [An Open Source Framework for High-Level Synthesis Dataset Generation for Machine Learning](https://oscar-workshop.github.io)

### Awards

- **Best Paper Award** at the 2024 ACM/IEEE 6th Symposium on Machine Learning for CAD (MLCAD).
- **Artifact Evaluation Award Nomination** at the 2024 ACM/IEEE 6th Symposium on Machine Learning for CAD (MLCAD).

## Citing and Referencing

If you use HLSFactory in your research, please cite as follows:

```text
S. Abi-Karam et al., “HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond,” in 2024 ACM/IEEE 6th Symposium on Machine Learning for CAD (MLCAD), Sep. 2024, pp. 1–9. doi: 10.1109/MLCAD62225.2024.10740213.
```

```bibtex
@inproceedings{hlsfactory,
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

## Source Code

The source code repository is hosted on GitHub: [https://github.com/sharc-lab/hlsfactory](https://github.com/sharc-lab/hlsfactory)

## License

The source code is hosted under the *AGPLv3* license. Please see the LICENSE file in the source code repository for more details.

```{toctree}
:hidden:

self

setup

framework/index

tutorials/index

built_in_datasets

extending

apidocs/index
```
