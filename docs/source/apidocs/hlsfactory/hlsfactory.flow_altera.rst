:py:mod:`hlsfactory.flow_altera`
================================

.. py:module:: hlsfactory.flow_altera

.. autodoc2-docstring:: hlsfactory.flow_altera
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`AlteraHLSSynthData <hlsfactory.flow_altera.AlteraHLSSynthData>`
     - .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthData
          :summary:
   * - :py:obj:`AlteraHLSDesign <hlsfactory.flow_altera.AlteraHLSDesign>`
     - .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSDesign
          :summary:
   * - :py:obj:`AlteraImplDesignResource <hlsfactory.flow_altera.AlteraImplDesignResource>`
     - .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraImplDesignResource
          :summary:
   * - :py:obj:`_AlteraFlow <hlsfactory.flow_altera._AlteraFlow>`
     -
   * - :py:obj:`AlteraHLSSynthFlow <hlsfactory.flow_altera.AlteraHLSSynthFlow>`
     - .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthFlow
          :summary:
   * - :py:obj:`AlteraHLSCsimFlow <hlsfactory.flow_altera.AlteraHLSCsimFlow>`
     - .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCsimFlow
          :summary:
   * - :py:obj:`AlteraQuartusImplFlow <hlsfactory.flow_altera.AlteraQuartusImplFlow>`
     - .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraQuartusImplFlow
          :summary:
   * - :py:obj:`AlteraHLSCosimFlow <hlsfactory.flow_altera.AlteraHLSCosimFlow>`
     - .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCosimFlow
          :summary:

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`read_altera_report <hlsfactory.flow_altera.read_altera_report>`
     - .. autodoc2-docstring:: hlsfactory.flow_altera.read_altera_report
          :summary:

Data
~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`HLSFACTORY_ALTERA_HLS_PATH_ENV_VAR <hlsfactory.flow_altera.HLSFACTORY_ALTERA_HLS_PATH_ENV_VAR>`
     - .. autodoc2-docstring:: hlsfactory.flow_altera.HLSFACTORY_ALTERA_HLS_PATH_ENV_VAR
          :summary:
   * - :py:obj:`HLSFACTORY_ALTERA_QUESTA_SETUP_ENV_VAR <hlsfactory.flow_altera.HLSFACTORY_ALTERA_QUESTA_SETUP_ENV_VAR>`
     - .. autodoc2-docstring:: hlsfactory.flow_altera.HLSFACTORY_ALTERA_QUESTA_SETUP_ENV_VAR
          :summary:
   * - :py:obj:`_QUESTA_INI_COMPAT_SCRIPT <hlsfactory.flow_altera._QUESTA_INI_COMPAT_SCRIPT>`
     - .. autodoc2-docstring:: hlsfactory.flow_altera._QUESTA_INI_COMPAT_SCRIPT
          :summary:

API
~~~

.. py:data:: HLSFACTORY_ALTERA_HLS_PATH_ENV_VAR
   :canonical: hlsfactory.flow_altera.HLSFACTORY_ALTERA_HLS_PATH_ENV_VAR
   :value: 'HLSFACTORY_ALTERA_HLS_PATH'

   .. autodoc2-docstring:: hlsfactory.flow_altera.HLSFACTORY_ALTERA_HLS_PATH_ENV_VAR

.. py:data:: HLSFACTORY_ALTERA_QUESTA_SETUP_ENV_VAR
   :canonical: hlsfactory.flow_altera.HLSFACTORY_ALTERA_QUESTA_SETUP_ENV_VAR
   :value: 'HLSFACTORY_ALTERA_QUESTA_SETUP'

   .. autodoc2-docstring:: hlsfactory.flow_altera.HLSFACTORY_ALTERA_QUESTA_SETUP_ENV_VAR

.. py:data:: _QUESTA_INI_COMPAT_SCRIPT
   :canonical: hlsfactory.flow_altera._QUESTA_INI_COMPAT_SCRIPT
   :value: <Multiline-String>

   .. autodoc2-docstring:: hlsfactory.flow_altera._QUESTA_INI_COMPAT_SCRIPT

.. py:function:: read_altera_report(path: pathlib.Path) -> list[dict[str, typing.Any]]
   :canonical: hlsfactory.flow_altera.read_altera_report

   .. autodoc2-docstring:: hlsfactory.flow_altera.read_altera_report

.. py:class:: AlteraHLSSynthData
   :canonical: hlsfactory.flow_altera.AlteraHLSSynthData

   .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthData

   .. py:attribute:: resources_ALUTs_used
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthData.resources_ALUTs_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthData.resources_ALUTs_used

   .. py:attribute:: resources_FFs_used
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthData.resources_FFs_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthData.resources_FFs_used

   .. py:attribute:: resources_RAMs_used
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthData.resources_RAMs_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthData.resources_RAMs_used

   .. py:attribute:: resources_DSPs_used
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthData.resources_DSPs_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthData.resources_DSPs_used

   .. py:attribute:: resources_MLABs_used
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthData.resources_MLABs_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthData.resources_MLABs_used

   .. py:attribute:: resources_ALUTs_avail
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthData.resources_ALUTs_avail
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthData.resources_ALUTs_avail

   .. py:attribute:: resources_FFs_avail
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthData.resources_FFs_avail
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthData.resources_FFs_avail

   .. py:attribute:: resources_RAMs_avail
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthData.resources_RAMs_avail
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthData.resources_RAMs_avail

   .. py:attribute:: resources_DSPs_avail
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthData.resources_DSPs_avail
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthData.resources_DSPs_avail

   .. py:attribute:: resources_MLABs_avail
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthData.resources_MLABs_avail
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthData.resources_MLABs_avail

   .. py:method:: parse_from_synth_report_file(data_file: pathlib.Path) -> hlsfactory.flow_altera.AlteraHLSSynthData
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthData.parse_from_synth_report_file
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthData.parse_from_synth_report_file

.. py:class:: AlteraHLSDesign
   :canonical: hlsfactory.flow_altera.AlteraHLSDesign

   .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSDesign

   .. py:attribute:: name
      :canonical: hlsfactory.flow_altera.AlteraHLSDesign.name
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSDesign.name

   .. py:attribute:: target_clock
      :canonical: hlsfactory.flow_altera.AlteraHLSDesign.target_clock
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSDesign.target_clock

   .. py:attribute:: family
      :canonical: hlsfactory.flow_altera.AlteraHLSDesign.family
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSDesign.family

   .. py:attribute:: product
      :canonical: hlsfactory.flow_altera.AlteraHLSDesign.product
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSDesign.product

   .. py:attribute:: quartus
      :canonical: hlsfactory.flow_altera.AlteraHLSDesign.quartus
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSDesign.quartus

   .. py:attribute:: time
      :canonical: hlsfactory.flow_altera.AlteraHLSDesign.time
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSDesign.time

   .. py:attribute:: version
      :canonical: hlsfactory.flow_altera.AlteraHLSDesign.version
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSDesign.version

   .. py:attribute:: kernels
      :canonical: hlsfactory.flow_altera.AlteraHLSDesign.kernels
      :type: list[str]
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSDesign.kernels

   .. py:method:: parse_from_synth_report_file(info_json: pathlib.Path, summary_json: pathlib.Path) -> hlsfactory.flow_altera.AlteraHLSDesign
      :canonical: hlsfactory.flow_altera.AlteraHLSDesign.parse_from_synth_report_file
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSDesign.parse_from_synth_report_file

.. py:class:: AlteraImplDesignResource
   :canonical: hlsfactory.flow_altera.AlteraImplDesignResource

   .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraImplDesignResource

   .. py:attribute:: resource_usage
      :canonical: hlsfactory.flow_altera.AlteraImplDesignResource.resource_usage
      :type: list[dict[str, typing.Any]]
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraImplDesignResource.resource_usage

   .. py:attribute:: clock_frequencies
      :canonical: hlsfactory.flow_altera.AlteraImplDesignResource.clock_frequencies
      :type: list[dict[str, typing.Any]]
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraImplDesignResource.clock_frequencies

   .. py:method:: parse_from_impl_report_file(quartus_json: pathlib.Path) -> hlsfactory.flow_altera.AlteraImplDesignResource
      :canonical: hlsfactory.flow_altera.AlteraImplDesignResource.parse_from_impl_report_file
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraImplDesignResource.parse_from_impl_report_file

.. py:class:: _AlteraFlow(ahls_bin: str | pathlib.Path | None = None, altera_install_dir: str | pathlib.Path | None = None, ahls_sh: str | pathlib.Path | None = None, log_output: bool = False, log_execution_time: bool = True)
   :canonical: hlsfactory.flow_altera._AlteraFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: flow_name
      :canonical: hlsfactory.flow_altera._AlteraFlow.flow_name
      :type: hlsfactory.design_config.FlowName
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera._AlteraFlow.flow_name

   .. py:attribute:: stage
      :canonical: hlsfactory.flow_altera._AlteraFlow.stage
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera._AlteraFlow.stage

   .. py:attribute:: result_file
      :canonical: hlsfactory.flow_altera._AlteraFlow.result_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera._AlteraFlow.result_file

   .. py:attribute:: questa_setup_script
      :canonical: hlsfactory.flow_altera._AlteraFlow.questa_setup_script
      :type: pathlib.Path | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera._AlteraFlow.questa_setup_script

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_altera._AlteraFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_altera._AlteraFlow.execute

   .. py:method:: _collect_results(design_dir: pathlib.Path, output: pathlib.Path, clock: str | None) -> None
      :canonical: hlsfactory.flow_altera._AlteraFlow._collect_results

      .. autodoc2-docstring:: hlsfactory.flow_altera._AlteraFlow._collect_results

.. py:class:: AlteraHLSSynthFlow(ahls_bin: str | pathlib.Path | None = None, altera_install_dir: str | pathlib.Path | None = None, ahls_sh: str | pathlib.Path | None = None, log_output: bool = False, log_execution_time: bool = True)
   :canonical: hlsfactory.flow_altera.AlteraHLSSynthFlow

   Bases: :py:obj:`hlsfactory.flow_altera._AlteraFlow`

   .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthFlow

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthFlow.__init__

   .. py:attribute:: name
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthFlow.name
      :value: 'AlteraHLSSynthFlow'

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthFlow.name

   .. py:attribute:: flow_name
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthFlow.flow_name
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthFlow.flow_name

   .. py:attribute:: stage
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthFlow.stage
      :value: 'synth'

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthFlow.stage

   .. py:attribute:: result_file
      :canonical: hlsfactory.flow_altera.AlteraHLSSynthFlow.result_file
      :value: 'data_altera_synth.json'

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSSynthFlow.result_file

.. py:class:: AlteraHLSCsimFlow(ahls_bin: str | pathlib.Path | None = None, altera_install_dir: str | pathlib.Path | None = None, ahls_sh: str | pathlib.Path | None = None, log_output: bool = False, log_execution_time: bool = True)
   :canonical: hlsfactory.flow_altera.AlteraHLSCsimFlow

   Bases: :py:obj:`hlsfactory.flow_altera._AlteraFlow`

   .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCsimFlow

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCsimFlow.__init__

   .. py:attribute:: name
      :canonical: hlsfactory.flow_altera.AlteraHLSCsimFlow.name
      :value: 'AlteraHLSCsimFlow'

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCsimFlow.name

   .. py:attribute:: flow_name
      :canonical: hlsfactory.flow_altera.AlteraHLSCsimFlow.flow_name
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCsimFlow.flow_name

   .. py:attribute:: stage
      :canonical: hlsfactory.flow_altera.AlteraHLSCsimFlow.stage
      :value: 'csim'

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCsimFlow.stage

   .. py:attribute:: result_file
      :canonical: hlsfactory.flow_altera.AlteraHLSCsimFlow.result_file
      :value: 'data_altera_csim.json'

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCsimFlow.result_file

.. py:class:: AlteraQuartusImplFlow(ahls_bin: str | pathlib.Path | None = None, altera_install_dir: str | pathlib.Path | None = None, ahls_sh: str | pathlib.Path | None = None, log_output: bool = False, log_execution_time: bool = True)
   :canonical: hlsfactory.flow_altera.AlteraQuartusImplFlow

   Bases: :py:obj:`hlsfactory.flow_altera._AlteraFlow`

   .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraQuartusImplFlow

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraQuartusImplFlow.__init__

   .. py:attribute:: name
      :canonical: hlsfactory.flow_altera.AlteraQuartusImplFlow.name
      :value: 'AlteraQuartusImplFlow'

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraQuartusImplFlow.name

   .. py:attribute:: flow_name
      :canonical: hlsfactory.flow_altera.AlteraQuartusImplFlow.flow_name
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraQuartusImplFlow.flow_name

   .. py:attribute:: stage
      :canonical: hlsfactory.flow_altera.AlteraQuartusImplFlow.stage
      :value: 'impl'

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraQuartusImplFlow.stage

   .. py:attribute:: result_file
      :canonical: hlsfactory.flow_altera.AlteraQuartusImplFlow.result_file
      :value: 'data_altera_impl.json'

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraQuartusImplFlow.result_file

.. py:class:: AlteraHLSCosimFlow(ahls_bin: str | pathlib.Path | None = None, altera_install_dir: str | pathlib.Path | None = None, ahls_sh: str | pathlib.Path | None = None, log_output: bool = False, log_execution_time: bool = True, questa_setup_script: str | pathlib.Path | None = None)
   :canonical: hlsfactory.flow_altera.AlteraHLSCosimFlow

   Bases: :py:obj:`hlsfactory.flow_altera._AlteraFlow`

   .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCosimFlow

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCosimFlow.__init__

   .. py:attribute:: name
      :canonical: hlsfactory.flow_altera.AlteraHLSCosimFlow.name
      :value: 'AlteraHLSCosimFlow'

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCosimFlow.name

   .. py:attribute:: flow_name
      :canonical: hlsfactory.flow_altera.AlteraHLSCosimFlow.flow_name
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCosimFlow.flow_name

   .. py:attribute:: stage
      :canonical: hlsfactory.flow_altera.AlteraHLSCosimFlow.stage
      :value: 'cosim'

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCosimFlow.stage

   .. py:attribute:: result_file
      :canonical: hlsfactory.flow_altera.AlteraHLSCosimFlow.result_file
      :value: 'data_altera_cosim.json'

      .. autodoc2-docstring:: hlsfactory.flow_altera.AlteraHLSCosimFlow.result_file
