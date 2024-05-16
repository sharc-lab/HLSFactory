# HLSFactory

***HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond***

HLSFactory is a framework for High-Level Synthesis (HLS) design datasets. It provides both the facilities to collect and build custom HLS datasets using various frontends, supported HLS tools, and data aggregation as well as provides built-in design dataset sources for users who want to run their own experiments out of the box. We also provide easy facilities for new users to contribute their own HLS designs to the existing design datasets as well as augment the existing tool flows to support custom flows such as new front ends for design space sampling and new vendor tool support.

## Quick Guide

- [](setup): Details on how to install and setup HLSDataset
- [](framework/index): Details on the HLSDataset build framework: 1) frontend parameterization, 2) backend tool flows, 3) data aggregation and formatting
- [](tutorials/index): Details on how to use the HLSDataset framework with guided tutorials and demos
- [](built_in_datasets): Details on the built-in HLS design datasets
- [](extending): Details on how to contribute to HLSFactory to add your own designs and datasets to the built-in datasets, add custom flows, and general contributions to the framework
- [](apidocs/index): Python API documentation for the HLSFactory framework

## Publications

HLSDataset has been published in the following places:

- ArXiv
  - [HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond](https://arxiv.org/abs/2405.00820)
- Open-Source Computer Architecture Research (OSCAR) Workshop at ISCA 2024
  - [An Open Source Framework for High-Level Synthesis Dataset Generation for Machine Learning](https://oscar-workshop.github.io)

## Citing and Referencing

If you use HLSDataset in your research, please cite as follows:

```text
S. Abi-Karam et al., “HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond.” arXiv, May 01, 2024. doi: 10.48550/arXiv.2405.00820.
```

```bibtex
@misc{hlsfactory,
      title={HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond}, 
      author={Stefan Abi-Karam and Rishov Sarkar and Allison Seigler and Sean Lowe and Zhigang Wei and Hanqiu Chen and Nanditha Rao and Lizy John and Aman Arora and Cong Hao},
      year={2024},
      eprint={2405.00820},
      archivePrefix={arXiv},
      primaryClass={cs.AR}
}
```

## Source Code

The source code repository is hosted on GitHub: [https://github.com/sharc-lab/gnn-builder](https://github.com/sharc-lab/gnn-builder)

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

