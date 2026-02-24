:py:mod:`hlsfactory.design_config`
==================================

.. py:module:: hlsfactory.design_config

.. autodoc2-docstring:: hlsfactory.design_config
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`FlowName <hlsfactory.design_config.FlowName>`
     -
   * - :py:obj:`FlowConfig <hlsfactory.design_config.FlowConfig>`
     - .. autodoc2-docstring:: hlsfactory.design_config.FlowConfig
          :summary:
   * - :py:obj:`DesignConfig <hlsfactory.design_config.DesignConfig>`
     - .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig
          :summary:

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`_ensure_non_empty_str <hlsfactory.design_config._ensure_non_empty_str>`
     - .. autodoc2-docstring:: hlsfactory.design_config._ensure_non_empty_str
          :summary:
   * - :py:obj:`_ensure_str_sequence <hlsfactory.design_config._ensure_str_sequence>`
     - .. autodoc2-docstring:: hlsfactory.design_config._ensure_str_sequence
          :summary:
   * - :py:obj:`_ensure_str_mapping <hlsfactory.design_config._ensure_str_mapping>`
     - .. autodoc2-docstring:: hlsfactory.design_config._ensure_str_mapping
          :summary:
   * - :py:obj:`_normalise_flow_name <hlsfactory.design_config._normalise_flow_name>`
     - .. autodoc2-docstring:: hlsfactory.design_config._normalise_flow_name
          :summary:
   * - :py:obj:`read_design_config <hlsfactory.design_config.read_design_config>`
     - .. autodoc2-docstring:: hlsfactory.design_config.read_design_config
          :summary:
   * - :py:obj:`write_design_config <hlsfactory.design_config.write_design_config>`
     - .. autodoc2-docstring:: hlsfactory.design_config.write_design_config
          :summary:
   * - :py:obj:`_quote_toml_string <hlsfactory.design_config._quote_toml_string>`
     - .. autodoc2-docstring:: hlsfactory.design_config._quote_toml_string
          :summary:

Data
~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`DESIGN_CONFIG_FILENAME <hlsfactory.design_config.DESIGN_CONFIG_FILENAME>`
     - .. autodoc2-docstring:: hlsfactory.design_config.DESIGN_CONFIG_FILENAME
          :summary:
   * - :py:obj:`__all__ <hlsfactory.design_config.__all__>`
     - .. autodoc2-docstring:: hlsfactory.design_config.__all__
          :summary:

API
~~~

.. py:exception:: DesignConfigError()
   :canonical: hlsfactory.design_config.DesignConfigError

   Bases: :py:obj:`ValueError`

   .. autodoc2-docstring:: hlsfactory.design_config.DesignConfigError

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.design_config.DesignConfigError.__init__

.. py:exception:: FlowConfigError()
   :canonical: hlsfactory.design_config.FlowConfigError

   Bases: :py:obj:`ValueError`

   .. autodoc2-docstring:: hlsfactory.design_config.FlowConfigError

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.design_config.FlowConfigError.__init__

.. py:data:: DESIGN_CONFIG_FILENAME
   :canonical: hlsfactory.design_config.DESIGN_CONFIG_FILENAME
   :value: 'hlsfactory.toml'

   .. autodoc2-docstring:: hlsfactory.design_config.DESIGN_CONFIG_FILENAME

.. py:class:: FlowName()
   :canonical: hlsfactory.design_config.FlowName

   Bases: :py:obj:`enum.StrEnum`

   .. py:attribute:: OPT_DSL_V2
      :canonical: hlsfactory.design_config.FlowName.OPT_DSL_V2
      :value: 'OptDSLv2'

      .. autodoc2-docstring:: hlsfactory.design_config.FlowName.OPT_DSL_V2

   .. py:attribute:: VITIS_HLS_SYNTH
      :canonical: hlsfactory.design_config.FlowName.VITIS_HLS_SYNTH
      :value: 'VitisHLSSynthFlow'

      .. autodoc2-docstring:: hlsfactory.design_config.FlowName.VITIS_HLS_SYNTH

   .. py:attribute:: VITIS_HLS_CSIM
      :canonical: hlsfactory.design_config.FlowName.VITIS_HLS_CSIM
      :value: 'VitisHLSCsimFlow'

      .. autodoc2-docstring:: hlsfactory.design_config.FlowName.VITIS_HLS_CSIM

   .. py:attribute:: VITIS_HLS_IMPL
      :canonical: hlsfactory.design_config.FlowName.VITIS_HLS_IMPL
      :value: 'VitisHLSImplFlow'

      .. autodoc2-docstring:: hlsfactory.design_config.FlowName.VITIS_HLS_IMPL

   .. py:attribute:: VITIS_HLS_COSIM
      :canonical: hlsfactory.design_config.FlowName.VITIS_HLS_COSIM
      :value: 'VitisHLSCosimFlow'

      .. autodoc2-docstring:: hlsfactory.design_config.FlowName.VITIS_HLS_COSIM

   .. py:attribute:: VITIS_HLS_COSIM_SETUP
      :canonical: hlsfactory.design_config.FlowName.VITIS_HLS_COSIM_SETUP
      :value: 'VitisHLSCosimSetupFlow'

      .. autodoc2-docstring:: hlsfactory.design_config.FlowName.VITIS_HLS_COSIM_SETUP

   .. py:attribute:: VITIS_HLS_IMPL_REPORT
      :canonical: hlsfactory.design_config.FlowName.VITIS_HLS_IMPL_REPORT
      :value: 'VitisHLSImplReportFlow'

      .. autodoc2-docstring:: hlsfactory.design_config.FlowName.VITIS_HLS_IMPL_REPORT

   .. py:attribute:: LIGHTNING_SIM_V2
      :canonical: hlsfactory.design_config.FlowName.LIGHTNING_SIM_V2
      :value: 'LightningSimV2Flow'

      .. autodoc2-docstring:: hlsfactory.design_config.FlowName.LIGHTNING_SIM_V2

.. py:function:: _ensure_non_empty_str(value: typing.Any, *, field_name: str, error_cls: type[Exception] = DesignConfigError) -> str
   :canonical: hlsfactory.design_config._ensure_non_empty_str

   .. autodoc2-docstring:: hlsfactory.design_config._ensure_non_empty_str

.. py:function:: _ensure_str_sequence(values: collections.abc.Iterable[typing.Any], *, field_name: str, error_cls: type[Exception] = DesignConfigError) -> list[str]
   :canonical: hlsfactory.design_config._ensure_str_sequence

   .. autodoc2-docstring:: hlsfactory.design_config._ensure_str_sequence

.. py:function:: _ensure_str_mapping(values: collections.abc.Mapping[typing.Any, typing.Any], *, field_name: str, error_cls: type[Exception] = DesignConfigError) -> dict[str, str]
   :canonical: hlsfactory.design_config._ensure_str_mapping

   .. autodoc2-docstring:: hlsfactory.design_config._ensure_str_mapping

.. py:function:: _normalise_flow_name(flow_name: str | hlsfactory.design_config.FlowName) -> str
   :canonical: hlsfactory.design_config._normalise_flow_name

   .. autodoc2-docstring:: hlsfactory.design_config._normalise_flow_name

.. py:class:: FlowConfig
   :canonical: hlsfactory.design_config.FlowConfig

   .. autodoc2-docstring:: hlsfactory.design_config.FlowConfig

   .. py:attribute:: flow_name
      :canonical: hlsfactory.design_config.FlowConfig.flow_name
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.design_config.FlowConfig.flow_name

   .. py:attribute:: flow_settings
      :canonical: hlsfactory.design_config.FlowConfig.flow_settings
      :type: dict[str, str]
      :value: 'field(...)'

      .. autodoc2-docstring:: hlsfactory.design_config.FlowConfig.flow_settings

   .. py:attribute:: _REQUIRED_SETTINGS
      :canonical: hlsfactory.design_config.FlowConfig._REQUIRED_SETTINGS
      :type: typing.ClassVar[dict[str, frozenset[str]]]
      :value: None

      .. autodoc2-docstring:: hlsfactory.design_config.FlowConfig._REQUIRED_SETTINGS

   .. py:method:: __post_init__() -> None
      :canonical: hlsfactory.design_config.FlowConfig.__post_init__

      .. autodoc2-docstring:: hlsfactory.design_config.FlowConfig.__post_init__

   .. py:property:: required_settings
      :canonical: hlsfactory.design_config.FlowConfig.required_settings
      :type: frozenset[str]

      .. autodoc2-docstring:: hlsfactory.design_config.FlowConfig.required_settings

   .. py:property:: is_known_flow
      :canonical: hlsfactory.design_config.FlowConfig.is_known_flow
      :type: bool

      .. autodoc2-docstring:: hlsfactory.design_config.FlowConfig.is_known_flow

   .. py:method:: has_setting(key: str) -> bool
      :canonical: hlsfactory.design_config.FlowConfig.has_setting

      .. autodoc2-docstring:: hlsfactory.design_config.FlowConfig.has_setting

   .. py:method:: get_setting(key: str, default: str | None = None) -> str | None
      :canonical: hlsfactory.design_config.FlowConfig.get_setting

      .. autodoc2-docstring:: hlsfactory.design_config.FlowConfig.get_setting

   .. py:method:: require_setting(key: str) -> str
      :canonical: hlsfactory.design_config.FlowConfig.require_setting

      .. autodoc2-docstring:: hlsfactory.design_config.FlowConfig.require_setting

   .. py:method:: _validate_required_settings() -> None
      :canonical: hlsfactory.design_config.FlowConfig._validate_required_settings

      .. autodoc2-docstring:: hlsfactory.design_config.FlowConfig._validate_required_settings

   .. py:method:: from_mapping(data: collections.abc.Mapping[str, typing.Any]) -> hlsfactory.design_config.FlowConfig
      :canonical: hlsfactory.design_config.FlowConfig.from_mapping
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.design_config.FlowConfig.from_mapping

.. py:class:: DesignConfig
   :canonical: hlsfactory.design_config.DesignConfig

   .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig

   .. py:attribute:: design_name
      :canonical: hlsfactory.design_config.DesignConfig.design_name
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig.design_name

   .. py:attribute:: dataset_name
      :canonical: hlsfactory.design_config.DesignConfig.dataset_name
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig.dataset_name

   .. py:attribute:: tags
      :canonical: hlsfactory.design_config.DesignConfig.tags
      :type: list[str]
      :value: 'field(...)'

      .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig.tags

   .. py:attribute:: env_vars
      :canonical: hlsfactory.design_config.DesignConfig.env_vars
      :type: dict[str, str]
      :value: 'field(...)'

      .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig.env_vars

   .. py:attribute:: flow_configs
      :canonical: hlsfactory.design_config.DesignConfig.flow_configs
      :type: list[hlsfactory.design_config.FlowConfig]
      :value: 'field(...)'

      .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig.flow_configs

   .. py:method:: __post_init__() -> None
      :canonical: hlsfactory.design_config.DesignConfig.__post_init__

      .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig.__post_init__

   .. py:method:: supports_flow(flow_name: str | hlsfactory.design_config.FlowName) -> bool
      :canonical: hlsfactory.design_config.DesignConfig.supports_flow

      .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig.supports_flow

   .. py:method:: get_flow_config(flow_name: str | hlsfactory.design_config.FlowName) -> hlsfactory.design_config.FlowConfig | None
      :canonical: hlsfactory.design_config.DesignConfig.get_flow_config

      .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig.get_flow_config

   .. py:method:: require_flow_config(flow_name: str | hlsfactory.design_config.FlowName) -> hlsfactory.design_config.FlowConfig
      :canonical: hlsfactory.design_config.DesignConfig.require_flow_config

      .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig.require_flow_config

   .. py:method:: get_flow_setting(flow_name: str | hlsfactory.design_config.FlowName, setting_name: str, *, default: str | None = None) -> str | None
      :canonical: hlsfactory.design_config.DesignConfig.get_flow_setting

      .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig.get_flow_setting

   .. py:method:: require_flow_setting(flow_name: str | hlsfactory.design_config.FlowName, setting_name: str) -> str
      :canonical: hlsfactory.design_config.DesignConfig.require_flow_setting

      .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig.require_flow_setting

   .. py:property:: flow_names
      :canonical: hlsfactory.design_config.DesignConfig.flow_names
      :type: tuple[str, ...]

      .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig.flow_names

   .. py:method:: from_mapping(data: collections.abc.Mapping[str, typing.Any]) -> hlsfactory.design_config.DesignConfig
      :canonical: hlsfactory.design_config.DesignConfig.from_mapping
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.design_config.DesignConfig.from_mapping

.. py:function:: read_design_config(file_path: pathlib.Path) -> hlsfactory.design_config.DesignConfig
   :canonical: hlsfactory.design_config.read_design_config

   .. autodoc2-docstring:: hlsfactory.design_config.read_design_config

.. py:function:: write_design_config(file_path: pathlib.Path, config: hlsfactory.design_config.DesignConfig) -> None
   :canonical: hlsfactory.design_config.write_design_config

   .. autodoc2-docstring:: hlsfactory.design_config.write_design_config

.. py:function:: _quote_toml_string(value: str) -> str
   :canonical: hlsfactory.design_config._quote_toml_string

   .. autodoc2-docstring:: hlsfactory.design_config._quote_toml_string

.. py:data:: __all__
   :canonical: hlsfactory.design_config.__all__
   :value: ['DESIGN_CONFIG_FILENAME', 'DesignConfig', 'DesignConfigError', 'FlowConfig', 'FlowConfigError', 'Fl...

   .. autodoc2-docstring:: hlsfactory.design_config.__all__
