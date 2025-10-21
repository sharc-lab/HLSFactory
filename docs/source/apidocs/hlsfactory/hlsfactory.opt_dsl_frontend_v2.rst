:py:mod:`hlsfactory.opt_dsl_frontend_v2`
========================================

.. py:module:: hlsfactory.opt_dsl_frontend_v2

.. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_v2
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`OptDSLFrontend <hlsfactory.opt_dsl_frontend_v2.OptDSLFrontend>`
     -

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`count_possible_samples <hlsfactory.opt_dsl_frontend_v2.count_possible_samples>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_v2.count_possible_samples
          :summary:
   * - :py:obj:`unique_random_samples <hlsfactory.opt_dsl_frontend_v2.unique_random_samples>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_v2.unique_random_samples
          :summary:
   * - :py:obj:`generate_opt_sources <hlsfactory.opt_dsl_frontend_v2.generate_opt_sources>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_v2.generate_opt_sources
          :summary:

API
~~~

.. py:function:: count_possible_samples(data)
   :canonical: hlsfactory.opt_dsl_frontend_v2.count_possible_samples

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_v2.count_possible_samples

.. py:function:: unique_random_samples(data, num_samples)
   :canonical: hlsfactory.opt_dsl_frontend_v2.unique_random_samples

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_v2.unique_random_samples

.. py:function:: generate_opt_sources(static_lines: list[str], groups, pipelines, partitions, unrolls, random_sample: bool = False, random_sample_num: int = 10, random_sample_seed: int = 42) -> list[str]
   :canonical: hlsfactory.opt_dsl_frontend_v2.generate_opt_sources

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_v2.generate_opt_sources

.. py:class:: OptDSLFrontend(work_dir: pathlib.Path, random_sample: bool = False, random_sample_num: int = 10, random_sample_seed: int = 42, log_execution_time: bool = True)
   :canonical: hlsfactory.opt_dsl_frontend_v2.OptDSLFrontend

   Bases: :py:obj:`hlsfactory.framework.Frontend`

   .. py:attribute:: name
      :canonical: hlsfactory.opt_dsl_frontend_v2.OptDSLFrontend.name
      :value: 'OptDSLFrontend'

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_v2.OptDSLFrontend.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.opt_dsl_frontend_v2.OptDSLFrontend.execute

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_v2.OptDSLFrontend.execute
