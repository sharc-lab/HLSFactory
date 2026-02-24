:py:mod:`hlsfactory.loc_flow`
=============================

.. py:module:: hlsfactory.loc_flow

.. autodoc2-docstring:: hlsfactory.loc_flow
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`LOCCounterFlow <hlsfactory.loc_flow.LOCCounterFlow>`
     -

API
~~~

.. py:class:: LOCCounterFlow(log_execution_time: bool = True)
   :canonical: hlsfactory.loc_flow.LOCCounterFlow

   Bases: :py:obj:`hlsfactory.framework.Flow`

   .. py:attribute:: name
      :canonical: hlsfactory.loc_flow.LOCCounterFlow.name
      :value: 'LOCCounterFlow'

      .. autodoc2-docstring:: hlsfactory.loc_flow.LOCCounterFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.loc_flow.LOCCounterFlow.execute

      .. autodoc2-docstring:: hlsfactory.loc_flow.LOCCounterFlow.execute
