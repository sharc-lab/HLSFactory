:py:mod:`hlsfactory.flow_catapult`
==================================

.. py:module:: hlsfactory.flow_catapult

.. autodoc2-docstring:: hlsfactory.flow_catapult
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`DesignHLSSynthData <hlsfactory.flow_catapult.DesignHLSSynthData>`
     - .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData
          :summary:
   * - :py:obj:`CatapultHLSSynthFlow <hlsfactory.flow_catapult.CatapultHLSSynthFlow>`
     - .. autodoc2-docstring:: hlsfactory.flow_catapult.CatapultHLSSynthFlow
          :summary:

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`get_catapult_bin <hlsfactory.flow_catapult.get_catapult_bin>`
     - .. autodoc2-docstring:: hlsfactory.flow_catapult.get_catapult_bin
          :summary:
   * - :py:obj:`auto_find_synth_report <hlsfactory.flow_catapult.auto_find_synth_report>`
     - .. autodoc2-docstring:: hlsfactory.flow_catapult.auto_find_synth_report
          :summary:
   * - :py:obj:`auto_find_cycle_report <hlsfactory.flow_catapult.auto_find_cycle_report>`
     - .. autodoc2-docstring:: hlsfactory.flow_catapult.auto_find_cycle_report
          :summary:
   * - :py:obj:`_parse_post_assignment_area <hlsfactory.flow_catapult._parse_post_assignment_area>`
     - .. autodoc2-docstring:: hlsfactory.flow_catapult._parse_post_assignment_area
          :summary:

Data
~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`HLSFACTORY_CATAPULT_PATH_ENV_VAR <hlsfactory.flow_catapult.HLSFACTORY_CATAPULT_PATH_ENV_VAR>`
     - .. autodoc2-docstring:: hlsfactory.flow_catapult.HLSFACTORY_CATAPULT_PATH_ENV_VAR
          :summary:
   * - :py:obj:`_NUMBER_PATTERN <hlsfactory.flow_catapult._NUMBER_PATTERN>`
     - .. autodoc2-docstring:: hlsfactory.flow_catapult._NUMBER_PATTERN
          :summary:
   * - :py:obj:`_TOTAL_AREA_PATTERNS <hlsfactory.flow_catapult._TOTAL_AREA_PATTERNS>`
     - .. autodoc2-docstring:: hlsfactory.flow_catapult._TOTAL_AREA_PATTERNS
          :summary:
   * - :py:obj:`_TOTAL_AREA_BREAKDOWN_PATTERN <hlsfactory.flow_catapult._TOTAL_AREA_BREAKDOWN_PATTERN>`
     - .. autodoc2-docstring:: hlsfactory.flow_catapult._TOTAL_AREA_BREAKDOWN_PATTERN
          :summary:
   * - :py:obj:`__all__ <hlsfactory.flow_catapult.__all__>`
     - .. autodoc2-docstring:: hlsfactory.flow_catapult.__all__
          :summary:

API
~~~

.. py:data:: HLSFACTORY_CATAPULT_PATH_ENV_VAR
   :canonical: hlsfactory.flow_catapult.HLSFACTORY_CATAPULT_PATH_ENV_VAR
   :value: 'HLSFACTORY_CATAPULT_PATH'

   .. autodoc2-docstring:: hlsfactory.flow_catapult.HLSFACTORY_CATAPULT_PATH_ENV_VAR

.. py:data:: _NUMBER_PATTERN
   :canonical: hlsfactory.flow_catapult._NUMBER_PATTERN
   :value: '(?:[0-9]+(?:\\.[0-9]*)?|\\.[0-9]+)(?:[eE][+-]?\\d+)?'

   .. autodoc2-docstring:: hlsfactory.flow_catapult._NUMBER_PATTERN

.. py:data:: _TOTAL_AREA_PATTERNS
   :canonical: hlsfactory.flow_catapult._TOTAL_AREA_PATTERNS
   :value: ()

   .. autodoc2-docstring:: hlsfactory.flow_catapult._TOTAL_AREA_PATTERNS

.. py:data:: _TOTAL_AREA_BREAKDOWN_PATTERN
   :canonical: hlsfactory.flow_catapult._TOTAL_AREA_BREAKDOWN_PATTERN
   :value: 'compile(...)'

   .. autodoc2-docstring:: hlsfactory.flow_catapult._TOTAL_AREA_BREAKDOWN_PATTERN

.. py:function:: get_catapult_bin(catapult_bin: str | pathlib.Path | None = None) -> str
   :canonical: hlsfactory.flow_catapult.get_catapult_bin

   .. autodoc2-docstring:: hlsfactory.flow_catapult.get_catapult_bin

.. py:function:: auto_find_synth_report(dir_path: pathlib.Path) -> pathlib.Path
   :canonical: hlsfactory.flow_catapult.auto_find_synth_report

   .. autodoc2-docstring:: hlsfactory.flow_catapult.auto_find_synth_report

.. py:function:: auto_find_cycle_report(dir_path: pathlib.Path) -> pathlib.Path
   :canonical: hlsfactory.flow_catapult.auto_find_cycle_report

   .. autodoc2-docstring:: hlsfactory.flow_catapult.auto_find_cycle_report

.. py:function:: _parse_post_assignment_area(report_text: str, label: str) -> float | None
   :canonical: hlsfactory.flow_catapult._parse_post_assignment_area

   .. autodoc2-docstring:: hlsfactory.flow_catapult._parse_post_assignment_area

.. py:class:: DesignHLSSynthData
   :canonical: hlsfactory.flow_catapult.DesignHLSSynthData

   .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData

   .. py:attribute:: total_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.total_area
      :type: float
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.total_area

   .. py:attribute:: tool_version
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.tool_version
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.tool_version

   .. py:attribute:: clock_signal
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.clock_signal
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.clock_signal

   .. py:attribute:: clock_edge
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.clock_edge
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.clock_edge

   .. py:attribute:: clock_period
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.clock_period
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.clock_period

   .. py:attribute:: clock_period_ns
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.clock_period_ns
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.clock_period_ns

   .. py:attribute:: clock_uncertainty_ns
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.clock_uncertainty_ns
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.clock_uncertainty_ns

   .. py:attribute:: clock_allocation_percent
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.clock_allocation_percent
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.clock_allocation_percent

   .. py:attribute:: real_operation_count
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.real_operation_count
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.real_operation_count

   .. py:attribute:: latency_cycles
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.latency_cycles
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.latency_cycles

   .. py:attribute:: latency_seconds
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.latency_seconds
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.latency_seconds

   .. py:attribute:: throughput_cycles
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.throughput_cycles
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.throughput_cycles

   .. py:attribute:: throughput_seconds
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.throughput_seconds
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.throughput_seconds

   .. py:attribute:: reset_length_cycles
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.reset_length_cycles
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.reset_length_cycles

   .. py:attribute:: initiation_interval_cycles
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.initiation_interval_cycles
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.initiation_interval_cycles

   .. py:attribute:: combinational_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.combinational_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.combinational_area

   .. py:attribute:: raw_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.raw_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.raw_area

   .. py:attribute:: sequential_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.sequential_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.sequential_area

   .. py:attribute:: total_area_score
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.total_area_score
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.total_area_score

   .. py:attribute:: total_register_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.total_register_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.total_register_area

   .. py:attribute:: datapath_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.datapath_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.datapath_area

   .. py:attribute:: mux_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.mux_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.mux_area

   .. py:attribute:: functional_unit_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.functional_unit_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.functional_unit_area

   .. py:attribute:: logic_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.logic_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.logic_area

   .. py:attribute:: buffer_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.buffer_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.buffer_area

   .. py:attribute:: memory_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.memory_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.memory_area

   .. py:attribute:: rom_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.rom_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.rom_area

   .. py:attribute:: register_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.register_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.register_area

   .. py:attribute:: fsm_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.fsm_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.fsm_area

   .. py:attribute:: fsm_register_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.fsm_register_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.fsm_register_area

   .. py:attribute:: fsm_combinational_area
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.fsm_combinational_area
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.fsm_combinational_area

   .. py:attribute:: critical_path_delay_ns
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.critical_path_delay_ns
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.critical_path_delay_ns

   .. py:attribute:: critical_path_slack_ns
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.critical_path_slack_ns
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.critical_path_slack_ns

   .. py:method:: parse_from_synth_report_file(report_file: pathlib.Path, cycle_report_file: pathlib.Path | None = None) -> hlsfactory.flow_catapult.DesignHLSSynthData
      :canonical: hlsfactory.flow_catapult.DesignHLSSynthData.parse_from_synth_report_file
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.flow_catapult.DesignHLSSynthData.parse_from_synth_report_file

.. py:class:: CatapultHLSSynthFlow(catapult_bin: str | pathlib.Path | None = None, log_output: bool = False, log_execution_time: bool = True)
   :canonical: hlsfactory.flow_catapult.CatapultHLSSynthFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. autodoc2-docstring:: hlsfactory.flow_catapult.CatapultHLSSynthFlow

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.flow_catapult.CatapultHLSSynthFlow.__init__

   .. py:attribute:: name
      :canonical: hlsfactory.flow_catapult.CatapultHLSSynthFlow.name
      :value: 'CatapultHLSSynthFlow'

      .. autodoc2-docstring:: hlsfactory.flow_catapult.CatapultHLSSynthFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_catapult.CatapultHLSSynthFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_catapult.CatapultHLSSynthFlow.execute

   .. py:method:: _log_execution_time(design_dir: pathlib.Path, start_time: float) -> None
      :canonical: hlsfactory.flow_catapult.CatapultHLSSynthFlow._log_execution_time

      .. autodoc2-docstring:: hlsfactory.flow_catapult.CatapultHLSSynthFlow._log_execution_time

.. py:data:: __all__
   :canonical: hlsfactory.flow_catapult.__all__
   :value: ['CatapultHLSSynthFlow', 'DesignHLSSynthData', 'HLSFACTORY_CATAPULT_PATH_ENV_VAR', 'auto_find_cycle_...

   .. autodoc2-docstring:: hlsfactory.flow_catapult.__all__
