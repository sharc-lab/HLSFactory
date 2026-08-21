:py:mod:`hlsfactory.flow_xls`
=============================

.. py:module:: hlsfactory.flow_xls

.. autodoc2-docstring:: hlsfactory.flow_xls
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`DesignHLSSynthData <hlsfactory.flow_xls.DesignHLSSynthData>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData
          :summary:
   * - :py:obj:`XLSBlockMetrics <hlsfactory.flow_xls.XLSBlockMetrics>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls.XLSBlockMetrics
          :summary:
   * - :py:obj:`XLSHLSSynthFlow <hlsfactory.flow_xls.XLSHLSSynthFlow>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls.XLSHLSSynthFlow
          :summary:

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`get_xls_install_dir <hlsfactory.flow_xls.get_xls_install_dir>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls.get_xls_install_dir
          :summary:
   * - :py:obj:`find_xls_binary <hlsfactory.flow_xls.find_xls_binary>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls.find_xls_binary
          :summary:
   * - :py:obj:`parse_xls_bool_setting <hlsfactory.flow_xls.parse_xls_bool_setting>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls.parse_xls_bool_setting
          :summary:
   * - :py:obj:`_extract_textproto_blocks <hlsfactory.flow_xls._extract_textproto_blocks>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls._extract_textproto_blocks
          :summary:
   * - :py:obj:`_parse_textproto_int <hlsfactory.flow_xls._parse_textproto_int>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls._parse_textproto_int
          :summary:
   * - :py:obj:`_parse_textproto_bool <hlsfactory.flow_xls._parse_textproto_bool>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls._parse_textproto_bool
          :summary:
   * - :py:obj:`_parse_textproto_string <hlsfactory.flow_xls._parse_textproto_string>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls._parse_textproto_string
          :summary:
   * - :py:obj:`_parse_textproto_enum <hlsfactory.flow_xls._parse_textproto_enum>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls._parse_textproto_enum
          :summary:
   * - :py:obj:`parse_xls_module_signature <hlsfactory.flow_xls.parse_xls_module_signature>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls.parse_xls_module_signature
          :summary:
   * - :py:obj:`parse_xls_block_metrics <hlsfactory.flow_xls.parse_xls_block_metrics>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls.parse_xls_block_metrics
          :summary:

Data
~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`HLSFACTORY_XLS_PATH_ENV_VAR <hlsfactory.flow_xls.HLSFACTORY_XLS_PATH_ENV_VAR>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls.HLSFACTORY_XLS_PATH_ENV_VAR
          :summary:
   * - :py:obj:`DEFAULT_XLS_PATH <hlsfactory.flow_xls.DEFAULT_XLS_PATH>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls.DEFAULT_XLS_PATH
          :summary:
   * - :py:obj:`_XLS_BINARY_LOCATIONS <hlsfactory.flow_xls._XLS_BINARY_LOCATIONS>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls._XLS_BINARY_LOCATIONS
          :summary:
   * - :py:obj:`__all__ <hlsfactory.flow_xls.__all__>`
     - .. autodoc2-docstring:: hlsfactory.flow_xls.__all__
          :summary:

API
~~~

.. py:data:: HLSFACTORY_XLS_PATH_ENV_VAR
   :canonical: hlsfactory.flow_xls.HLSFACTORY_XLS_PATH_ENV_VAR
   :value: 'HLSFACTORY_XLS_PATH'

   .. autodoc2-docstring:: hlsfactory.flow_xls.HLSFACTORY_XLS_PATH_ENV_VAR

.. py:data:: DEFAULT_XLS_PATH
   :canonical: hlsfactory.flow_xls.DEFAULT_XLS_PATH
   :value: 'Path(...)'

   .. autodoc2-docstring:: hlsfactory.flow_xls.DEFAULT_XLS_PATH

.. py:data:: _XLS_BINARY_LOCATIONS
   :canonical: hlsfactory.flow_xls._XLS_BINARY_LOCATIONS
   :value: None

   .. autodoc2-docstring:: hlsfactory.flow_xls._XLS_BINARY_LOCATIONS

.. py:function:: get_xls_install_dir(xls_install_dir: str | pathlib.Path | None = None) -> pathlib.Path
   :canonical: hlsfactory.flow_xls.get_xls_install_dir

   .. autodoc2-docstring:: hlsfactory.flow_xls.get_xls_install_dir

.. py:function:: find_xls_binary(xls_install_dir: pathlib.Path, binary_name: str) -> pathlib.Path
   :canonical: hlsfactory.flow_xls.find_xls_binary

   .. autodoc2-docstring:: hlsfactory.flow_xls.find_xls_binary

.. py:function:: parse_xls_bool_setting(value: str | None, setting_name: str) -> bool
   :canonical: hlsfactory.flow_xls.parse_xls_bool_setting

   .. autodoc2-docstring:: hlsfactory.flow_xls.parse_xls_bool_setting

.. py:class:: DesignHLSSynthData
   :canonical: hlsfactory.flow_xls.DesignHLSSynthData

   .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData

   .. py:attribute:: dslx_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.dslx_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.dslx_file

   .. py:attribute:: top
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.top
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.top

   .. py:attribute:: generator
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.generator
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.generator

   .. py:attribute:: pipeline_stages
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.pipeline_stages
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.pipeline_stages

   .. py:attribute:: delay_model
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.delay_model
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.delay_model

   .. py:attribute:: reset
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.reset
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.reset

   .. py:attribute:: ir_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.ir_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.ir_file

   .. py:attribute:: optimized_ir_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.optimized_ir_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.optimized_ir_file

   .. py:attribute:: rtl_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.rtl_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.rtl_file

   .. py:attribute:: module_signature_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.module_signature_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.module_signature_file

   .. py:attribute:: block_metrics_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.block_metrics_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.block_metrics_file

   .. py:attribute:: interface_proto_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.interface_proto_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.interface_proto_file

   .. py:attribute:: interface_textproto_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.interface_textproto_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.interface_textproto_file

   .. py:attribute:: ir_converter_options_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.ir_converter_options_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.ir_converter_options_file

   .. py:attribute:: optimizer_options_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.optimizer_options_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.optimizer_options_file

   .. py:attribute:: optimization_pass_metrics_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.optimization_pass_metrics_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.optimization_pass_metrics_file

   .. py:attribute:: schedule_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.schedule_file
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.schedule_file

   .. py:attribute:: scheduled_ir_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.scheduled_ir_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.scheduled_ir_file

   .. py:attribute:: block_ir_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.block_ir_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.block_ir_file

   .. py:attribute:: verilog_line_map_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.verilog_line_map_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.verilog_line_map_file

   .. py:attribute:: scheduling_options_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.scheduling_options_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.scheduling_options_file

   .. py:attribute:: codegen_options_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.codegen_options_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.codegen_options_file

   .. py:attribute:: scheduling_pass_metrics_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.scheduling_pass_metrics_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.scheduling_pass_metrics_file

   .. py:attribute:: codegen_pass_metrics_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.codegen_pass_metrics_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.codegen_pass_metrics_file

   .. py:attribute:: dump_optimizer_ir
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.dump_optimizer_ir
      :type: bool
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.dump_optimizer_ir

   .. py:attribute:: optimizer_ir_dump_dir
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.optimizer_ir_dump_dir
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.optimizer_ir_dump_dir

   .. py:attribute:: dump_codegen_ir
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.dump_codegen_ir
      :type: bool
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.dump_codegen_ir

   .. py:attribute:: codegen_ir_dump_dir
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.codegen_ir_dump_dir
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.codegen_ir_dump_dir

   .. py:attribute:: profile_passes
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.profile_passes
      :type: bool
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.profile_passes

   .. py:attribute:: optimizer_pass_profile_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.optimizer_pass_profile_file
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.optimizer_pass_profile_file

   .. py:attribute:: codegen_pass_profile_file
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.codegen_pass_profile_file
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.codegen_pass_profile_file

   .. py:attribute:: latency_cycles
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.latency_cycles
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.latency_cycles

   .. py:attribute:: initiation_interval
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.initiation_interval
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.initiation_interval

   .. py:attribute:: flop_count
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.flop_count
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.flop_count

   .. py:attribute:: feedthrough_path_exists
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.feedthrough_path_exists
      :type: bool
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.feedthrough_path_exists

   .. py:attribute:: metrics_delay_model
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.metrics_delay_model
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.metrics_delay_model

   .. py:attribute:: max_reg_to_reg_delay_ps
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.max_reg_to_reg_delay_ps
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.max_reg_to_reg_delay_ps

   .. py:attribute:: max_input_to_reg_delay_ps
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.max_input_to_reg_delay_ps
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.max_input_to_reg_delay_ps

   .. py:attribute:: max_reg_to_output_delay_ps
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.max_reg_to_output_delay_ps
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.max_reg_to_output_delay_ps

   .. py:attribute:: max_feedthrough_path_delay_ps
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.max_feedthrough_path_delay_ps
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.max_feedthrough_path_delay_ps

   .. py:attribute:: operation_counts
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.operation_counts
      :type: dict[str, int]
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.operation_counts

   .. py:attribute:: operation_kind_counts
      :canonical: hlsfactory.flow_xls.DesignHLSSynthData.operation_kind_counts
      :type: dict[str, int]
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.DesignHLSSynthData.operation_kind_counts

.. py:class:: XLSBlockMetrics
   :canonical: hlsfactory.flow_xls.XLSBlockMetrics

   .. autodoc2-docstring:: hlsfactory.flow_xls.XLSBlockMetrics

   .. py:attribute:: flop_count
      :canonical: hlsfactory.flow_xls.XLSBlockMetrics.flop_count
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.XLSBlockMetrics.flop_count

   .. py:attribute:: feedthrough_path_exists
      :canonical: hlsfactory.flow_xls.XLSBlockMetrics.feedthrough_path_exists
      :type: bool
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.XLSBlockMetrics.feedthrough_path_exists

   .. py:attribute:: delay_model
      :canonical: hlsfactory.flow_xls.XLSBlockMetrics.delay_model
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.XLSBlockMetrics.delay_model

   .. py:attribute:: max_reg_to_reg_delay_ps
      :canonical: hlsfactory.flow_xls.XLSBlockMetrics.max_reg_to_reg_delay_ps
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.XLSBlockMetrics.max_reg_to_reg_delay_ps

   .. py:attribute:: max_input_to_reg_delay_ps
      :canonical: hlsfactory.flow_xls.XLSBlockMetrics.max_input_to_reg_delay_ps
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.XLSBlockMetrics.max_input_to_reg_delay_ps

   .. py:attribute:: max_reg_to_output_delay_ps
      :canonical: hlsfactory.flow_xls.XLSBlockMetrics.max_reg_to_output_delay_ps
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.XLSBlockMetrics.max_reg_to_output_delay_ps

   .. py:attribute:: max_feedthrough_path_delay_ps
      :canonical: hlsfactory.flow_xls.XLSBlockMetrics.max_feedthrough_path_delay_ps
      :type: int | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.XLSBlockMetrics.max_feedthrough_path_delay_ps

   .. py:attribute:: operation_counts
      :canonical: hlsfactory.flow_xls.XLSBlockMetrics.operation_counts
      :type: dict[str, int]
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.XLSBlockMetrics.operation_counts

   .. py:attribute:: operation_kind_counts
      :canonical: hlsfactory.flow_xls.XLSBlockMetrics.operation_kind_counts
      :type: dict[str, int]
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_xls.XLSBlockMetrics.operation_kind_counts

.. py:function:: _extract_textproto_blocks(text: str, field_name: str) -> list[str]
   :canonical: hlsfactory.flow_xls._extract_textproto_blocks

   .. autodoc2-docstring:: hlsfactory.flow_xls._extract_textproto_blocks

.. py:function:: _parse_textproto_int(text: str, field_name: str) -> int | None
   :canonical: hlsfactory.flow_xls._parse_textproto_int

   .. autodoc2-docstring:: hlsfactory.flow_xls._parse_textproto_int

.. py:function:: _parse_textproto_bool(text: str, field_name: str) -> bool | None
   :canonical: hlsfactory.flow_xls._parse_textproto_bool

   .. autodoc2-docstring:: hlsfactory.flow_xls._parse_textproto_bool

.. py:function:: _parse_textproto_string(text: str, field_name: str) -> str | None
   :canonical: hlsfactory.flow_xls._parse_textproto_string

   .. autodoc2-docstring:: hlsfactory.flow_xls._parse_textproto_string

.. py:function:: _parse_textproto_enum(text: str, field_name: str) -> str | None
   :canonical: hlsfactory.flow_xls._parse_textproto_enum

   .. autodoc2-docstring:: hlsfactory.flow_xls._parse_textproto_enum

.. py:function:: parse_xls_module_signature(path: pathlib.Path) -> tuple[int | None, int | None]
   :canonical: hlsfactory.flow_xls.parse_xls_module_signature

   .. autodoc2-docstring:: hlsfactory.flow_xls.parse_xls_module_signature

.. py:function:: parse_xls_block_metrics(path: pathlib.Path) -> hlsfactory.flow_xls.XLSBlockMetrics
   :canonical: hlsfactory.flow_xls.parse_xls_block_metrics

   .. autodoc2-docstring:: hlsfactory.flow_xls.parse_xls_block_metrics

.. py:class:: XLSHLSSynthFlow(xls_install_dir: str | pathlib.Path | None = None, log_output: bool = False, log_execution_time: bool = True)
   :canonical: hlsfactory.flow_xls.XLSHLSSynthFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. autodoc2-docstring:: hlsfactory.flow_xls.XLSHLSSynthFlow

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.flow_xls.XLSHLSSynthFlow.__init__

   .. py:attribute:: name
      :canonical: hlsfactory.flow_xls.XLSHLSSynthFlow.name
      :value: 'XLSHLSSynthFlow'

      .. autodoc2-docstring:: hlsfactory.flow_xls.XLSHLSSynthFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_xls.XLSHLSSynthFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_xls.XLSHLSSynthFlow.execute

   .. py:method:: _remaining_timeout(start_time: float, timeout: float | None) -> float | None
      :canonical: hlsfactory.flow_xls.XLSHLSSynthFlow._remaining_timeout
      :staticmethod:

      .. autodoc2-docstring:: hlsfactory.flow_xls.XLSHLSSynthFlow._remaining_timeout

   .. py:method:: _log_execution_time(design_dir: pathlib.Path, start_time: float) -> None
      :canonical: hlsfactory.flow_xls.XLSHLSSynthFlow._log_execution_time

      .. autodoc2-docstring:: hlsfactory.flow_xls.XLSHLSSynthFlow._log_execution_time

.. py:data:: __all__
   :canonical: hlsfactory.flow_xls.__all__
   :value: ['DEFAULT_XLS_PATH', 'DesignHLSSynthData', 'XLSBlockMetrics', 'XLSHLSSynthFlow', 'HLSFACTORY_XLS_PAT...

   .. autodoc2-docstring:: hlsfactory.flow_xls.__all__
