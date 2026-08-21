:py:mod:`hlsfactory.flow_lightningsim`
======================================

.. py:module:: hlsfactory.flow_lightningsim

.. autodoc2-docstring:: hlsfactory.flow_lightningsim
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`EvalResult <hlsfactory.flow_lightningsim.EvalResult>`
     - .. autodoc2-docstring:: hlsfactory.flow_lightningsim.EvalResult
          :summary:
   * - :py:obj:`LSEnv <hlsfactory.flow_lightningsim.LSEnv>`
     - .. autodoc2-docstring:: hlsfactory.flow_lightningsim.LSEnv
          :summary:
   * - :py:obj:`LightningSimFlow <hlsfactory.flow_lightningsim.LightningSimFlow>`
     -

API
~~~

.. py:class:: EvalResult
   :canonical: hlsfactory.flow_lightningsim.EvalResult

   .. autodoc2-docstring:: hlsfactory.flow_lightningsim.EvalResult

   .. py:attribute:: fifo_sizes
      :canonical: hlsfactory.flow_lightningsim.EvalResult.fifo_sizes
      :type: dict[int, int]
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_lightningsim.EvalResult.fifo_sizes

   .. py:attribute:: deadlock
      :canonical: hlsfactory.flow_lightningsim.EvalResult.deadlock
      :type: bool
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_lightningsim.EvalResult.deadlock

   .. py:attribute:: latency
      :canonical: hlsfactory.flow_lightningsim.EvalResult.latency
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_lightningsim.EvalResult.latency

   .. py:attribute:: bram_usage_total
      :canonical: hlsfactory.flow_lightningsim.EvalResult.bram_usage_total
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_lightningsim.EvalResult.bram_usage_total

   .. py:attribute:: timestamp
      :canonical: hlsfactory.flow_lightningsim.EvalResult.timestamp
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_lightningsim.EvalResult.timestamp

.. py:class:: LSEnv(vitis_hls_solution_dir: pathlib.Path, env_vars_extra: dict[str, str] | None = None)
   :canonical: hlsfactory.flow_lightningsim.LSEnv

   .. autodoc2-docstring:: hlsfactory.flow_lightningsim.LSEnv

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.flow_lightningsim.LSEnv.__init__

   .. py:method:: eval_solution_single(x: dict[int, int]) -> hlsfactory.flow_lightningsim.EvalResult
      :canonical: hlsfactory.flow_lightningsim.LSEnv.eval_solution_single

      .. autodoc2-docstring:: hlsfactory.flow_lightningsim.LSEnv.eval_solution_single

   .. py:method:: eval_solution_default() -> hlsfactory.flow_lightningsim.EvalResult
      :canonical: hlsfactory.flow_lightningsim.LSEnv.eval_solution_default

      .. autodoc2-docstring:: hlsfactory.flow_lightningsim.LSEnv.eval_solution_default

.. py:class:: LightningSimFlow(log_execution_time: bool = True)
   :canonical: hlsfactory.flow_lightningsim.LightningSimFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: name
      :canonical: hlsfactory.flow_lightningsim.LightningSimFlow.name
      :value: 'LightningSimFlow'

      .. autodoc2-docstring:: hlsfactory.flow_lightningsim.LightningSimFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_lightningsim.LightningSimFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_lightningsim.LightningSimFlow.execute
