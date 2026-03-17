# HLSFactory

***HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond***

HLSFactory is a framework for High-Level Synthesis (HLS) design datasets. It provides both the facilities to collect and build custom HLS datasets using various frontends, supported HLS tools, and data aggregation as well as provides built-in design dataset sources for users who want to run their own experiments out of the box. We also provide easy facilities for new users to contribute their own HLS designs to the existing design datasets as well as augment the existing tool flows to support custom flows such as new front ends for design space sampling and new vendor tool support.

## New to HLSFactory?

If you have HLS experience but are new to this framework: start with [Concepts](concepts) for the mental model, then [Setup](setup), then the [Xilinx Flow Tutorial](tutorials/xilinx_flow).

## Quick Guide

**Run experiments:** Jump right in with the [Xilinx Flow tutorial](tutorials/xilinx_flow) and the [Setup](setup) guide.

**Contribute designs:** Add your own designs and flows via the [Extending](extending) guide.

- [Setup](setup): Install and configure HLSFactory
- [Concepts](concepts): Core abstractions and value proposition
- [Framework](framework/index): Frontend parameterization, backend tool flows, data aggregation
- [Tutorials](tutorials/index): Guided tutorials and demos
- [HLS Design Collection](built_in_datasets): Community-curated HLS design corpora
- [Extending](extending): Contribute designs, datasets, and custom flows
- [API Reference](apidocs/index): Python API documentation

## HLS Design Collection

The [HLS Design Collection](built_in_datasets) is a community-organized effort to build the largest collection of HLS designs available in one place. It is a major effort within HLSFactory, bringing together curated HLS design corpora from across the research community.

We hope that this effort allows HLSFactory to be the one-stop-shop for HLS designs for any application or research as well as highlight HLS projects across the HLS and FPGA community. Any contributed design will be cited through our HLS Factory work and linked directly to credit the original authors and their works as well.

### Open to Community Contributions!
HLSFactory welcomes open-source HLS designs from the community. If you have designs you would like to add to the collection, or need help integrating them, please reach out. We are more than happy to assist!


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

concepts

framework/index

tutorials/index

built_in_datasets

extending

apidocs/index
```
