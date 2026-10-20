:py:mod:`hlsfactory.data_packaging`
===================================

.. py:module:: hlsfactory.data_packaging

.. autodoc2-docstring:: hlsfactory.data_packaging
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`CompleteHLSData <hlsfactory.data_packaging.CompleteHLSData>`
     - .. autodoc2-docstring:: hlsfactory.data_packaging.CompleteHLSData
          :summary:
   * - :py:obj:`DataAggregator <hlsfactory.data_packaging.DataAggregator>`
     -
   * - :py:obj:`ArtifactsXilinx <hlsfactory.data_packaging.ArtifactsXilinx>`
     -
   * - :py:obj:`JSONFileDataAggregator <hlsfactory.data_packaging.JSONFileDataAggregator>`
     - .. autodoc2-docstring:: hlsfactory.data_packaging.JSONFileDataAggregator
          :summary:
   * - :py:obj:`DataAggregatorXilinx <hlsfactory.data_packaging.DataAggregatorXilinx>`
     -
   * - :py:obj:`ArtifactAggregator <hlsfactory.data_packaging.ArtifactAggregator>`
     - .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactAggregator
          :summary:
   * - :py:obj:`ArtifactsXLS <hlsfactory.data_packaging.ArtifactsXLS>`
     -
   * - :py:obj:`DataAggregatorXLS <hlsfactory.data_packaging.DataAggregatorXLS>`
     - .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXLS
          :summary:
   * - :py:obj:`ArtifactsCatapult <hlsfactory.data_packaging.ArtifactsCatapult>`
     -
   * - :py:obj:`DataAggregatorCatapult <hlsfactory.data_packaging.DataAggregatorCatapult>`
     - .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorCatapult
          :summary:
   * - :py:obj:`ArtifactsStratus <hlsfactory.data_packaging.ArtifactsStratus>`
     -
   * - :py:obj:`DataAggregatorStratus <hlsfactory.data_packaging.DataAggregatorStratus>`
     - .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorStratus
          :summary:
   * - :py:obj:`ArtifactsIntel <hlsfactory.data_packaging.ArtifactsIntel>`
     -
   * - :py:obj:`DataAggregatorIntel <hlsfactory.data_packaging.DataAggregatorIntel>`
     - .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorIntel
          :summary:
   * - :py:obj:`ArtifactsAltera <hlsfactory.data_packaging.ArtifactsAltera>`
     -
   * - :py:obj:`DataAggregatorAltera <hlsfactory.data_packaging.DataAggregatorAltera>`
     - .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorAltera
          :summary:
   * - :py:obj:`ArtifactsVitisModern <hlsfactory.data_packaging.ArtifactsVitisModern>`
     -
   * - :py:obj:`DataAggregatorVitisModern <hlsfactory.data_packaging.DataAggregatorVitisModern>`
     - .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorVitisModern
          :summary:

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`get_file_in_root <hlsfactory.data_packaging.get_file_in_root>`
     - .. autodoc2-docstring:: hlsfactory.data_packaging.get_file_in_root
          :summary:

Data
~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`InMemoryArchive <hlsfactory.data_packaging.InMemoryArchive>`
     - .. autodoc2-docstring:: hlsfactory.data_packaging.InMemoryArchive
          :summary:
   * - :py:obj:`ArtifactCollection <hlsfactory.data_packaging.ArtifactCollection>`
     - .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactCollection
          :summary:

API
~~~

.. py:data:: InMemoryArchive
   :canonical: hlsfactory.data_packaging.InMemoryArchive
   :value: None

   .. autodoc2-docstring:: hlsfactory.data_packaging.InMemoryArchive

.. py:class:: CompleteHLSData
   :canonical: hlsfactory.data_packaging.CompleteHLSData

   .. autodoc2-docstring:: hlsfactory.data_packaging.CompleteHLSData

   .. py:attribute:: design
      :canonical: hlsfactory.data_packaging.CompleteHLSData.design
      :type: dict | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.CompleteHLSData.design

   .. py:attribute:: synthesis
      :canonical: hlsfactory.data_packaging.CompleteHLSData.synthesis
      :type: dict | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.CompleteHLSData.synthesis

   .. py:attribute:: implementation
      :canonical: hlsfactory.data_packaging.CompleteHLSData.implementation
      :type: dict | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.CompleteHLSData.implementation

   .. py:attribute:: execution
      :canonical: hlsfactory.data_packaging.CompleteHLSData.execution
      :type: dict | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.CompleteHLSData.execution

   .. py:attribute:: artifacts
      :canonical: hlsfactory.data_packaging.CompleteHLSData.artifacts
      :type: hlsfactory.data_packaging.InMemoryArchive | None
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.CompleteHLSData.artifacts

   .. py:attribute:: design_id
      :canonical: hlsfactory.data_packaging.CompleteHLSData.design_id
      :type: str
      :value: 'field(...)'

      .. autodoc2-docstring:: hlsfactory.data_packaging.CompleteHLSData.design_id

   .. py:method:: to_flat_dict() -> dict
      :canonical: hlsfactory.data_packaging.CompleteHLSData.to_flat_dict

      .. autodoc2-docstring:: hlsfactory.data_packaging.CompleteHLSData.to_flat_dict

   .. py:method:: to_json() -> str
      :canonical: hlsfactory.data_packaging.CompleteHLSData.to_json

      .. autodoc2-docstring:: hlsfactory.data_packaging.CompleteHLSData.to_json

   .. py:method:: to_json_file(file_path: pathlib.Path) -> None
      :canonical: hlsfactory.data_packaging.CompleteHLSData.to_json_file

      .. autodoc2-docstring:: hlsfactory.data_packaging.CompleteHLSData.to_json_file

   .. py:method:: to_csv() -> str
      :canonical: hlsfactory.data_packaging.CompleteHLSData.to_csv

      .. autodoc2-docstring:: hlsfactory.data_packaging.CompleteHLSData.to_csv

   .. py:method:: to_csv_file(file_path: pathlib.Path) -> None
      :canonical: hlsfactory.data_packaging.CompleteHLSData.to_csv_file

      .. autodoc2-docstring:: hlsfactory.data_packaging.CompleteHLSData.to_csv_file

.. py:class:: DataAggregator
   :canonical: hlsfactory.data_packaging.DataAggregator

   Bases: :py:obj:`abc.ABC`

   .. py:attribute:: VENDER
      :canonical: hlsfactory.data_packaging.DataAggregator.VENDER
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregator.VENDER

   .. py:method:: gather_hls_design_data(design: hlsfactory.framework.Design) -> dict
      :canonical: hlsfactory.data_packaging.DataAggregator.gather_hls_design_data
      :abstractmethod:

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregator.gather_hls_design_data

   .. py:method:: gather_hls_synthesis_data(design: hlsfactory.framework.Design) -> dict
      :canonical: hlsfactory.data_packaging.DataAggregator.gather_hls_synthesis_data
      :abstractmethod:

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregator.gather_hls_synthesis_data

   .. py:method:: gather_implementation_data(design: hlsfactory.framework.Design) -> dict
      :canonical: hlsfactory.data_packaging.DataAggregator.gather_implementation_data
      :abstractmethod:

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregator.gather_implementation_data

   .. py:method:: gather_execution_data(design: hlsfactory.framework.Design) -> dict
      :canonical: hlsfactory.data_packaging.DataAggregator.gather_execution_data
      :abstractmethod:

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregator.gather_execution_data

   .. py:method:: gather_hls_synthesis_artifacts(design: hlsfactory.framework.Design) -> hlsfactory.data_packaging.InMemoryArchive
      :canonical: hlsfactory.data_packaging.DataAggregator.gather_hls_synthesis_artifacts
      :abstractmethod:

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregator.gather_hls_synthesis_artifacts

   .. py:method:: gather_all_data(design: hlsfactory.framework.Design, **kwargs) -> hlsfactory.data_packaging.CompleteHLSData
      :canonical: hlsfactory.data_packaging.DataAggregator.gather_all_data

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregator.gather_all_data

   .. py:method:: gather_multiple_designs(designs: list[hlsfactory.framework.Design], n_jobs: int = 1) -> list[hlsfactory.data_packaging.CompleteHLSData]
      :canonical: hlsfactory.data_packaging.DataAggregator.gather_multiple_designs

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregator.gather_multiple_designs

   .. py:method:: aggregated_data_to_csv(data: list[hlsfactory.data_packaging.CompleteHLSData]) -> str
      :canonical: hlsfactory.data_packaging.DataAggregator.aggregated_data_to_csv

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregator.aggregated_data_to_csv

   .. py:method:: aggregated_data_to_csv_file(data: list[hlsfactory.data_packaging.CompleteHLSData], file_path: pathlib.Path) -> None
      :canonical: hlsfactory.data_packaging.DataAggregator.aggregated_data_to_csv_file

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregator.aggregated_data_to_csv_file

   .. py:method:: aggregated_data_to_json(data: list[hlsfactory.data_packaging.CompleteHLSData]) -> str
      :canonical: hlsfactory.data_packaging.DataAggregator.aggregated_data_to_json

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregator.aggregated_data_to_json

   .. py:method:: aggregated_data_to_json_file(data: list[hlsfactory.data_packaging.CompleteHLSData], file_path: pathlib.Path) -> None
      :canonical: hlsfactory.data_packaging.DataAggregator.aggregated_data_to_json_file

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregator.aggregated_data_to_json_file

   .. py:method:: aggregated_data_to_archive(data: list[hlsfactory.data_packaging.CompleteHLSData], file_path: pathlib.Path) -> None
      :canonical: hlsfactory.data_packaging.DataAggregator.aggregated_data_to_archive

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregator.aggregated_data_to_archive

.. py:data:: ArtifactCollection
   :canonical: hlsfactory.data_packaging.ArtifactCollection
   :value: None

   .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactCollection

.. py:class:: ArtifactsXilinx(*args, **kwds)
   :canonical: hlsfactory.data_packaging.ArtifactsXilinx

   Bases: :py:obj:`enum.Enum`

   .. py:attribute:: IR
      :canonical: hlsfactory.data_packaging.ArtifactsXilinx.IR
      :value: 'ir'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsXilinx.IR

   .. py:attribute:: ADB
      :canonical: hlsfactory.data_packaging.ArtifactsXilinx.ADB
      :value: 'adb'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsXilinx.ADB

   .. py:attribute:: REPORT
      :canonical: hlsfactory.data_packaging.ArtifactsXilinx.REPORT
      :value: 'report'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsXilinx.REPORT

   .. py:attribute:: HDL
      :canonical: hlsfactory.data_packaging.ArtifactsXilinx.HDL
      :value: 'hdl'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsXilinx.HDL

   .. py:attribute:: IP
      :canonical: hlsfactory.data_packaging.ArtifactsXilinx.IP
      :value: 'ip'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsXilinx.IP

.. py:function:: get_file_in_root(dir_fp: pathlib.Path, file_name: str) -> dict
   :canonical: hlsfactory.data_packaging.get_file_in_root

   .. autodoc2-docstring:: hlsfactory.data_packaging.get_file_in_root

.. py:class:: JSONFileDataAggregator
   :canonical: hlsfactory.data_packaging.JSONFileDataAggregator

   Bases: :py:obj:`hlsfactory.data_packaging.DataAggregator`

   .. autodoc2-docstring:: hlsfactory.data_packaging.JSONFileDataAggregator

   .. py:method:: gather_hls_design_data(design: hlsfactory.framework.Design, **_kwargs) -> dict
      :canonical: hlsfactory.data_packaging.JSONFileDataAggregator.gather_hls_design_data

      .. autodoc2-docstring:: hlsfactory.data_packaging.JSONFileDataAggregator.gather_hls_design_data

   .. py:method:: gather_hls_synthesis_data(design: hlsfactory.framework.Design, **_kwargs) -> dict
      :canonical: hlsfactory.data_packaging.JSONFileDataAggregator.gather_hls_synthesis_data

      .. autodoc2-docstring:: hlsfactory.data_packaging.JSONFileDataAggregator.gather_hls_synthesis_data

   .. py:method:: gather_implementation_data(design: hlsfactory.framework.Design, **_kwargs) -> dict
      :canonical: hlsfactory.data_packaging.JSONFileDataAggregator.gather_implementation_data

      .. autodoc2-docstring:: hlsfactory.data_packaging.JSONFileDataAggregator.gather_implementation_data

   .. py:method:: gather_execution_data(design: hlsfactory.framework.Design, **_kwargs) -> dict
      :canonical: hlsfactory.data_packaging.JSONFileDataAggregator.gather_execution_data

      .. autodoc2-docstring:: hlsfactory.data_packaging.JSONFileDataAggregator.gather_execution_data

.. py:class:: DataAggregatorXilinx
   :canonical: hlsfactory.data_packaging.DataAggregatorXilinx

   Bases: :py:obj:`hlsfactory.data_packaging.JSONFileDataAggregator`

   .. py:attribute:: VENDER
      :canonical: hlsfactory.data_packaging.DataAggregatorXilinx.VENDER
      :value: 'Xilinx'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXilinx.VENDER

   .. py:method:: gather_hls_synthesis_artifacts_data(design: hlsfactory.framework.Design, artifacts_to_extract: set[hlsfactory.data_packaging.ArtifactsXilinx] = {ArtifactsXilinx.IR, ArtifactsXilinx.ADB, ArtifactsXilinx.REPORT, ArtifactsXilinx.HDL, ArtifactsXilinx.IP}, error_if_missing_data: bool = False) -> hlsfactory.data_packaging.ArtifactCollection
      :canonical: hlsfactory.data_packaging.DataAggregatorXilinx.gather_hls_synthesis_artifacts_data

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXilinx.gather_hls_synthesis_artifacts_data

   .. py:method:: gather_hls_synthesis_artifacts(design: hlsfactory.framework.Design, artifacts_to_extract: set[hlsfactory.data_packaging.ArtifactsXilinx] = {ArtifactsXilinx.IR, ArtifactsXilinx.ADB, ArtifactsXilinx.REPORT, ArtifactsXilinx.HDL, ArtifactsXilinx.IP}, error_if_missing_data: bool = False) -> hlsfactory.data_packaging.InMemoryArchive
      :canonical: hlsfactory.data_packaging.DataAggregatorXilinx.gather_hls_synthesis_artifacts

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXilinx.gather_hls_synthesis_artifacts

.. py:class:: ArtifactAggregator
   :canonical: hlsfactory.data_packaging.ArtifactAggregator

   Bases: :py:obj:`hlsfactory.data_packaging.JSONFileDataAggregator`

   .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactAggregator

   .. py:attribute:: ARTIFACTS
      :canonical: hlsfactory.data_packaging.ArtifactAggregator.ARTIFACTS
      :type: typing.ClassVar[type[enum.Enum]]
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactAggregator.ARTIFACTS

   .. py:attribute:: ARTIFACT_PATTERNS
      :canonical: hlsfactory.data_packaging.ArtifactAggregator.ARTIFACT_PATTERNS
      :type: typing.ClassVar[dict[enum.Enum, tuple[str, ...]]]
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactAggregator.ARTIFACT_PATTERNS

   .. py:attribute:: SYNTH_FLOW_NAME
      :canonical: hlsfactory.data_packaging.ArtifactAggregator.SYNTH_FLOW_NAME
      :type: typing.ClassVar[str]
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactAggregator.SYNTH_FLOW_NAME

   .. py:method:: _synthesis_incomplete(design: hlsfactory.framework.Design) -> bool
      :canonical: hlsfactory.data_packaging.ArtifactAggregator._synthesis_incomplete

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactAggregator._synthesis_incomplete

   .. py:method:: gather_hls_synthesis_artifacts_data(design: hlsfactory.framework.Design, artifacts_to_extract: collections.abc.Iterable[enum.Enum] | None = None, error_if_missing_data: bool = False) -> hlsfactory.data_packaging.ArtifactCollection
      :canonical: hlsfactory.data_packaging.ArtifactAggregator.gather_hls_synthesis_artifacts_data

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactAggregator.gather_hls_synthesis_artifacts_data

   .. py:method:: gather_hls_synthesis_artifacts(design: hlsfactory.framework.Design, artifacts_to_extract: collections.abc.Iterable[enum.Enum] | None = None, error_if_missing_data: bool = False, **_kwargs) -> hlsfactory.data_packaging.InMemoryArchive
      :canonical: hlsfactory.data_packaging.ArtifactAggregator.gather_hls_synthesis_artifacts

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactAggregator.gather_hls_synthesis_artifacts

.. py:class:: ArtifactsXLS(*args, **kwds)
   :canonical: hlsfactory.data_packaging.ArtifactsXLS

   Bases: :py:obj:`enum.Enum`

   .. py:attribute:: IR
      :canonical: hlsfactory.data_packaging.ArtifactsXLS.IR
      :value: 'ir'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsXLS.IR

   .. py:attribute:: RTL
      :canonical: hlsfactory.data_packaging.ArtifactsXLS.RTL
      :value: 'rtl'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsXLS.RTL

   .. py:attribute:: METRICS
      :canonical: hlsfactory.data_packaging.ArtifactsXLS.METRICS
      :value: 'metrics'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsXLS.METRICS

   .. py:attribute:: INTERFACE
      :canonical: hlsfactory.data_packaging.ArtifactsXLS.INTERFACE
      :value: 'interface'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsXLS.INTERFACE

   .. py:attribute:: OPTIONS
      :canonical: hlsfactory.data_packaging.ArtifactsXLS.OPTIONS
      :value: 'options'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsXLS.OPTIONS

   .. py:attribute:: SOURCE_MAP
      :canonical: hlsfactory.data_packaging.ArtifactsXLS.SOURCE_MAP
      :value: 'source_map'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsXLS.SOURCE_MAP

   .. py:attribute:: DEBUG
      :canonical: hlsfactory.data_packaging.ArtifactsXLS.DEBUG
      :value: 'debug'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsXLS.DEBUG

.. py:class:: DataAggregatorXLS
   :canonical: hlsfactory.data_packaging.DataAggregatorXLS

   Bases: :py:obj:`hlsfactory.data_packaging.ArtifactAggregator`

   .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXLS

   .. py:attribute:: VENDER
      :canonical: hlsfactory.data_packaging.DataAggregatorXLS.VENDER
      :value: 'XLS'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXLS.VENDER

   .. py:attribute:: ARTIFACTS
      :canonical: hlsfactory.data_packaging.DataAggregatorXLS.ARTIFACTS
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXLS.ARTIFACTS

   .. py:attribute:: SYNTH_FLOW_NAME
      :canonical: hlsfactory.data_packaging.DataAggregatorXLS.SYNTH_FLOW_NAME
      :value: 'XLSHLSSynthFlow'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXLS.SYNTH_FLOW_NAME

   .. py:attribute:: ARTIFACT_PATTERNS
      :canonical: hlsfactory.data_packaging.DataAggregatorXLS.ARTIFACT_PATTERNS
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXLS.ARTIFACT_PATTERNS

.. py:class:: ArtifactsCatapult(*args, **kwds)
   :canonical: hlsfactory.data_packaging.ArtifactsCatapult

   Bases: :py:obj:`enum.Enum`

   .. py:attribute:: REPORT
      :canonical: hlsfactory.data_packaging.ArtifactsCatapult.REPORT
      :value: 'report'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsCatapult.REPORT

   .. py:attribute:: RTL
      :canonical: hlsfactory.data_packaging.ArtifactsCatapult.RTL
      :value: 'rtl'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsCatapult.RTL

.. py:class:: DataAggregatorCatapult
   :canonical: hlsfactory.data_packaging.DataAggregatorCatapult

   Bases: :py:obj:`hlsfactory.data_packaging.ArtifactAggregator`

   .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorCatapult

   .. py:attribute:: VENDER
      :canonical: hlsfactory.data_packaging.DataAggregatorCatapult.VENDER
      :value: 'Catapult'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorCatapult.VENDER

   .. py:attribute:: ARTIFACTS
      :canonical: hlsfactory.data_packaging.DataAggregatorCatapult.ARTIFACTS
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorCatapult.ARTIFACTS

   .. py:attribute:: SYNTH_FLOW_NAME
      :canonical: hlsfactory.data_packaging.DataAggregatorCatapult.SYNTH_FLOW_NAME
      :value: 'CatapultHLSSynthFlow'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorCatapult.SYNTH_FLOW_NAME

   .. py:attribute:: ARTIFACT_PATTERNS
      :canonical: hlsfactory.data_packaging.DataAggregatorCatapult.ARTIFACT_PATTERNS
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorCatapult.ARTIFACT_PATTERNS

.. py:class:: ArtifactsStratus(*args, **kwds)
   :canonical: hlsfactory.data_packaging.ArtifactsStratus

   Bases: :py:obj:`enum.Enum`

   .. py:attribute:: RTL
      :canonical: hlsfactory.data_packaging.ArtifactsStratus.RTL
      :value: 'rtl'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsStratus.RTL

   .. py:attribute:: REPORT
      :canonical: hlsfactory.data_packaging.ArtifactsStratus.REPORT
      :value: 'report'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsStratus.REPORT

   .. py:attribute:: LOG
      :canonical: hlsfactory.data_packaging.ArtifactsStratus.LOG
      :value: 'log'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsStratus.LOG

.. py:class:: DataAggregatorStratus
   :canonical: hlsfactory.data_packaging.DataAggregatorStratus

   Bases: :py:obj:`hlsfactory.data_packaging.ArtifactAggregator`

   .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorStratus

   .. py:attribute:: VENDER
      :canonical: hlsfactory.data_packaging.DataAggregatorStratus.VENDER
      :value: 'Stratus'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorStratus.VENDER

   .. py:attribute:: ARTIFACTS
      :canonical: hlsfactory.data_packaging.DataAggregatorStratus.ARTIFACTS
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorStratus.ARTIFACTS

   .. py:attribute:: SYNTH_FLOW_NAME
      :canonical: hlsfactory.data_packaging.DataAggregatorStratus.SYNTH_FLOW_NAME
      :value: 'StratusHLSSynthFlow'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorStratus.SYNTH_FLOW_NAME

   .. py:attribute:: ARTIFACT_PATTERNS
      :canonical: hlsfactory.data_packaging.DataAggregatorStratus.ARTIFACT_PATTERNS
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorStratus.ARTIFACT_PATTERNS

.. py:class:: ArtifactsIntel(*args, **kwds)
   :canonical: hlsfactory.data_packaging.ArtifactsIntel

   Bases: :py:obj:`enum.Enum`

   .. py:attribute:: REPORT
      :canonical: hlsfactory.data_packaging.ArtifactsIntel.REPORT
      :value: 'report'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsIntel.REPORT

   .. py:attribute:: HDL
      :canonical: hlsfactory.data_packaging.ArtifactsIntel.HDL
      :value: 'hdl'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsIntel.HDL

   .. py:attribute:: QUARTUS
      :canonical: hlsfactory.data_packaging.ArtifactsIntel.QUARTUS
      :value: 'quartus'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsIntel.QUARTUS

.. py:class:: DataAggregatorIntel
   :canonical: hlsfactory.data_packaging.DataAggregatorIntel

   Bases: :py:obj:`hlsfactory.data_packaging.ArtifactAggregator`

   .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorIntel

   .. py:attribute:: VENDER
      :canonical: hlsfactory.data_packaging.DataAggregatorIntel.VENDER
      :value: 'Intel'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorIntel.VENDER

   .. py:attribute:: ARTIFACTS
      :canonical: hlsfactory.data_packaging.DataAggregatorIntel.ARTIFACTS
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorIntel.ARTIFACTS

   .. py:attribute:: SYNTH_FLOW_NAME
      :canonical: hlsfactory.data_packaging.DataAggregatorIntel.SYNTH_FLOW_NAME
      :value: 'IntelHLSSynthFlow'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorIntel.SYNTH_FLOW_NAME

   .. py:attribute:: ARTIFACT_PATTERNS
      :canonical: hlsfactory.data_packaging.DataAggregatorIntel.ARTIFACT_PATTERNS
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorIntel.ARTIFACT_PATTERNS

.. py:class:: ArtifactsAltera(*args, **kwds)
   :canonical: hlsfactory.data_packaging.ArtifactsAltera

   Bases: :py:obj:`enum.Enum`

   .. py:attribute:: REPORT
      :canonical: hlsfactory.data_packaging.ArtifactsAltera.REPORT
      :value: 'report'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsAltera.REPORT

   .. py:attribute:: INTERFACE
      :canonical: hlsfactory.data_packaging.ArtifactsAltera.INTERFACE
      :value: 'interface'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsAltera.INTERFACE

   .. py:attribute:: HDL
      :canonical: hlsfactory.data_packaging.ArtifactsAltera.HDL
      :value: 'hdl'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsAltera.HDL

   .. py:attribute:: LOG
      :canonical: hlsfactory.data_packaging.ArtifactsAltera.LOG
      :value: 'log'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsAltera.LOG

.. py:class:: DataAggregatorAltera
   :canonical: hlsfactory.data_packaging.DataAggregatorAltera

   Bases: :py:obj:`hlsfactory.data_packaging.ArtifactAggregator`

   .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorAltera

   .. py:attribute:: VENDER
      :canonical: hlsfactory.data_packaging.DataAggregatorAltera.VENDER
      :value: 'Altera'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorAltera.VENDER

   .. py:attribute:: ARTIFACTS
      :canonical: hlsfactory.data_packaging.DataAggregatorAltera.ARTIFACTS
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorAltera.ARTIFACTS

   .. py:attribute:: SYNTH_FLOW_NAME
      :canonical: hlsfactory.data_packaging.DataAggregatorAltera.SYNTH_FLOW_NAME
      :value: 'AlteraHLSSynthFlow'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorAltera.SYNTH_FLOW_NAME

   .. py:attribute:: ARTIFACT_PATTERNS
      :canonical: hlsfactory.data_packaging.DataAggregatorAltera.ARTIFACT_PATTERNS
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorAltera.ARTIFACT_PATTERNS

.. py:class:: ArtifactsVitisModern(*args, **kwds)
   :canonical: hlsfactory.data_packaging.ArtifactsVitisModern

   Bases: :py:obj:`enum.Enum`

   .. py:attribute:: REPORT
      :canonical: hlsfactory.data_packaging.ArtifactsVitisModern.REPORT
      :value: 'report'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsVitisModern.REPORT

   .. py:attribute:: HDL
      :canonical: hlsfactory.data_packaging.ArtifactsVitisModern.HDL
      :value: 'hdl'

      .. autodoc2-docstring:: hlsfactory.data_packaging.ArtifactsVitisModern.HDL

.. py:class:: DataAggregatorVitisModern
   :canonical: hlsfactory.data_packaging.DataAggregatorVitisModern

   Bases: :py:obj:`hlsfactory.data_packaging.ArtifactAggregator`

   .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorVitisModern

   .. py:attribute:: VENDER
      :canonical: hlsfactory.data_packaging.DataAggregatorVitisModern.VENDER
      :value: 'Xilinx'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorVitisModern.VENDER

   .. py:attribute:: ARTIFACTS
      :canonical: hlsfactory.data_packaging.DataAggregatorVitisModern.ARTIFACTS
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorVitisModern.ARTIFACTS

   .. py:attribute:: SYNTH_FLOW_NAME
      :canonical: hlsfactory.data_packaging.DataAggregatorVitisModern.SYNTH_FLOW_NAME
      :value: 'VitisHLSModernSynthFlow'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorVitisModern.SYNTH_FLOW_NAME

   .. py:attribute:: ARTIFACT_PATTERNS
      :canonical: hlsfactory.data_packaging.DataAggregatorVitisModern.ARTIFACT_PATTERNS
      :value: None

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorVitisModern.ARTIFACT_PATTERNS
