.. toctree::
   :hidden:
   :maxdepth: 2

   prebuilt_dataset
   hls_build_framework
   extending
   api

==========
HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond
==========

HLSFactory is a framework for High-Level Synthesis (HLS) design datasets. It provides both the facilities to collect and build custom HLS datasets using various frontends, supported HLS tools, and data aggregation as well as provides built-in design dataset sources for users who want to run their own experiments out of the box. We also provide easy facilities for new users to contribute their own HLS designs to the existing design datasets as well as augment the existing tool flows to support custom flows such as new front ends for design space sampling and new vendor tool support.

Quick Guide
===========

* :doc:`prebuilt_dataset` - Detials on how to access and use the prebuilt HLS design dataset
* :doc:`hls_build_framework` - Details on the HLSDataset build framwork: 1) frontend paramateraization, 2) backend tool flows, 3) data aggregation and formatting
* :doc:`extending` - Details on how to add and extend the HLSDataset with your own desings or data

Source Code and License
=======================

The source code repository is hosted on GitHub: `https://github.com/sharc-lab/gnn-builder <https://github.com/sharc-lab/gnn-builder>`_

The source code is hosted under the *AGPLv3* license. Please see the LICENSE file in the source code repository for more details.

Publications
============

HLSDataset has been published in the following places:

* Arxiv: `HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond <https://arxiv.org/abs/2405.00820>`_

Citing and Referencing
======================

If you use HLSDataset in your research, please cite as follows:

.. code-block:: text
   :class: wrap

   S. Abi-Karam et al., “HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond.” arXiv, May 01, 2024. doi: 10.48550/arXiv.2405.00820.


.. code-block:: bibtex

   @misc{hlsfactory,
         title={HLSFactory: A Framework Empowering High-Level Synthesis Datasets for Machine Learning and Beyond}, 
         author={Stefan Abi-Karam and Rishov Sarkar and Allison Seigler and Sean Lowe and Zhigang Wei and Hanqiu Chen and Nanditha Rao and Lizy John and Aman Arora and Cong Hao},
         year={2024},
         eprint={2405.00820},
         archivePrefix={arXiv},
         primaryClass={cs.AR}
   }


