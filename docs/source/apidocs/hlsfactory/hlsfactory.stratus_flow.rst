:py:mod:`hlsfactory.stratus_flow`
=================================

.. py:module:: hlsfactory.stratus_flow

.. autodoc2-docstring:: hlsfactory.stratus_flow
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`DesignHLSSynthData <hlsfactory.stratus_flow.DesignHLSSynthData>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow.DesignHLSSynthData
          :summary:
   * - :py:obj:`StratusHLSDesign <hlsfactory.stratus_flow.StratusHLSDesign>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSDesign
          :summary:
   * - :py:obj:`StratusHLSSynthFlow <hlsfactory.stratus_flow.StratusHLSSynthFlow>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSSynthFlow
          :summary:

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`_parse_metric_value <hlsfactory.stratus_flow._parse_metric_value>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow._parse_metric_value
          :summary:
   * - :py:obj:`parse_metrics_file <hlsfactory.stratus_flow.parse_metrics_file>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow.parse_metrics_file
          :summary:
   * - :py:obj:`parse_area_metrics_from_synth_log <hlsfactory.stratus_flow.parse_area_metrics_from_synth_log>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow.parse_area_metrics_from_synth_log
          :summary:
   * - :py:obj:`auto_find_hls_output_dir <hlsfactory.stratus_flow.auto_find_hls_output_dir>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow.auto_find_hls_output_dir
          :summary:
   * - :py:obj:`_require_generated_rtl <hlsfactory.stratus_flow._require_generated_rtl>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow._require_generated_rtl
          :summary:
   * - :py:obj:`_tcl_quote <hlsfactory.stratus_flow._tcl_quote>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow._tcl_quote
          :summary:
   * - :py:obj:`_metrics_tcl <hlsfactory.stratus_flow._metrics_tcl>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow._metrics_tcl
          :summary:
   * - :py:obj:`_stratus_environment <hlsfactory.stratus_flow._stratus_environment>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow._stratus_environment
          :summary:

Data
~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`_METRICS_TCL_FILENAME <hlsfactory.stratus_flow._METRICS_TCL_FILENAME>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow._METRICS_TCL_FILENAME
          :summary:
   * - :py:obj:`_METRICS_TSV_FILENAME <hlsfactory.stratus_flow._METRICS_TSV_FILENAME>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow._METRICS_TSV_FILENAME
          :summary:
   * - :py:obj:`MetricValue <hlsfactory.stratus_flow.MetricValue>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow.MetricValue
          :summary:
   * - :py:obj:`_NUMBER_PATTERN <hlsfactory.stratus_flow._NUMBER_PATTERN>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow._NUMBER_PATTERN
          :summary:
   * - :py:obj:`_TOTAL_AREA_PATTERN <hlsfactory.stratus_flow._TOTAL_AREA_PATTERN>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow._TOTAL_AREA_PATTERN
          :summary:
   * - :py:obj:`__all__ <hlsfactory.stratus_flow.__all__>`
     - .. autodoc2-docstring:: hlsfactory.stratus_flow.__all__
          :summary:

API
~~~

.. py:data:: _METRICS_TCL_FILENAME
   :canonical: hlsfactory.stratus_flow._METRICS_TCL_FILENAME
   :value: '.hlsfactory_stratus_metrics.tcl'

   .. autodoc2-docstring:: hlsfactory.stratus_flow._METRICS_TCL_FILENAME

.. py:data:: _METRICS_TSV_FILENAME
   :canonical: hlsfactory.stratus_flow._METRICS_TSV_FILENAME
   :value: '.hlsfactory_stratus_metrics.tsv'

   .. autodoc2-docstring:: hlsfactory.stratus_flow._METRICS_TSV_FILENAME

.. py:data:: MetricValue
   :canonical: hlsfactory.stratus_flow.MetricValue
   :value: None

   .. autodoc2-docstring:: hlsfactory.stratus_flow.MetricValue

.. py:data:: _NUMBER_PATTERN
   :canonical: hlsfactory.stratus_flow._NUMBER_PATTERN
   :value: '[0-9]+(?:\\.[0-9]*)?(?:[eE][+-]?[0-9]+)?|\\.[0-9]+(?:[eE][+-]?[0-9]+)?'

   .. autodoc2-docstring:: hlsfactory.stratus_flow._NUMBER_PATTERN

.. py:data:: _TOTAL_AREA_PATTERN
   :canonical: hlsfactory.stratus_flow._TOTAL_AREA_PATTERN
   :value: 'compile(...)'

   .. autodoc2-docstring:: hlsfactory.stratus_flow._TOTAL_AREA_PATTERN

.. py:function:: _parse_metric_value(value: str) -> hlsfactory.stratus_flow.MetricValue
   :canonical: hlsfactory.stratus_flow._parse_metric_value

   .. autodoc2-docstring:: hlsfactory.stratus_flow._parse_metric_value

.. py:function:: parse_metrics_file(metrics_file: pathlib.Path) -> dict[str, hlsfactory.stratus_flow.MetricValue]
   :canonical: hlsfactory.stratus_flow.parse_metrics_file

   .. autodoc2-docstring:: hlsfactory.stratus_flow.parse_metrics_file

.. py:function:: parse_area_metrics_from_synth_log(synth_log: pathlib.Path) -> dict[str, hlsfactory.stratus_flow.MetricValue]
   :canonical: hlsfactory.stratus_flow.parse_area_metrics_from_synth_log

   .. autodoc2-docstring:: hlsfactory.stratus_flow.parse_area_metrics_from_synth_log

.. py:function:: auto_find_hls_output_dir(design_dir: pathlib.Path, hls_module: str, hls_config: str) -> pathlib.Path
   :canonical: hlsfactory.stratus_flow.auto_find_hls_output_dir

   .. autodoc2-docstring:: hlsfactory.stratus_flow.auto_find_hls_output_dir

.. py:function:: _require_generated_rtl(output_dir: pathlib.Path) -> pathlib.Path
   :canonical: hlsfactory.stratus_flow._require_generated_rtl

   .. autodoc2-docstring:: hlsfactory.stratus_flow._require_generated_rtl

.. py:function:: _tcl_quote(value: str) -> str
   :canonical: hlsfactory.stratus_flow._tcl_quote

   .. autodoc2-docstring:: hlsfactory.stratus_flow._tcl_quote

.. py:function:: _metrics_tcl(project_tcl: pathlib.Path, metrics_file: pathlib.Path, hls_module: str, hls_config: str) -> str
   :canonical: hlsfactory.stratus_flow._metrics_tcl

   .. autodoc2-docstring:: hlsfactory.stratus_flow._metrics_tcl

.. py:class:: DesignHLSSynthData
   :canonical: hlsfactory.stratus_flow.DesignHLSSynthData

   .. autodoc2-docstring:: hlsfactory.stratus_flow.DesignHLSSynthData

   .. py:attribute:: total_area
      :canonical: hlsfactory.stratus_flow.DesignHLSSynthData.total_area
      :type: float
      :value: None

      .. autodoc2-docstring:: hlsfactory.stratus_flow.DesignHLSSynthData.total_area

   .. py:attribute:: combinational_area
      :canonical: hlsfactory.stratus_flow.DesignHLSSynthData.combinational_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.stratus_flow.DesignHLSSynthData.combinational_area

   .. py:attribute:: sequential_area
      :canonical: hlsfactory.stratus_flow.DesignHLSSynthData.sequential_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.stratus_flow.DesignHLSSynthData.sequential_area

   .. py:attribute:: latency_total_cycles
      :canonical: hlsfactory.stratus_flow.DesignHLSSynthData.latency_total_cycles
      :type: int | float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.stratus_flow.DesignHLSSynthData.latency_total_cycles

   .. py:attribute:: latency_cycles
      :canonical: hlsfactory.stratus_flow.DesignHLSSynthData.latency_cycles
      :type: dict[str, int | float]
      :value: None

      .. autodoc2-docstring:: hlsfactory.stratus_flow.DesignHLSSynthData.latency_cycles

   .. py:attribute:: metrics
      :canonical: hlsfactory.stratus_flow.DesignHLSSynthData.metrics
      :type: dict[str, hlsfactory.stratus_flow.MetricValue]
      :value: None

      .. autodoc2-docstring:: hlsfactory.stratus_flow.DesignHLSSynthData.metrics

   .. py:method:: parse_from_metrics_file(metrics_file: pathlib.Path) -> hlsfactory.stratus_flow.DesignHLSSynthData
      :canonical: hlsfactory.stratus_flow.DesignHLSSynthData.parse_from_metrics_file
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.stratus_flow.DesignHLSSynthData.parse_from_metrics_file

   .. py:method:: parse_from_metrics(metrics: dict[str, hlsfactory.stratus_flow.MetricValue], *, source: str) -> hlsfactory.stratus_flow.DesignHLSSynthData
      :canonical: hlsfactory.stratus_flow.DesignHLSSynthData.parse_from_metrics
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.stratus_flow.DesignHLSSynthData.parse_from_metrics

.. py:class:: StratusHLSDesign
   :canonical: hlsfactory.stratus_flow.StratusHLSDesign

   .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSDesign

   .. py:attribute:: name
      :canonical: hlsfactory.stratus_flow.StratusHLSDesign.name
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSDesign.name

   .. py:attribute:: hls_config
      :canonical: hlsfactory.stratus_flow.StratusHLSDesign.hls_config
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSDesign.hls_config

   .. py:attribute:: target_clock_period
      :canonical: hlsfactory.stratus_flow.StratusHLSDesign.target_clock_period
      :type: float
      :value: None

      .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSDesign.target_clock_period

   .. py:attribute:: version_stratus
      :canonical: hlsfactory.stratus_flow.StratusHLSDesign.version_stratus
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSDesign.version_stratus

   .. py:method:: parse_from_metrics_file(metrics_file: pathlib.Path, *, hls_module: str, hls_config: str) -> hlsfactory.stratus_flow.StratusHLSDesign
      :canonical: hlsfactory.stratus_flow.StratusHLSDesign.parse_from_metrics_file
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSDesign.parse_from_metrics_file

.. py:function:: _stratus_environment(*tool_paths: str) -> typing.Iterator[None]
   :canonical: hlsfactory.stratus_flow._stratus_environment

   .. autodoc2-docstring:: hlsfactory.stratus_flow._stratus_environment

.. py:class:: StratusHLSSynthFlow(stratus_install_dir: str | pathlib.Path | None = None, stratus_bin: str | None = None, bdw_makegen_bin: str | None = None, make_bin: str | None = None, log_output: bool = False, log_execution_time: bool = True)
   :canonical: hlsfactory.stratus_flow.StratusHLSSynthFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSSynthFlow

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSSynthFlow.__init__

   .. py:attribute:: name
      :canonical: hlsfactory.stratus_flow.StratusHLSSynthFlow.name
      :value: 'StratusHLSSynthFlow'

      .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSSynthFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.stratus_flow.StratusHLSSynthFlow.execute

      .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSSynthFlow.execute

   .. py:method:: _handle_tool_failure(design_dir: pathlib.Path, result: hlsfactory.utils.CallToolResult, timeout: float | None, start_time: float, stage: str) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.stratus_flow.StratusHLSSynthFlow._handle_tool_failure

      .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSSynthFlow._handle_tool_failure

   .. py:method:: _write_error(design_dir: pathlib.Path, message: str) -> None
      :canonical: hlsfactory.stratus_flow.StratusHLSSynthFlow._write_error

      .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSSynthFlow._write_error

   .. py:method:: _log_execution_time(design_dir: pathlib.Path, start_time: float, status: hlsfactory.utils.ExecutionDataStatus = ExecutionDataStatus.SUCCESS, error_message: str | None = None) -> None
      :canonical: hlsfactory.stratus_flow.StratusHLSSynthFlow._log_execution_time

      .. autodoc2-docstring:: hlsfactory.stratus_flow.StratusHLSSynthFlow._log_execution_time

.. py:data:: __all__
   :canonical: hlsfactory.stratus_flow.__all__
   :value: ['DesignHLSSynthData', 'StratusHLSDesign', 'StratusHLSSynthFlow', 'auto_find_hls_output_dir', 'parse...

   .. autodoc2-docstring:: hlsfactory.stratus_flow.__all__
