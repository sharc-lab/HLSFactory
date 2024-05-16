:py:mod:`hlsfactory.flow_intel`
===============================

.. py:module:: hlsfactory.flow_intel

.. autodoc2-docstring:: hlsfactory.flow_intel
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`DesignHLSSynthData <hlsfactory.flow_intel.DesignHLSSynthData>`
     - .. autodoc2-docstring:: hlsfactory.flow_intel.DesignHLSSynthData
          :summary:
   * - :py:obj:`IntelHLSDesign <hlsfactory.flow_intel.IntelHLSDesign>`
     - .. autodoc2-docstring:: hlsfactory.flow_intel.IntelHLSDesign
          :summary:
   * - :py:obj:`IntelHLSSynthFlow <hlsfactory.flow_intel.IntelHLSSynthFlow>`
     -
   * - :py:obj:`IntelImpDesignResource <hlsfactory.flow_intel.IntelImpDesignResource>`
     - .. autodoc2-docstring:: hlsfactory.flow_intel.IntelImpDesignResource
          :summary:
   * - :py:obj:`IntelQuartusImplFlow <hlsfactory.flow_intel.IntelQuartusImplFlow>`
     -

API
~~~

.. py:class:: DesignHLSSynthData
   :canonical: hlsfactory.flow_intel.DesignHLSSynthData

   .. autodoc2-docstring:: hlsfactory.flow_intel.DesignHLSSynthData

   .. py:attribute:: resources_ALUTs_used
      :canonical: hlsfactory.flow_intel.DesignHLSSynthData.resources_ALUTs_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.DesignHLSSynthData.resources_ALUTs_used

   .. py:attribute:: resources_FFs_used
      :canonical: hlsfactory.flow_intel.DesignHLSSynthData.resources_FFs_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.DesignHLSSynthData.resources_FFs_used

   .. py:attribute:: resources_RAMs_used
      :canonical: hlsfactory.flow_intel.DesignHLSSynthData.resources_RAMs_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.DesignHLSSynthData.resources_RAMs_used

   .. py:attribute:: resources_DSPs_used
      :canonical: hlsfactory.flow_intel.DesignHLSSynthData.resources_DSPs_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.DesignHLSSynthData.resources_DSPs_used

   .. py:attribute:: resources_MLABs_used
      :canonical: hlsfactory.flow_intel.DesignHLSSynthData.resources_MLABs_used
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.DesignHLSSynthData.resources_MLABs_used

   .. py:attribute:: resources_ALUTs_avail
      :canonical: hlsfactory.flow_intel.DesignHLSSynthData.resources_ALUTs_avail
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.DesignHLSSynthData.resources_ALUTs_avail

   .. py:attribute:: resources_FFs_avail
      :canonical: hlsfactory.flow_intel.DesignHLSSynthData.resources_FFs_avail
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.DesignHLSSynthData.resources_FFs_avail

   .. py:attribute:: resources_RAMs_avail
      :canonical: hlsfactory.flow_intel.DesignHLSSynthData.resources_RAMs_avail
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.DesignHLSSynthData.resources_RAMs_avail

   .. py:attribute:: resources_DSPs_avail
      :canonical: hlsfactory.flow_intel.DesignHLSSynthData.resources_DSPs_avail
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.DesignHLSSynthData.resources_DSPs_avail

   .. py:attribute:: resources_MLABs_avail
      :canonical: hlsfactory.flow_intel.DesignHLSSynthData.resources_MLABs_avail
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.DesignHLSSynthData.resources_MLABs_avail

   .. py:method:: parse_from_synth_report_file(data_file: pathlib.Path) -> hlsfactory.flow_intel.DesignHLSSynthData
      :canonical: hlsfactory.flow_intel.DesignHLSSynthData.parse_from_synth_report_file
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.flow_intel.DesignHLSSynthData.parse_from_synth_report_file

.. py:class:: IntelHLSDesign
   :canonical: hlsfactory.flow_intel.IntelHLSDesign

   .. autodoc2-docstring:: hlsfactory.flow_intel.IntelHLSDesign

   .. py:attribute:: name
      :canonical: hlsfactory.flow_intel.IntelHLSDesign.name
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelHLSDesign.name

   .. py:attribute:: target_clock_period
      :canonical: hlsfactory.flow_intel.IntelHLSDesign.target_clock_period
      :type: None | str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelHLSDesign.target_clock_period

   .. py:attribute:: family
      :canonical: hlsfactory.flow_intel.IntelHLSDesign.family
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelHLSDesign.family

   .. py:attribute:: product
      :canonical: hlsfactory.flow_intel.IntelHLSDesign.product
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelHLSDesign.product

   .. py:attribute:: quartus
      :canonical: hlsfactory.flow_intel.IntelHLSDesign.quartus
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelHLSDesign.quartus

   .. py:attribute:: time
      :canonical: hlsfactory.flow_intel.IntelHLSDesign.time
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelHLSDesign.time

   .. py:attribute:: version
      :canonical: hlsfactory.flow_intel.IntelHLSDesign.version
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelHLSDesign.version

   .. py:method:: parse_from_synth_report_file(info_json: pathlib.Path, summary_json: pathlib.Path) -> hlsfactory.flow_intel.IntelHLSDesign
      :canonical: hlsfactory.flow_intel.IntelHLSDesign.parse_from_synth_report_file
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelHLSDesign.parse_from_synth_report_file

.. py:class:: IntelHLSSynthFlow(ipp_bin: str | None = None, arch: str = '1ST110EN1F43E1VG', clock: str = '10ns', verbose: bool = True)
   :canonical: hlsfactory.flow_intel.IntelHLSSynthFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: name
      :canonical: hlsfactory.flow_intel.IntelHLSSynthFlow.name
      :value: 'IntelHLSSynthFlow'

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelHLSSynthFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_intel.IntelHLSSynthFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelHLSSynthFlow.execute

.. py:class:: IntelImpDesignResource
   :canonical: hlsfactory.flow_intel.IntelImpDesignResource

   .. autodoc2-docstring:: hlsfactory.flow_intel.IntelImpDesignResource

   .. py:attribute:: name
      :canonical: hlsfactory.flow_intel.IntelImpDesignResource.name
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelImpDesignResource.name

   .. py:attribute:: clock_unit
      :canonical: hlsfactory.flow_intel.IntelImpDesignResource.clock_unit
      :type: str | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelImpDesignResource.clock_unit

   .. py:attribute:: clock
      :canonical: hlsfactory.flow_intel.IntelImpDesignResource.clock
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelImpDesignResource.clock

   .. py:attribute:: clock1x
      :canonical: hlsfactory.flow_intel.IntelImpDesignResource.clock1x
      :type: float | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelImpDesignResource.clock1x

   .. py:attribute:: alm
      :canonical: hlsfactory.flow_intel.IntelImpDesignResource.alm
      :type: float
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelImpDesignResource.alm

   .. py:attribute:: reg
      :canonical: hlsfactory.flow_intel.IntelImpDesignResource.reg
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelImpDesignResource.reg

   .. py:attribute:: dsp
      :canonical: hlsfactory.flow_intel.IntelImpDesignResource.dsp
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelImpDesignResource.dsp

   .. py:attribute:: ram
      :canonical: hlsfactory.flow_intel.IntelImpDesignResource.ram
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelImpDesignResource.ram

   .. py:attribute:: mlab
      :canonical: hlsfactory.flow_intel.IntelImpDesignResource.mlab
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelImpDesignResource.mlab

   .. py:method:: parse_from_impl_report_file(quartus_json: pathlib.Path) -> hlsfactory.flow_intel.IntelImpDesignResource
      :canonical: hlsfactory.flow_intel.IntelImpDesignResource.parse_from_impl_report_file
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelImpDesignResource.parse_from_impl_report_file

.. py:class:: IntelQuartusImplFlow(quartus_bin: str | None = None, verbose: bool = True)
   :canonical: hlsfactory.flow_intel.IntelQuartusImplFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: name
      :canonical: hlsfactory.flow_intel.IntelQuartusImplFlow.name
      :value: 'IntelQuartusImplFlow'

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelQuartusImplFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_intel.IntelQuartusImplFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_intel.IntelQuartusImplFlow.execute
