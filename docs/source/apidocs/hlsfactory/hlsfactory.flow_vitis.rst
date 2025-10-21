:py:mod:`hlsfactory.flow_vitis`
===============================

.. py:module:: hlsfactory.flow_vitis

.. autodoc2-docstring:: hlsfactory.flow_vitis
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`DesignHLSSynthData <hlsfactory.flow_vitis.DesignHLSSynthData>`
     - .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData
          :summary:
   * - :py:obj:`VitisHLSDesign <hlsfactory.flow_vitis.VitisHLSDesign>`
     - .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSDesign
          :summary:
   * - :py:obj:`VitisHLSSynthFlow <hlsfactory.flow_vitis.VitisHLSSynthFlow>`
     -
   * - :py:obj:`VitisHLSCosimSetupFlow <hlsfactory.flow_vitis.VitisHLSCosimSetupFlow>`
     -
   * - :py:obj:`VitisHLSCosimFlow <hlsfactory.flow_vitis.VitisHLSCosimFlow>`
     -
   * - :py:obj:`VitisHLSCsimFlow <hlsfactory.flow_vitis.VitisHLSCsimFlow>`
     -
   * - :py:obj:`VitisHLSImplFlow <hlsfactory.flow_vitis.VitisHLSImplFlow>`
     -
   * - :py:obj:`VitisHLSImplReportFlow <hlsfactory.flow_vitis.VitisHLSImplReportFlow>`
     -

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`print_xml_element <hlsfactory.flow_vitis.print_xml_element>`
     - .. autodoc2-docstring:: hlsfactory.flow_vitis.print_xml_element
          :summary:
   * - :py:obj:`auto_find_solutions <hlsfactory.flow_vitis.auto_find_solutions>`
     - .. autodoc2-docstring:: hlsfactory.flow_vitis.auto_find_solutions
          :summary:
   * - :py:obj:`auto_find_synth_report <hlsfactory.flow_vitis.auto_find_synth_report>`
     - .. autodoc2-docstring:: hlsfactory.flow_vitis.auto_find_synth_report
          :summary:
   * - :py:obj:`auto_find_multiple_synth_report <hlsfactory.flow_vitis.auto_find_multiple_synth_report>`
     - .. autodoc2-docstring:: hlsfactory.flow_vitis.auto_find_multiple_synth_report
          :summary:
   * - :py:obj:`auto_find_exported_ip <hlsfactory.flow_vitis.auto_find_exported_ip>`
     - .. autodoc2-docstring:: hlsfactory.flow_vitis.auto_find_exported_ip
          :summary:
   * - :py:obj:`check_build_files_exist <hlsfactory.flow_vitis.check_build_files_exist>`
     - .. autodoc2-docstring:: hlsfactory.flow_vitis.check_build_files_exist
          :summary:
   * - :py:obj:`check_files_exist <hlsfactory.flow_vitis.check_files_exist>`
     - .. autodoc2-docstring:: hlsfactory.flow_vitis.check_files_exist
          :summary:
   * - :py:obj:`warn_for_reset_flags <hlsfactory.flow_vitis.warn_for_reset_flags>`
     - .. autodoc2-docstring:: hlsfactory.flow_vitis.warn_for_reset_flags
          :summary:

API
~~~

.. py:function:: print_xml_element(node: xml.etree.ElementTree.Element) -> None
   :canonical: hlsfactory.flow_vitis.print_xml_element

   .. autodoc2-docstring:: hlsfactory.flow_vitis.print_xml_element

.. py:function:: auto_find_solutions(dir_path: pathlib.Path) -> list[pathlib.Path]
   :canonical: hlsfactory.flow_vitis.auto_find_solutions

   .. autodoc2-docstring:: hlsfactory.flow_vitis.auto_find_solutions

.. py:function:: auto_find_synth_report(dir_path: pathlib.Path) -> pathlib.Path
   :canonical: hlsfactory.flow_vitis.auto_find_synth_report

   .. autodoc2-docstring:: hlsfactory.flow_vitis.auto_find_synth_report

.. py:function:: auto_find_multiple_synth_report(dir_path: pathlib.Path) -> list[pathlib.Path]
   :canonical: hlsfactory.flow_vitis.auto_find_multiple_synth_report

   .. autodoc2-docstring:: hlsfactory.flow_vitis.auto_find_multiple_synth_report

.. py:class:: DesignHLSSynthData
   :canonical: hlsfactory.flow_vitis.DesignHLSSynthData

   .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData

   .. py:attribute:: clock_period
      :canonical: hlsfactory.flow_vitis.DesignHLSSynthData.clock_period
      :type: float
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData.clock_period

   .. py:attribute:: latency_best_cycles
      :canonical: hlsfactory.flow_vitis.DesignHLSSynthData.latency_best_cycles
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData.latency_best_cycles

   .. py:attribute:: latency_best_seconds
      :canonical: hlsfactory.flow_vitis.DesignHLSSynthData.latency_best_seconds
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData.latency_best_seconds

   .. py:attribute:: latency_average_cycles
      :canonical: hlsfactory.flow_vitis.DesignHLSSynthData.latency_average_cycles
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData.latency_average_cycles

   .. py:attribute:: latency_average_seconds
      :canonical: hlsfactory.flow_vitis.DesignHLSSynthData.latency_average_seconds
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData.latency_average_seconds

   .. py:attribute:: latency_worst_cycles
      :canonical: hlsfactory.flow_vitis.DesignHLSSynthData.latency_worst_cycles
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData.latency_worst_cycles

   .. py:attribute:: latency_worst_seconds
      :canonical: hlsfactory.flow_vitis.DesignHLSSynthData.latency_worst_seconds
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData.latency_worst_seconds

   .. py:attribute:: resources_lut_used
      :canonical: hlsfactory.flow_vitis.DesignHLSSynthData.resources_lut_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData.resources_lut_used

   .. py:attribute:: resources_ff_used
      :canonical: hlsfactory.flow_vitis.DesignHLSSynthData.resources_ff_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData.resources_ff_used

   .. py:attribute:: resources_dsp_used
      :canonical: hlsfactory.flow_vitis.DesignHLSSynthData.resources_dsp_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData.resources_dsp_used

   .. py:attribute:: resources_bram_used
      :canonical: hlsfactory.flow_vitis.DesignHLSSynthData.resources_bram_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData.resources_bram_used

   .. py:attribute:: resources_uram_used
      :canonical: hlsfactory.flow_vitis.DesignHLSSynthData.resources_uram_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData.resources_uram_used

   .. py:method:: parse_from_synth_report_file(fp: pathlib.Path) -> hlsfactory.flow_vitis.DesignHLSSynthData
      :canonical: hlsfactory.flow_vitis.DesignHLSSynthData.parse_from_synth_report_file
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.flow_vitis.DesignHLSSynthData.parse_from_synth_report_file

.. py:class:: VitisHLSDesign
   :canonical: hlsfactory.flow_vitis.VitisHLSDesign

   .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSDesign

   .. py:attribute:: name
      :canonical: hlsfactory.flow_vitis.VitisHLSDesign.name
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSDesign.name

   .. py:attribute:: part
      :canonical: hlsfactory.flow_vitis.VitisHLSDesign.part
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSDesign.part

   .. py:attribute:: target_clock_period
      :canonical: hlsfactory.flow_vitis.VitisHLSDesign.target_clock_period
      :type: float
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSDesign.target_clock_period

   .. py:attribute:: version_vitis_hls
      :canonical: hlsfactory.flow_vitis.VitisHLSDesign.version_vitis_hls
      :type: None | str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSDesign.version_vitis_hls

   .. py:attribute:: version_vivado
      :canonical: hlsfactory.flow_vitis.VitisHLSDesign.version_vivado
      :type: None | str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSDesign.version_vivado

   .. py:method:: parse_from_synth_report_file(fp: pathlib.Path) -> hlsfactory.flow_vitis.VitisHLSDesign
      :canonical: hlsfactory.flow_vitis.VitisHLSDesign.parse_from_synth_report_file
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSDesign.parse_from_synth_report_file

.. py:function:: auto_find_exported_ip(dir_path: pathlib.Path) -> list[pathlib.Path]
   :canonical: hlsfactory.flow_vitis.auto_find_exported_ip

   .. autodoc2-docstring:: hlsfactory.flow_vitis.auto_find_exported_ip

.. py:function:: check_build_files_exist(build_files: list[pathlib.Path]) -> None
   :canonical: hlsfactory.flow_vitis.check_build_files_exist

   .. autodoc2-docstring:: hlsfactory.flow_vitis.check_build_files_exist

.. py:function:: check_files_exist(files: list[pathlib.Path]) -> None
   :canonical: hlsfactory.flow_vitis.check_files_exist

   .. autodoc2-docstring:: hlsfactory.flow_vitis.check_files_exist

.. py:function:: warn_for_reset_flags(files: list[pathlib.Path], reset_flag_str: str = '-reset') -> None
   :canonical: hlsfactory.flow_vitis.warn_for_reset_flags

   .. autodoc2-docstring:: hlsfactory.flow_vitis.warn_for_reset_flags

.. py:class:: VitisHLSSynthFlow(vitis_hls_bin: str | None = None, log_output: bool = False, log_execution_time: bool = True, env_var_xilinx_hls: str | None = None, env_var_xilinx_vivado: str | None = None)
   :canonical: hlsfactory.flow_vitis.VitisHLSSynthFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: name
      :canonical: hlsfactory.flow_vitis.VitisHLSSynthFlow.name
      :value: 'VitisHLSSynthFlow'

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSSynthFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_vitis.VitisHLSSynthFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSSynthFlow.execute

.. py:class:: VitisHLSCosimSetupFlow(vitis_hls_bin: str | None = None, log_output: bool = False)
   :canonical: hlsfactory.flow_vitis.VitisHLSCosimSetupFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: name
      :canonical: hlsfactory.flow_vitis.VitisHLSCosimSetupFlow.name
      :value: 'VitisHLSCosimSetupFlow'

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSCosimSetupFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_vitis.VitisHLSCosimSetupFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSCosimSetupFlow.execute

.. py:class:: VitisHLSCosimFlow(vitis_hls_bin: str | None = None, log_output: bool = False)
   :canonical: hlsfactory.flow_vitis.VitisHLSCosimFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: name
      :canonical: hlsfactory.flow_vitis.VitisHLSCosimFlow.name
      :value: 'VitisHLSCosimFlow'

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSCosimFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_vitis.VitisHLSCosimFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSCosimFlow.execute

.. py:class:: VitisHLSCsimFlow(vitis_hls_bin: str | None = None, log_output: bool = False, env_var_xilinx_hls: str | None = None, env_var_xilinx_vivado: str | None = None)
   :canonical: hlsfactory.flow_vitis.VitisHLSCsimFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: name
      :canonical: hlsfactory.flow_vitis.VitisHLSCsimFlow.name
      :value: 'VitisHLSCsimFlow'

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSCsimFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_vitis.VitisHLSCsimFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSCsimFlow.execute

.. py:class:: VitisHLSImplFlow(vitis_hls_bin: str | None = None, log_output: bool = False, env_var_xilinx_hls: str | None = None, env_var_xilinx_vivado: str | None = None)
   :canonical: hlsfactory.flow_vitis.VitisHLSImplFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: name
      :canonical: hlsfactory.flow_vitis.VitisHLSImplFlow.name
      :value: 'VitisHLSImplFlow'

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSImplFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_vitis.VitisHLSImplFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSImplFlow.execute

.. py:class:: VitisHLSImplReportFlow(vitis_hls_bin: str | None = None, vivado_bin: str | None = None, log_output: bool = False, env_var_xilinx_hls: str | None = None, env_var_xilinx_vivado: str | None = None)
   :canonical: hlsfactory.flow_vitis.VitisHLSImplReportFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: name
      :canonical: hlsfactory.flow_vitis.VitisHLSImplReportFlow.name
      :value: 'VitisHLSImplReportFlow'

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSImplReportFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_vitis.VitisHLSImplReportFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSImplReportFlow.execute

   .. py:method:: parse_all_reports(design_dir: pathlib.Path) -> dict[str, dict]
      :canonical: hlsfactory.flow_vitis.VitisHLSImplReportFlow.parse_all_reports
      :staticmethod:

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSImplReportFlow.parse_all_reports

   .. py:method:: parse_power_report(design_dir: pathlib.Path) -> dict
      :canonical: hlsfactory.flow_vitis.VitisHLSImplReportFlow.parse_power_report
      :staticmethod:

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSImplReportFlow.parse_power_report

   .. py:method:: parse_utilization_report(design_dir: pathlib.Path) -> dict
      :canonical: hlsfactory.flow_vitis.VitisHLSImplReportFlow.parse_utilization_report
      :staticmethod:

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSImplReportFlow.parse_utilization_report

   .. py:method:: parse_timing_report(design_dir: pathlib.Path) -> dict[str, float | int | str]
      :canonical: hlsfactory.flow_vitis.VitisHLSImplReportFlow.parse_timing_report
      :staticmethod:

      .. autodoc2-docstring:: hlsfactory.flow_vitis.VitisHLSImplReportFlow.parse_timing_report
