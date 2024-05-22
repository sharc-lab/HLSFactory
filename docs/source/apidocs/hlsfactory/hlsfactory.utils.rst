:py:mod:`hlsfactory.utils`
==========================

.. py:module:: hlsfactory.utils

.. autodoc2-docstring:: hlsfactory.utils
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`CallToolResult <hlsfactory.utils.CallToolResult>`
     - .. autodoc2-docstring:: hlsfactory.utils.CallToolResult
          :summary:
   * - :py:obj:`FlowTimer <hlsfactory.utils.FlowTimer>`
     - .. autodoc2-docstring:: hlsfactory.utils.FlowTimer
          :summary:
   * - :py:obj:`DirSource <hlsfactory.utils.DirSource>`
     - .. autodoc2-docstring:: hlsfactory.utils.DirSource
          :summary:
   * - :py:obj:`ToolPathsSource <hlsfactory.utils.ToolPathsSource>`
     - .. autodoc2-docstring:: hlsfactory.utils.ToolPathsSource
          :summary:

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`call_tool <hlsfactory.utils.call_tool>`
     - .. autodoc2-docstring:: hlsfactory.utils.call_tool
          :summary:
   * - :py:obj:`terminate_process_and_children <hlsfactory.utils.terminate_process_and_children>`
     - .. autodoc2-docstring:: hlsfactory.utils.terminate_process_and_children
          :summary:
   * - :py:obj:`wait_for_files_creation <hlsfactory.utils.wait_for_files_creation>`
     - .. autodoc2-docstring:: hlsfactory.utils.wait_for_files_creation
          :summary:
   * - :py:obj:`find_bin_path <hlsfactory.utils.find_bin_path>`
     - .. autodoc2-docstring:: hlsfactory.utils.find_bin_path
          :summary:
   * - :py:obj:`log_execution_time_to_file <hlsfactory.utils.log_execution_time_to_file>`
     - .. autodoc2-docstring:: hlsfactory.utils.log_execution_time_to_file
          :summary:
   * - :py:obj:`serialize_methods_for_dataclass <hlsfactory.utils.serialize_methods_for_dataclass>`
     - .. autodoc2-docstring:: hlsfactory.utils.serialize_methods_for_dataclass
          :summary:
   * - :py:obj:`timeout_not_supported <hlsfactory.utils.timeout_not_supported>`
     - .. autodoc2-docstring:: hlsfactory.utils.timeout_not_supported
          :summary:
   * - :py:obj:`get_work_dir <hlsfactory.utils.get_work_dir>`
     - .. autodoc2-docstring:: hlsfactory.utils.get_work_dir
          :summary:
   * - :py:obj:`get_tool_paths <hlsfactory.utils.get_tool_paths>`
     - .. autodoc2-docstring:: hlsfactory.utils.get_tool_paths
          :summary:
   * - :py:obj:`remove_dir_if_exists <hlsfactory.utils.remove_dir_if_exists>`
     - .. autodoc2-docstring:: hlsfactory.utils.remove_dir_if_exists
          :summary:
   * - :py:obj:`remove_and_make_new_dir_if_exists <hlsfactory.utils.remove_and_make_new_dir_if_exists>`
     - .. autodoc2-docstring:: hlsfactory.utils.remove_and_make_new_dir_if_exists
          :summary:

Data
~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`T <hlsfactory.utils.T>`
     - .. autodoc2-docstring:: hlsfactory.utils.T
          :summary:

API
~~~

.. py:class:: CallToolResult
   :canonical: hlsfactory.utils.CallToolResult

   Bases: :py:obj:`enum.Enum`

   .. autodoc2-docstring:: hlsfactory.utils.CallToolResult

   .. py:attribute:: SUCCESS
      :canonical: hlsfactory.utils.CallToolResult.SUCCESS
      :value: 'auto(...)'

      .. autodoc2-docstring:: hlsfactory.utils.CallToolResult.SUCCESS

   .. py:attribute:: TIMEOUT
      :canonical: hlsfactory.utils.CallToolResult.TIMEOUT
      :value: 'auto(...)'

      .. autodoc2-docstring:: hlsfactory.utils.CallToolResult.TIMEOUT

   .. py:attribute:: ERROR
      :canonical: hlsfactory.utils.CallToolResult.ERROR
      :value: 'auto(...)'

      .. autodoc2-docstring:: hlsfactory.utils.CallToolResult.ERROR

.. py:function:: call_tool(cmd: str, cwd: pathlib.Path, shell: bool = False, timeout: float | None = None, log_output: bool = False, raise_on_error: bool = True) -> hlsfactory.utils.CallToolResult
   :canonical: hlsfactory.utils.call_tool

   .. autodoc2-docstring:: hlsfactory.utils.call_tool

.. py:function:: terminate_process_and_children(pid: int) -> None
   :canonical: hlsfactory.utils.terminate_process_and_children

   .. autodoc2-docstring:: hlsfactory.utils.terminate_process_and_children

.. py:function:: wait_for_files_creation(file_paths: list[pathlib.Path], timeout: float, poll_interval: float = 1) -> bool
   :canonical: hlsfactory.utils.wait_for_files_creation

   .. autodoc2-docstring:: hlsfactory.utils.wait_for_files_creation

.. py:function:: find_bin_path(cmd: str) -> str
   :canonical: hlsfactory.utils.find_bin_path

   .. autodoc2-docstring:: hlsfactory.utils.find_bin_path

.. py:function:: log_execution_time_to_file(design_dir: pathlib.Path, flow_name: str, t_0: float, t_1: float) -> None
   :canonical: hlsfactory.utils.log_execution_time_to_file

   .. autodoc2-docstring:: hlsfactory.utils.log_execution_time_to_file

.. py:class:: FlowTimer(flow_name: str, dir_path: pathlib.Path)
   :canonical: hlsfactory.utils.FlowTimer

   .. autodoc2-docstring:: hlsfactory.utils.FlowTimer

   .. rubric:: Initialization

   .. autodoc2-docstring:: hlsfactory.utils.FlowTimer.__init__

   .. py:method:: start() -> None
      :canonical: hlsfactory.utils.FlowTimer.start

      .. autodoc2-docstring:: hlsfactory.utils.FlowTimer.start

   .. py:method:: stop() -> None
      :canonical: hlsfactory.utils.FlowTimer.stop

      .. autodoc2-docstring:: hlsfactory.utils.FlowTimer.stop

   .. py:method:: log() -> None
      :canonical: hlsfactory.utils.FlowTimer.log

      .. autodoc2-docstring:: hlsfactory.utils.FlowTimer.log

   .. py:method:: __enter__() -> hlsfactory.utils.FlowTimer
      :canonical: hlsfactory.utils.FlowTimer.__enter__

      .. autodoc2-docstring:: hlsfactory.utils.FlowTimer.__enter__

   .. py:method:: __exit__(_exc_type, _exc_value, _traceback) -> None
      :canonical: hlsfactory.utils.FlowTimer.__exit__

      .. autodoc2-docstring:: hlsfactory.utils.FlowTimer.__exit__

.. py:data:: T
   :canonical: hlsfactory.utils.T
   :value: 'TypeVar(...)'

   .. autodoc2-docstring:: hlsfactory.utils.T

.. py:function:: serialize_methods_for_dataclass(cls: type[hlsfactory.utils.T]) -> type[hlsfactory.utils.T]
   :canonical: hlsfactory.utils.serialize_methods_for_dataclass

   .. autodoc2-docstring:: hlsfactory.utils.serialize_methods_for_dataclass

.. py:function:: timeout_not_supported(flow_name: str) -> None
   :canonical: hlsfactory.utils.timeout_not_supported

   .. autodoc2-docstring:: hlsfactory.utils.timeout_not_supported

.. py:class:: DirSource
   :canonical: hlsfactory.utils.DirSource

   Bases: :py:obj:`enum.Enum`

   .. autodoc2-docstring:: hlsfactory.utils.DirSource

   .. py:attribute:: ENVFILE
      :canonical: hlsfactory.utils.DirSource.ENVFILE
      :value: 'auto(...)'

      .. autodoc2-docstring:: hlsfactory.utils.DirSource.ENVFILE

   .. py:attribute:: ENV
      :canonical: hlsfactory.utils.DirSource.ENV
      :value: 'auto(...)'

      .. autodoc2-docstring:: hlsfactory.utils.DirSource.ENV

   .. py:attribute:: TEMP
      :canonical: hlsfactory.utils.DirSource.TEMP
      :value: 'auto(...)'

      .. autodoc2-docstring:: hlsfactory.utils.DirSource.TEMP

.. py:function:: get_work_dir(dir_source: hlsfactory.utils.DirSource = DirSource.ENVFILE) -> pathlib.Path
   :canonical: hlsfactory.utils.get_work_dir

   .. autodoc2-docstring:: hlsfactory.utils.get_work_dir

.. py:class:: ToolPathsSource
   :canonical: hlsfactory.utils.ToolPathsSource

   Bases: :py:obj:`enum.Enum`

   .. autodoc2-docstring:: hlsfactory.utils.ToolPathsSource

   .. py:attribute:: ENVFILE
      :canonical: hlsfactory.utils.ToolPathsSource.ENVFILE
      :value: 'auto(...)'

      .. autodoc2-docstring:: hlsfactory.utils.ToolPathsSource.ENVFILE

   .. py:attribute:: ENV
      :canonical: hlsfactory.utils.ToolPathsSource.ENV
      :value: 'auto(...)'

      .. autodoc2-docstring:: hlsfactory.utils.ToolPathsSource.ENV

.. py:function:: get_tool_paths(tool_paths_source: hlsfactory.utils.ToolPathsSource) -> tuple[pathlib.Path, pathlib.Path]
   :canonical: hlsfactory.utils.get_tool_paths

   .. autodoc2-docstring:: hlsfactory.utils.get_tool_paths

.. py:function:: remove_dir_if_exists(dir_path: pathlib.Path) -> None
   :canonical: hlsfactory.utils.remove_dir_if_exists

   .. autodoc2-docstring:: hlsfactory.utils.remove_dir_if_exists

.. py:function:: remove_and_make_new_dir_if_exists(dir_path: pathlib.Path) -> None
   :canonical: hlsfactory.utils.remove_and_make_new_dir_if_exists

   .. autodoc2-docstring:: hlsfactory.utils.remove_and_make_new_dir_if_exists
