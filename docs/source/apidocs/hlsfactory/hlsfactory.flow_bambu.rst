:py:mod:`hlsfactory.flow_bambu`
===============================

.. py:module:: hlsfactory.flow_bambu

.. autodoc2-docstring:: hlsfactory.flow_bambu
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`BambuSynthFlow <hlsfactory.flow_bambu.BambuSynthFlow>`
     -

API
~~~

.. py:class:: BambuSynthFlow(bambu_bin: str | None = None, log_output: bool = False, log_execution_time: bool = True)
   :canonical: hlsfactory.flow_bambu.BambuSynthFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: name
      :canonical: hlsfactory.flow_bambu.BambuSynthFlow.name
      :value: 'BambuSynthFlow'

      .. autodoc2-docstring:: hlsfactory.flow_bambu.BambuSynthFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_bambu.BambuSynthFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_bambu.BambuSynthFlow.execute
