:py:mod:`hlsfactory.flow_config`
================================

.. py:module:: hlsfactory.flow_config

.. autodoc2-docstring:: hlsfactory.flow_config
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`FlowConfig <hlsfactory.flow_config.FlowConfig>`
     - .. autodoc2-docstring:: hlsfactory.flow_config.FlowConfig
          :summary:
   * - :py:obj:`DesignConfig <hlsfactory.flow_config.DesignConfig>`
     - .. autodoc2-docstring:: hlsfactory.flow_config.DesignConfig
          :summary:

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`read_design_config <hlsfactory.flow_config.read_design_config>`
     - .. autodoc2-docstring:: hlsfactory.flow_config.read_design_config
          :summary:
   * - :py:obj:`write_design_config <hlsfactory.flow_config.write_design_config>`
     - .. autodoc2-docstring:: hlsfactory.flow_config.write_design_config
          :summary:

API
~~~

.. py:class:: FlowConfig
   :canonical: hlsfactory.flow_config.FlowConfig

   .. autodoc2-docstring:: hlsfactory.flow_config.FlowConfig

   .. py:attribute:: flow_name
      :canonical: hlsfactory.flow_config.FlowConfig.flow_name
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_config.FlowConfig.flow_name

   .. py:attribute:: flow_settings
      :canonical: hlsfactory.flow_config.FlowConfig.flow_settings
      :type: dict[str, str]
      :value: 'field(...)'

      .. autodoc2-docstring:: hlsfactory.flow_config.FlowConfig.flow_settings

.. py:class:: DesignConfig
   :canonical: hlsfactory.flow_config.DesignConfig

   .. autodoc2-docstring:: hlsfactory.flow_config.DesignConfig

   .. py:attribute:: design_name
      :canonical: hlsfactory.flow_config.DesignConfig.design_name
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_config.DesignConfig.design_name

   .. py:attribute:: dataset_name
      :canonical: hlsfactory.flow_config.DesignConfig.dataset_name
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_config.DesignConfig.dataset_name

   .. py:attribute:: env_vars
      :canonical: hlsfactory.flow_config.DesignConfig.env_vars
      :type: dict[str, str]
      :value: 'field(...)'

      .. autodoc2-docstring:: hlsfactory.flow_config.DesignConfig.env_vars

   .. py:attribute:: flow_configs
      :canonical: hlsfactory.flow_config.DesignConfig.flow_configs
      :type: list[hlsfactory.flow_config.FlowConfig]
      :value: 'field(...)'

      .. autodoc2-docstring:: hlsfactory.flow_config.DesignConfig.flow_configs

.. py:function:: read_design_config(file_path: pathlib.Path) -> hlsfactory.flow_config.DesignConfig
   :canonical: hlsfactory.flow_config.read_design_config

   .. autodoc2-docstring:: hlsfactory.flow_config.read_design_config

.. py:function:: write_design_config(file_path: pathlib.Path, config: hlsfactory.flow_config.DesignConfig) -> None
   :canonical: hlsfactory.flow_config.write_design_config

   .. autodoc2-docstring:: hlsfactory.flow_config.write_design_config
