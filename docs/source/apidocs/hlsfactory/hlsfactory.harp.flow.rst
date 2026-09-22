:py:mod:`hlsfactory.harp.flow`
==============================

.. py:module:: hlsfactory.harp.flow

.. autodoc2-docstring:: hlsfactory.harp.flow
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`HARPGraphFlow <hlsfactory.harp.flow.HARPGraphFlow>`
     - .. autodoc2-docstring:: hlsfactory.harp.flow.HARPGraphFlow
          :summary:

Data
~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`GRAPH_FILENAME <hlsfactory.harp.flow.GRAPH_FILENAME>`
     - .. autodoc2-docstring:: hlsfactory.harp.flow.GRAPH_FILENAME
          :summary:
   * - :py:obj:`GRAPH_SUMMARY_FILENAME <hlsfactory.harp.flow.GRAPH_SUMMARY_FILENAME>`
     - .. autodoc2-docstring:: hlsfactory.harp.flow.GRAPH_SUMMARY_FILENAME
          :summary:

API
~~~

.. py:data:: GRAPH_FILENAME
   :canonical: hlsfactory.harp.flow.GRAPH_FILENAME
   :value: 'data_graph_harp.gexf'

   .. autodoc2-docstring:: hlsfactory.harp.flow.GRAPH_FILENAME

.. py:data:: GRAPH_SUMMARY_FILENAME
   :canonical: hlsfactory.harp.flow.GRAPH_SUMMARY_FILENAME
   :value: 'data_graph_harp.json'

   .. autodoc2-docstring:: hlsfactory.harp.flow.GRAPH_SUMMARY_FILENAME

.. py:class:: HARPGraphFlow(clang_bin: str | None = None, log_output: bool = False)
   :canonical: hlsfactory.harp.flow.HARPGraphFlow

   Bases: :py:obj:`hlsfactory.framework.Flow`

   .. autodoc2-docstring:: hlsfactory.harp.flow.HARPGraphFlow

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.harp.flow.HARPGraphFlow.__init__

   .. py:attribute:: name
      :canonical: hlsfactory.harp.flow.HARPGraphFlow.name
      :value: 'HARPGraphFlow'

      .. autodoc2-docstring:: hlsfactory.harp.flow.HARPGraphFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.harp.flow.HARPGraphFlow.execute

      .. autodoc2-docstring:: hlsfactory.harp.flow.HARPGraphFlow.execute

   .. py:method:: _extract(design_dir: pathlib.Path, top: str, source_path: pathlib.Path) -> None
      :canonical: hlsfactory.harp.flow.HARPGraphFlow._extract

      .. autodoc2-docstring:: hlsfactory.harp.flow.HARPGraphFlow._extract

   .. py:method:: _write_summary(design_dir: pathlib.Path, graph, top: str) -> None
      :canonical: hlsfactory.harp.flow.HARPGraphFlow._write_summary
      :staticmethod:

      .. autodoc2-docstring:: hlsfactory.harp.flow.HARPGraphFlow._write_summary
