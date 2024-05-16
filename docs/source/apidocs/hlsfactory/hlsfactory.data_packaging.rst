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
   * - :py:obj:`DataAggregatorXilinx <hlsfactory.data_packaging.DataAggregatorXilinx>`
     -

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

.. py:class:: ArtifactsXilinx
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

.. py:class:: DataAggregatorXilinx
   :canonical: hlsfactory.data_packaging.DataAggregatorXilinx

   Bases: :py:obj:`hlsfactory.data_packaging.DataAggregator`

   .. py:attribute:: VENDER
      :canonical: hlsfactory.data_packaging.DataAggregatorXilinx.VENDER
      :value: 'Xilinx'

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXilinx.VENDER

   .. py:method:: gather_hls_design_data(design: hlsfactory.framework.Design) -> dict
      :canonical: hlsfactory.data_packaging.DataAggregatorXilinx.gather_hls_design_data

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXilinx.gather_hls_design_data

   .. py:method:: gather_hls_synthesis_data(design: hlsfactory.framework.Design) -> dict
      :canonical: hlsfactory.data_packaging.DataAggregatorXilinx.gather_hls_synthesis_data

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXilinx.gather_hls_synthesis_data

   .. py:method:: gather_implementation_data(design: hlsfactory.framework.Design) -> dict
      :canonical: hlsfactory.data_packaging.DataAggregatorXilinx.gather_implementation_data

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXilinx.gather_implementation_data

   .. py:method:: gather_execution_data(design: hlsfactory.framework.Design) -> dict
      :canonical: hlsfactory.data_packaging.DataAggregatorXilinx.gather_execution_data

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXilinx.gather_execution_data

   .. py:method:: gather_hls_synthesis_artifacts_data(design: hlsfactory.framework.Design, artifacts_to_extract: set[hlsfactory.data_packaging.ArtifactsXilinx] = {ArtifactsXilinx.IR, ArtifactsXilinx.ADB, ArtifactsXilinx.REPORT, ArtifactsXilinx.HDL, ArtifactsXilinx.IP}, error_if_missing_data: bool = True) -> hlsfactory.data_packaging.ArtifactCollection
      :canonical: hlsfactory.data_packaging.DataAggregatorXilinx.gather_hls_synthesis_artifacts_data

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXilinx.gather_hls_synthesis_artifacts_data

   .. py:method:: gather_hls_synthesis_artifacts(design: hlsfactory.framework.Design, artifacts_to_extract: set[hlsfactory.data_packaging.ArtifactsXilinx] = {ArtifactsXilinx.IR, ArtifactsXilinx.ADB, ArtifactsXilinx.REPORT, ArtifactsXilinx.HDL, ArtifactsXilinx.IP}, error_if_missing_data: bool = True) -> hlsfactory.data_packaging.InMemoryArchive
      :canonical: hlsfactory.data_packaging.DataAggregatorXilinx.gather_hls_synthesis_artifacts

      .. autodoc2-docstring:: hlsfactory.data_packaging.DataAggregatorXilinx.gather_hls_synthesis_artifacts
