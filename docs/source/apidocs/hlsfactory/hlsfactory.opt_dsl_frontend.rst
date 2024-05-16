:py:mod:`hlsfactory.opt_dsl_frontend`
=====================================

.. py:module:: hlsfactory.opt_dsl_frontend

.. autodoc2-docstring:: hlsfactory.opt_dsl_frontend
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`ArrayPartition <hlsfactory.opt_dsl_frontend.ArrayPartition>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.ArrayPartition
          :summary:
   * - :py:obj:`LoopOpt <hlsfactory.opt_dsl_frontend.LoopOpt>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.LoopOpt
          :summary:
   * - :py:obj:`OptDSLFrontend <hlsfactory.opt_dsl_frontend.OptDSLFrontend>`
     -
   * - :py:obj:`OptDSLPassthroughFrontend <hlsfactory.opt_dsl_frontend.OptDSLPassthroughFrontend>`
     -

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`parse_template <hlsfactory.opt_dsl_frontend.parse_template>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.parse_template
          :summary:
   * - :py:obj:`gen_opt <hlsfactory.opt_dsl_frontend.gen_opt>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.gen_opt
          :summary:
   * - :py:obj:`generate_opt_sources <hlsfactory.opt_dsl_frontend.generate_opt_sources>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.generate_opt_sources
          :summary:

API
~~~

.. py:class:: ArrayPartition(array_settings: str)
   :canonical: hlsfactory.opt_dsl_frontend.ArrayPartition

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.ArrayPartition

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.ArrayPartition.__init__

   .. py:method:: get_flattened()
      :canonical: hlsfactory.opt_dsl_frontend.ArrayPartition.get_flattened

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.ArrayPartition.get_flattened

   .. py:method:: get_num_of_directives()
      :canonical: hlsfactory.opt_dsl_frontend.ArrayPartition.get_num_of_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.ArrayPartition.get_num_of_directives

   .. py:method:: append_directives(line)
      :canonical: hlsfactory.opt_dsl_frontend.ArrayPartition.append_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.ArrayPartition.append_directives

   .. py:method:: get_directives()
      :canonical: hlsfactory.opt_dsl_frontend.ArrayPartition.get_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.ArrayPartition.get_directives

.. py:class:: LoopOpt(loop_settings: str)
   :canonical: hlsfactory.opt_dsl_frontend.LoopOpt

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.LoopOpt

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.LoopOpt.__init__

   .. py:method:: get_flattened()
      :canonical: hlsfactory.opt_dsl_frontend.LoopOpt.get_flattened

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.LoopOpt.get_flattened

   .. py:method:: append_parameters(line)
      :canonical: hlsfactory.opt_dsl_frontend.LoopOpt.append_parameters

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.LoopOpt.append_parameters

   .. py:method:: append_directives(line)
      :canonical: hlsfactory.opt_dsl_frontend.LoopOpt.append_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.LoopOpt.append_directives

   .. py:method:: get_directives()
      :canonical: hlsfactory.opt_dsl_frontend.LoopOpt.get_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.LoopOpt.get_directives

   .. py:method:: get_num_of_parameters()
      :canonical: hlsfactory.opt_dsl_frontend.LoopOpt.get_num_of_parameters

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.LoopOpt.get_num_of_parameters

   .. py:method:: get_num_of_directives()
      :canonical: hlsfactory.opt_dsl_frontend.LoopOpt.get_num_of_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.LoopOpt.get_num_of_directives

.. py:function:: parse_template(src_template: pathlib.Path) -> tuple[list[hlsfactory.opt_dsl_frontend.ArrayPartition], list[hlsfactory.opt_dsl_frontend.LoopOpt], str]
   :canonical: hlsfactory.opt_dsl_frontend.parse_template

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.parse_template

.. py:function:: gen_opt(array_partition_object_lists, loop_opt_object_lists)
   :canonical: hlsfactory.opt_dsl_frontend.gen_opt

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.gen_opt

.. py:function:: generate_opt_sources(array_partition_lines, loop_opt_lines, static_lines, random_sample: bool = False, random_sample_num: int = 10, random_sample_seed: int = 42) -> list[str]
   :canonical: hlsfactory.opt_dsl_frontend.generate_opt_sources

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.generate_opt_sources

.. py:class:: OptDSLFrontend(work_dir: pathlib.Path, random_sample: bool = False, random_sample_num: int = 10, random_sample_seed: int = 42, log_execution_time: bool = True)
   :canonical: hlsfactory.opt_dsl_frontend.OptDSLFrontend

   Bases: :py:obj:`hlsfactory.framework.Frontend`

   .. py:attribute:: name
      :canonical: hlsfactory.opt_dsl_frontend.OptDSLFrontend.name
      :value: 'OptDSLFrontend'

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.OptDSLFrontend.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.opt_dsl_frontend.OptDSLFrontend.execute

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.OptDSLFrontend.execute

.. py:class:: OptDSLPassthroughFrontend(work_dir: pathlib.Path)
   :canonical: hlsfactory.opt_dsl_frontend.OptDSLPassthroughFrontend

   Bases: :py:obj:`hlsfactory.framework.Frontend`

   .. py:attribute:: name
      :canonical: hlsfactory.opt_dsl_frontend.OptDSLPassthroughFrontend.name
      :value: 'OptDSLPassthroughFrontend'

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.OptDSLPassthroughFrontend.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.opt_dsl_frontend.OptDSLPassthroughFrontend.execute

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend.OptDSLPassthroughFrontend.execute
