:py:mod:`hlsfactory.opt_dsl_v2.opt_dsl`
=======================================

.. py:module:: hlsfactory.opt_dsl_v2.opt_dsl

.. autodoc2-docstring:: hlsfactory.opt_dsl_v2.opt_dsl
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`OptDSL <hlsfactory.opt_dsl_v2.opt_dsl.OptDSL>`
     - .. autodoc2-docstring:: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL
          :summary:

API
~~~

.. py:class:: OptDSL(opt_dsl_source: str)
   :canonical: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL

   .. autodoc2-docstring:: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL.__init__

   .. py:method:: parse_opt_dsl_lang(opt_dsl_source) -> None
      :canonical: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL.parse_opt_dsl_lang

      .. autodoc2-docstring:: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL.parse_opt_dsl_lang

   .. py:method:: pipeline(label: str, function: str, optional: bool = False) -> None
      :canonical: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL.pipeline

      .. autodoc2-docstring:: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL.pipeline

   .. py:method:: unroll(label: str, function: str, factor, group: str | None = None) -> None
      :canonical: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL.unroll

      .. autodoc2-docstring:: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL.unroll

   .. py:method:: partition(array_var: str, function: str, partition_type: str, factor: list[int] | int, dim: int, group: str | None = None) -> None
      :canonical: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL.partition

      .. autodoc2-docstring:: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL.partition

   .. py:method:: validate_grouped_directives() -> None
      :canonical: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL.validate_grouped_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL.validate_grouped_directives

   .. py:method:: get_directives()
      :canonical: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL.get_directives

      .. autodoc2-docstring:: hlsfactory.opt_dsl_v2.opt_dsl.OptDSL.get_directives
