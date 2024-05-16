:py:mod:`hlsfactory.opt_dsl_frontend_intel`
===========================================

.. py:module:: hlsfactory.opt_dsl_frontend_intel

.. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`ArrayPartition <hlsfactory.opt_dsl_frontend_intel.ArrayPartition>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.ArrayPartition
          :summary:
   * - :py:obj:`LoopOpt <hlsfactory.opt_dsl_frontend_intel.LoopOpt>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.LoopOpt
          :summary:
   * - :py:obj:`OptDSLFrontendIntel <hlsfactory.opt_dsl_frontend_intel.OptDSLFrontendIntel>`
     -

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`parse_template <hlsfactory.opt_dsl_frontend_intel.parse_template>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.parse_template
          :summary:
   * - :py:obj:`gen_opt <hlsfactory.opt_dsl_frontend_intel.gen_opt>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.gen_opt
          :summary:
   * - :py:obj:`get_array_partition_dic <hlsfactory.opt_dsl_frontend_intel.get_array_partition_dic>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.get_array_partition_dic
          :summary:
   * - :py:obj:`get_loop_unroll_dic <hlsfactory.opt_dsl_frontend_intel.get_loop_unroll_dic>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.get_loop_unroll_dic
          :summary:
   * - :py:obj:`get_pipeline_list <hlsfactory.opt_dsl_frontend_intel.get_pipeline_list>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.get_pipeline_list
          :summary:
   * - :py:obj:`get_kernel <hlsfactory.opt_dsl_frontend_intel.get_kernel>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.get_kernel
          :summary:
   * - :py:obj:`polybench_copy <hlsfactory.opt_dsl_frontend_intel.polybench_copy>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.polybench_copy
          :summary:
   * - :py:obj:`generate_annotate_c <hlsfactory.opt_dsl_frontend_intel.generate_annotate_c>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.generate_annotate_c
          :summary:

Data
~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`polybench_header_text <hlsfactory.opt_dsl_frontend_intel.polybench_header_text>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.polybench_header_text
          :summary:

API
~~~

.. py:class:: ArrayPartition(array_settings: str)
   :canonical: hlsfactory.opt_dsl_frontend_intel.ArrayPartition

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.ArrayPartition

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.ArrayPartition.__init__

   .. py:method:: get_flattened() -> list
      :canonical: hlsfactory.opt_dsl_frontend_intel.ArrayPartition.get_flattened

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.ArrayPartition.get_flattened

   .. py:method:: get_num_of_directives()
      :canonical: hlsfactory.opt_dsl_frontend_intel.ArrayPartition.get_num_of_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.ArrayPartition.get_num_of_directives

   .. py:method:: append_directives(line)
      :canonical: hlsfactory.opt_dsl_frontend_intel.ArrayPartition.append_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.ArrayPartition.append_directives

   .. py:method:: get_directives()
      :canonical: hlsfactory.opt_dsl_frontend_intel.ArrayPartition.get_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.ArrayPartition.get_directives

.. py:class:: LoopOpt(loop_settings)
   :canonical: hlsfactory.opt_dsl_frontend_intel.LoopOpt

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.LoopOpt

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.LoopOpt.__init__

   .. py:method:: get_flattened()
      :canonical: hlsfactory.opt_dsl_frontend_intel.LoopOpt.get_flattened

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.LoopOpt.get_flattened

   .. py:method:: append_parameters(line)
      :canonical: hlsfactory.opt_dsl_frontend_intel.LoopOpt.append_parameters

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.LoopOpt.append_parameters

   .. py:method:: append_directives(line)
      :canonical: hlsfactory.opt_dsl_frontend_intel.LoopOpt.append_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.LoopOpt.append_directives

   .. py:method:: get_directives()
      :canonical: hlsfactory.opt_dsl_frontend_intel.LoopOpt.get_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.LoopOpt.get_directives

   .. py:method:: get_num_of_parameters()
      :canonical: hlsfactory.opt_dsl_frontend_intel.LoopOpt.get_num_of_parameters

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.LoopOpt.get_num_of_parameters

   .. py:method:: get_num_of_directives()
      :canonical: hlsfactory.opt_dsl_frontend_intel.LoopOpt.get_num_of_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.LoopOpt.get_num_of_directives

.. py:function:: parse_template(src_template: pathlib.Path) -> tuple[list[hlsfactory.opt_dsl_frontend_intel.ArrayPartition], list[hlsfactory.opt_dsl_frontend_intel.LoopOpt], str]
   :canonical: hlsfactory.opt_dsl_frontend_intel.parse_template

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.parse_template

.. py:function:: gen_opt(array_partition_object_lists, loop_opt_object_lists)
   :canonical: hlsfactory.opt_dsl_frontend_intel.gen_opt

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.gen_opt

.. py:function:: get_array_partition_dic(lines: str) -> dict[str, str]
   :canonical: hlsfactory.opt_dsl_frontend_intel.get_array_partition_dic

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.get_array_partition_dic

.. py:function:: get_loop_unroll_dic(lines: str) -> dict[str, str]
   :canonical: hlsfactory.opt_dsl_frontend_intel.get_loop_unroll_dic

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.get_loop_unroll_dic

.. py:function:: get_pipeline_list(lines: str) -> list[str]
   :canonical: hlsfactory.opt_dsl_frontend_intel.get_pipeline_list

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.get_pipeline_list

.. py:function:: get_kernel(hls_template: pathlib.Path) -> (str, str)
   :canonical: hlsfactory.opt_dsl_frontend_intel.get_kernel

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.get_kernel

.. py:data:: polybench_header_text
   :canonical: hlsfactory.opt_dsl_frontend_intel.polybench_header_text
   :value: ['#include <unistd.h> \n', '#include <string.h> \n', '#include <math.h> \n', '#include <HLS/stdio.h>...

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.polybench_header_text

.. py:function:: polybench_copy(source_dir: pathlib.Path, target_dir: pathlib.Path, kernel_name: str) -> None
   :canonical: hlsfactory.opt_dsl_frontend_intel.polybench_copy

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.polybench_copy

.. py:function:: generate_annotate_c(design_dir: pathlib.Path, array_partition_lines, loop_opt_lines, static_lines, work_dir: pathlib.Path, kernel_name: str, kernel_file: str, random_sample=False, random_sample_num=10) -> list[hlsfactory.framework.Design]
   :canonical: hlsfactory.opt_dsl_frontend_intel.generate_annotate_c

   .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.generate_annotate_c

.. py:class:: OptDSLFrontendIntel(work_dir: pathlib.Path, random_sample=False, random_sample_num=10)
   :canonical: hlsfactory.opt_dsl_frontend_intel.OptDSLFrontendIntel

   Bases: :py:obj:`hlsfactory.framework.Frontend`

   .. py:attribute:: name
      :canonical: hlsfactory.opt_dsl_frontend_intel.OptDSLFrontendIntel.name
      :value: 'OptDSLFrontendIntel'

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.OptDSLFrontendIntel.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.opt_dsl_frontend_intel.OptDSLFrontendIntel.execute

      .. autodoc2-docstring:: hlsfactory.opt_dsl_frontend_intel.OptDSLFrontendIntel.execute
