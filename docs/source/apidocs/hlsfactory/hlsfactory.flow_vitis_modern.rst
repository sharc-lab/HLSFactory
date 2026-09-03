:py:mod:`hlsfactory.flow_vitis_modern`
======================================

.. py:module:: hlsfactory.flow_vitis_modern

.. autodoc2-docstring:: hlsfactory.flow_vitis_modern
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`VitisHLSModernSynthFlow <hlsfactory.flow_vitis_modern.VitisHLSModernSynthFlow>`
     -
   * - :py:obj:`VitisHLSModernCsimFlow <hlsfactory.flow_vitis_modern.VitisHLSModernCsimFlow>`
     -

Data
~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`HLS_COMPONENT_DIR_NAME <hlsfactory.flow_vitis_modern.HLS_COMPONENT_DIR_NAME>`
     - .. autodoc2-docstring:: hlsfactory.flow_vitis_modern.HLS_COMPONENT_DIR_NAME
          :summary:

API
~~~

.. py:data:: HLS_COMPONENT_DIR_NAME
   :canonical: hlsfactory.flow_vitis_modern.HLS_COMPONENT_DIR_NAME
   :value: 'hls_component'

   .. autodoc2-docstring:: hlsfactory.flow_vitis_modern.HLS_COMPONENT_DIR_NAME

.. py:class:: VitisHLSModernSynthFlow(vpp_bin: str | None = None, log_output: bool = False, log_execution_time: bool = True, env_var_xilinx_hls: str | None = None, env_var_xilinx_vitis: str | None = None, env_var_xilinx_vivado: str | None = None)
   :canonical: hlsfactory.flow_vitis_modern.VitisHLSModernSynthFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: name
      :canonical: hlsfactory.flow_vitis_modern.VitisHLSModernSynthFlow.name
      :value: 'VitisHLSModernSynthFlow'

      .. autodoc2-docstring:: hlsfactory.flow_vitis_modern.VitisHLSModernSynthFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_vitis_modern.VitisHLSModernSynthFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_vitis_modern.VitisHLSModernSynthFlow.execute

.. py:class:: VitisHLSModernCsimFlow(vitis_run_bin: str | None = None, log_output: bool = False, log_execution_time: bool = True, env_var_xilinx_hls: str | None = None, env_var_xilinx_vitis: str | None = None, env_var_xilinx_vivado: str | None = None)
   :canonical: hlsfactory.flow_vitis_modern.VitisHLSModernCsimFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: name
      :canonical: hlsfactory.flow_vitis_modern.VitisHLSModernCsimFlow.name
      :value: 'VitisHLSModernCsimFlow'

      .. autodoc2-docstring:: hlsfactory.flow_vitis_modern.VitisHLSModernCsimFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_vitis_modern.VitisHLSModernCsimFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_vitis_modern.VitisHLSModernCsimFlow.execute
