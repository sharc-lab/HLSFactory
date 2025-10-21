:py:mod:`hlsfactory.framework`
==============================

.. py:module:: hlsfactory.framework

.. autodoc2-docstring:: hlsfactory.framework
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`DesignStage <hlsfactory.framework.DesignStage>`
     -
   * - :py:obj:`Design <hlsfactory.framework.Design>`
     - .. autodoc2-docstring:: hlsfactory.framework.Design
          :summary:
   * - :py:obj:`DesignDataset <hlsfactory.framework.DesignDataset>`
     - .. autodoc2-docstring:: hlsfactory.framework.DesignDataset
          :summary:
   * - :py:obj:`Flow <hlsfactory.framework.Flow>`
     -
   * - :py:obj:`Frontend <hlsfactory.framework.Frontend>`
     -
   * - :py:obj:`ToolFlow <hlsfactory.framework.ToolFlow>`
     -

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`all_files_in_dir <hlsfactory.framework.all_files_in_dir>`
     - .. autodoc2-docstring:: hlsfactory.framework.all_files_in_dir
          :summary:
   * - :py:obj:`filter_files_by_ext <hlsfactory.framework.filter_files_by_ext>`
     - .. autodoc2-docstring:: hlsfactory.framework.filter_files_by_ext
          :summary:
   * - :py:obj:`check_n_jobs_cpu_affinity <hlsfactory.framework.check_n_jobs_cpu_affinity>`
     - .. autodoc2-docstring:: hlsfactory.framework.check_n_jobs_cpu_affinity
          :summary:
   * - :py:obj:`count_designs_in_dataset_collection <hlsfactory.framework.count_designs_in_dataset_collection>`
     - .. autodoc2-docstring:: hlsfactory.framework.count_designs_in_dataset_collection
          :summary:
   * - :py:obj:`count_total_designs_in_dataset_collection <hlsfactory.framework.count_total_designs_in_dataset_collection>`
     - .. autodoc2-docstring:: hlsfactory.framework.count_total_designs_in_dataset_collection
          :summary:
   * - :py:obj:`worker_init <hlsfactory.framework.worker_init>`
     - .. autodoc2-docstring:: hlsfactory.framework.worker_init
          :summary:

Data
~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`EXTENSIONS_CPP <hlsfactory.framework.EXTENSIONS_CPP>`
     - .. autodoc2-docstring:: hlsfactory.framework.EXTENSIONS_CPP
          :summary:
   * - :py:obj:`EXTENTIONS_TCL <hlsfactory.framework.EXTENTIONS_TCL>`
     - .. autodoc2-docstring:: hlsfactory.framework.EXTENTIONS_TCL
          :summary:
   * - :py:obj:`EXTENSIONS_PYTHON <hlsfactory.framework.EXTENSIONS_PYTHON>`
     - .. autodoc2-docstring:: hlsfactory.framework.EXTENSIONS_PYTHON
          :summary:
   * - :py:obj:`NAMES_MAKEFILE <hlsfactory.framework.NAMES_MAKEFILE>`
     - .. autodoc2-docstring:: hlsfactory.framework.NAMES_MAKEFILE
          :summary:
   * - :py:obj:`EXTENTIONS_MAKEFILE <hlsfactory.framework.EXTENTIONS_MAKEFILE>`
     - .. autodoc2-docstring:: hlsfactory.framework.EXTENTIONS_MAKEFILE
          :summary:
   * - :py:obj:`DesignDatasetCollection <hlsfactory.framework.DesignDatasetCollection>`
     - .. autodoc2-docstring:: hlsfactory.framework.DesignDatasetCollection
          :summary:

API
~~~

.. py:data:: EXTENSIONS_CPP
   :canonical: hlsfactory.framework.EXTENSIONS_CPP
   :value: ['.cpp', '.cc', '.hpp', '.h']

   .. autodoc2-docstring:: hlsfactory.framework.EXTENSIONS_CPP

.. py:data:: EXTENTIONS_TCL
   :canonical: hlsfactory.framework.EXTENTIONS_TCL
   :value: ['.tcl']

   .. autodoc2-docstring:: hlsfactory.framework.EXTENTIONS_TCL

.. py:data:: EXTENSIONS_PYTHON
   :canonical: hlsfactory.framework.EXTENSIONS_PYTHON
   :value: ['.py']

   .. autodoc2-docstring:: hlsfactory.framework.EXTENSIONS_PYTHON

.. py:data:: NAMES_MAKEFILE
   :canonical: hlsfactory.framework.NAMES_MAKEFILE
   :value: ['Makefile', 'makefile']

   .. autodoc2-docstring:: hlsfactory.framework.NAMES_MAKEFILE

.. py:data:: EXTENTIONS_MAKEFILE
   :canonical: hlsfactory.framework.EXTENTIONS_MAKEFILE
   :value: ['.mk']

   .. autodoc2-docstring:: hlsfactory.framework.EXTENTIONS_MAKEFILE

.. py:function:: all_files_in_dir(dir_path: pathlib.Path) -> list[pathlib.Path]
   :canonical: hlsfactory.framework.all_files_in_dir

   .. autodoc2-docstring:: hlsfactory.framework.all_files_in_dir

.. py:function:: filter_files_by_ext(files: list[pathlib.Path], ext: str) -> list[pathlib.Path]
   :canonical: hlsfactory.framework.filter_files_by_ext

   .. autodoc2-docstring:: hlsfactory.framework.filter_files_by_ext

.. py:class:: DesignStage(*args, **kwds)
   :canonical: hlsfactory.framework.DesignStage

   Bases: :py:obj:`enum.Enum`

   .. py:attribute:: ABSTRACT
      :canonical: hlsfactory.framework.DesignStage.ABSTRACT
      :value: 'auto(...)'

      .. autodoc2-docstring:: hlsfactory.framework.DesignStage.ABSTRACT

   .. py:attribute:: CONCRETE
      :canonical: hlsfactory.framework.DesignStage.CONCRETE
      :value: 'auto(...)'

      .. autodoc2-docstring:: hlsfactory.framework.DesignStage.CONCRETE

.. py:class:: Design(name: str, dir_path: pathlib.Path)
   :canonical: hlsfactory.framework.Design

   .. autodoc2-docstring:: hlsfactory.framework.Design

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.framework.Design.__init__

   .. py:method:: __repr__() -> str
      :canonical: hlsfactory.framework.Design.__repr__

   .. py:property:: all_files
      :canonical: hlsfactory.framework.Design.all_files
      :type: list[pathlib.Path]

      .. autodoc2-docstring:: hlsfactory.framework.Design.all_files

   .. py:property:: tcl_files
      :canonical: hlsfactory.framework.Design.tcl_files
      :type: list[pathlib.Path]

      .. autodoc2-docstring:: hlsfactory.framework.Design.tcl_files

   .. py:property:: cpp_source_files
      :canonical: hlsfactory.framework.Design.cpp_source_files
      :type: list[pathlib.Path]

      .. autodoc2-docstring:: hlsfactory.framework.Design.cpp_source_files

   .. py:method:: rename(new_name: str) -> hlsfactory.framework.Design
      :canonical: hlsfactory.framework.Design.rename

      .. autodoc2-docstring:: hlsfactory.framework.Design.rename

   .. py:method:: move_to_new_parent_dir(new_parent_dir: pathlib.Path) -> hlsfactory.framework.Design
      :canonical: hlsfactory.framework.Design.move_to_new_parent_dir

      .. autodoc2-docstring:: hlsfactory.framework.Design.move_to_new_parent_dir

   .. py:method:: copy_to_new_parent_dir(new_parent_dir: pathlib.Path) -> hlsfactory.framework.Design
      :canonical: hlsfactory.framework.Design.copy_to_new_parent_dir

      .. autodoc2-docstring:: hlsfactory.framework.Design.copy_to_new_parent_dir

   .. py:method:: copy_and_rename_to_new_parent_dir(new_name: str, new_parent_dir: pathlib.Path) -> hlsfactory.framework.Design
      :canonical: hlsfactory.framework.Design.copy_and_rename_to_new_parent_dir

      .. autodoc2-docstring:: hlsfactory.framework.Design.copy_and_rename_to_new_parent_dir

.. py:class:: DesignDataset(name: str, dataset_dir: pathlib.Path, designs: list[hlsfactory.framework.Design])
   :canonical: hlsfactory.framework.DesignDataset

   .. autodoc2-docstring:: hlsfactory.framework.DesignDataset

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.framework.DesignDataset.__init__

   .. py:method:: __repr__() -> str
      :canonical: hlsfactory.framework.DesignDataset.__repr__

   .. py:method:: from_dir(name: str, dir_path: pathlib.Path, exclude_dir_filter: None | collections.abc.Callable[[pathlib.Path], bool] = None) -> hlsfactory.framework.DesignDataset
      :canonical: hlsfactory.framework.DesignDataset.from_dir
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.framework.DesignDataset.from_dir

   .. py:method:: from_empty_dir(name: str, work_dir: pathlib.Path) -> hlsfactory.framework.DesignDataset
      :canonical: hlsfactory.framework.DesignDataset.from_empty_dir
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.framework.DesignDataset.from_empty_dir

   .. py:method:: add_design(design: hlsfactory.framework.Design) -> hlsfactory.framework.DesignDataset
      :canonical: hlsfactory.framework.DesignDataset.add_design

      .. autodoc2-docstring:: hlsfactory.framework.DesignDataset.add_design

   .. py:method:: add_designs(designs: list[hlsfactory.framework.Design]) -> hlsfactory.framework.DesignDataset
      :canonical: hlsfactory.framework.DesignDataset.add_designs

      .. autodoc2-docstring:: hlsfactory.framework.DesignDataset.add_designs

   .. py:method:: add_design_copy(design: hlsfactory.framework.Design) -> hlsfactory.framework.DesignDataset
      :canonical: hlsfactory.framework.DesignDataset.add_design_copy

      .. autodoc2-docstring:: hlsfactory.framework.DesignDataset.add_design_copy

   .. py:method:: add_designs_copy(designs: list[hlsfactory.framework.Design]) -> hlsfactory.framework.DesignDataset
      :canonical: hlsfactory.framework.DesignDataset.add_designs_copy

      .. autodoc2-docstring:: hlsfactory.framework.DesignDataset.add_designs_copy

   .. py:method:: copy_dataset(work_dir: pathlib.Path) -> hlsfactory.framework.DesignDataset
      :canonical: hlsfactory.framework.DesignDataset.copy_dataset

      .. autodoc2-docstring:: hlsfactory.framework.DesignDataset.copy_dataset

   .. py:method:: copy_and_rename(new_name: str, work_dir: pathlib.Path) -> hlsfactory.framework.DesignDataset
      :canonical: hlsfactory.framework.DesignDataset.copy_and_rename

      .. autodoc2-docstring:: hlsfactory.framework.DesignDataset.copy_and_rename

.. py:function:: check_n_jobs_cpu_affinity(n_jobs: int, cpu_affinity: list[int] | None) -> None
   :canonical: hlsfactory.framework.check_n_jobs_cpu_affinity

   .. autodoc2-docstring:: hlsfactory.framework.check_n_jobs_cpu_affinity

.. py:data:: DesignDatasetCollection
   :canonical: hlsfactory.framework.DesignDatasetCollection
   :value: None

   .. autodoc2-docstring:: hlsfactory.framework.DesignDatasetCollection

.. py:function:: count_designs_in_dataset_collection(design_datasets: hlsfactory.framework.DesignDatasetCollection) -> dict[str, int]
   :canonical: hlsfactory.framework.count_designs_in_dataset_collection

   .. autodoc2-docstring:: hlsfactory.framework.count_designs_in_dataset_collection

.. py:function:: count_total_designs_in_dataset_collection(design_datasets: hlsfactory.framework.DesignDatasetCollection) -> int
   :canonical: hlsfactory.framework.count_total_designs_in_dataset_collection

   .. autodoc2-docstring:: hlsfactory.framework.count_total_designs_in_dataset_collection

.. py:function:: worker_init(core_queue: multiprocessing.Queue) -> None
   :canonical: hlsfactory.framework.worker_init

   .. autodoc2-docstring:: hlsfactory.framework.worker_init

.. py:class:: Flow(work_dir: pathlib.Path)
   :canonical: hlsfactory.framework.Flow

   Bases: :py:obj:`abc.ABC`

   .. py:attribute:: name
      :canonical: hlsfactory.framework.Flow.name
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.framework.Flow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.framework.Flow.execute
      :abstractmethod:

      .. autodoc2-docstring:: hlsfactory.framework.Flow.execute

   .. py:method:: execute_multiple_designs(designs: list[hlsfactory.framework.Design], n_jobs: int = 1, cpu_affinity: list[int] | None = None, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.framework.Flow.execute_multiple_designs

      .. autodoc2-docstring:: hlsfactory.framework.Flow.execute_multiple_designs

   .. py:method:: default_new_dataset_name_fn() -> collections.abc.Callable[[str], str]
      :canonical: hlsfactory.framework.Flow.default_new_dataset_name_fn

      .. autodoc2-docstring:: hlsfactory.framework.Flow.default_new_dataset_name_fn

   .. py:method:: execute_multiple_design_datasets_naive_parallel(design_datasets: hlsfactory.framework.DesignDatasetCollection, copy_dataset: bool, new_dataset_name_fn: collections.abc.Callable[[str], str] | None = None, n_jobs: int = 1, cpu_affinity: list[int] | None = None, timeout: float | None = None) -> hlsfactory.framework.DesignDatasetCollection
      :canonical: hlsfactory.framework.Flow.execute_multiple_design_datasets_naive_parallel

      .. autodoc2-docstring:: hlsfactory.framework.Flow.execute_multiple_design_datasets_naive_parallel

   .. py:method:: execute_multiple_design_datasets_fine_grained_parallel(design_datasets: hlsfactory.framework.DesignDatasetCollection, copy_dataset: bool, new_dataset_name_fn: collections.abc.Callable[[str], str] | None = None, n_jobs: int = 1, cpu_affinity: list[int] | None = None, par_chunksize: int | None = 1, timeout: float | None = None) -> hlsfactory.framework.DesignDatasetCollection
      :canonical: hlsfactory.framework.Flow.execute_multiple_design_datasets_fine_grained_parallel

      .. autodoc2-docstring:: hlsfactory.framework.Flow.execute_multiple_design_datasets_fine_grained_parallel

.. py:class:: Frontend(work_dir: pathlib.Path)
   :canonical: hlsfactory.framework.Frontend

   Bases: :py:obj:`hlsfactory.framework.Flow`

.. py:class:: ToolFlow(work_dir: pathlib.Path)
   :canonical: hlsfactory.framework.ToolFlow

   Bases: :py:obj:`hlsfactory.framework.Flow`
