:py:mod:`hlsfactory.define_frontend`
====================================

.. py:module:: hlsfactory.define_frontend

.. autodoc2-docstring:: hlsfactory.define_frontend
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`CPPPreprocessorFrontend <hlsfactory.define_frontend.CPPPreprocessorFrontend>`
     -

API
~~~

.. py:class:: CPPPreprocessorFrontend(work_dir: pathlib.Path, define_statements: list[dict[str, typing.Any]], log_execution_time: bool = True)
   :canonical: hlsfactory.define_frontend.CPPPreprocessorFrontend

   Bases: :py:obj:`hlsfactory.framework.Frontend`

   .. py:attribute:: name
      :canonical: hlsfactory.define_frontend.CPPPreprocessorFrontend.name
      :value: 'CPPPreprocessorFrontend'

      .. autodoc2-docstring:: hlsfactory.define_frontend.CPPPreprocessorFrontend.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.define_frontend.CPPPreprocessorFrontend.execute

      .. autodoc2-docstring:: hlsfactory.define_frontend.CPPPreprocessorFrontend.execute
