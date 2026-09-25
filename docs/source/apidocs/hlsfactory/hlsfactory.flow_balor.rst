:py:mod:`hlsfactory.flow_balor`
===============================

.. py:module:: hlsfactory.flow_balor

.. autodoc2-docstring:: hlsfactory.flow_balor
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`BalorGraphData <hlsfactory.flow_balor.BalorGraphData>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphData
          :summary:
   * - :py:obj:`BalorGraphFlow <hlsfactory.flow_balor.BalorGraphFlow>`
     -

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`extract_dot_graph <hlsfactory.flow_balor.extract_dot_graph>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.extract_dot_graph
          :summary:
   * - :py:obj:`flatten_dot_quoted_newlines <hlsfactory.flow_balor.flatten_dot_quoted_newlines>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.flatten_dot_quoted_newlines
          :summary:
   * - :py:obj:`get_balor_install_dir <hlsfactory.flow_balor.get_balor_install_dir>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.get_balor_install_dir
          :summary:
   * - :py:obj:`find_graph_compiler <hlsfactory.flow_balor.find_graph_compiler>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.find_graph_compiler
          :summary:
   * - :py:obj:`validate_mode <hlsfactory.flow_balor.validate_mode>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.validate_mode
          :summary:
   * - :py:obj:`_parse_dot_attributes <hlsfactory.flow_balor._parse_dot_attributes>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor._parse_dot_attributes
          :summary:

Data
~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`HLSFACTORY_BALOR_PATH_ENV_VAR <hlsfactory.flow_balor.HLSFACTORY_BALOR_PATH_ENV_VAR>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.HLSFACTORY_BALOR_PATH_ENV_VAR
          :summary:
   * - :py:obj:`HLSFACTORY_BALOR_DOCKER_IMAGE_ENV_VAR <hlsfactory.flow_balor.HLSFACTORY_BALOR_DOCKER_IMAGE_ENV_VAR>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.HLSFACTORY_BALOR_DOCKER_IMAGE_ENV_VAR
          :summary:
   * - :py:obj:`DEFAULT_BALOR_DOCKER_IMAGE <hlsfactory.flow_balor.DEFAULT_BALOR_DOCKER_IMAGE>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.DEFAULT_BALOR_DOCKER_IMAGE
          :summary:
   * - :py:obj:`GRAPH_COMPILER_RELATIVE_PATH <hlsfactory.flow_balor.GRAPH_COMPILER_RELATIVE_PATH>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.GRAPH_COMPILER_RELATIVE_PATH
          :summary:
   * - :py:obj:`GRAPH_FILENAME <hlsfactory.flow_balor.GRAPH_FILENAME>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.GRAPH_FILENAME
          :summary:
   * - :py:obj:`GRAPH_SUMMARY_FILENAME <hlsfactory.flow_balor.GRAPH_SUMMARY_FILENAME>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.GRAPH_SUMMARY_FILENAME
          :summary:
   * - :py:obj:`VALID_MODES <hlsfactory.flow_balor.VALID_MODES>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.VALID_MODES
          :summary:
   * - :py:obj:`_DOT_DEFAULT_DECLARATIONS <hlsfactory.flow_balor._DOT_DEFAULT_DECLARATIONS>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor._DOT_DEFAULT_DECLARATIONS
          :summary:
   * - :py:obj:`_DOT_NODE_PATTERN <hlsfactory.flow_balor._DOT_NODE_PATTERN>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor._DOT_NODE_PATTERN
          :summary:
   * - :py:obj:`_DOT_EDGE_PATTERN <hlsfactory.flow_balor._DOT_EDGE_PATTERN>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor._DOT_EDGE_PATTERN
          :summary:
   * - :py:obj:`_DOT_ATTR_PATTERN <hlsfactory.flow_balor._DOT_ATTR_PATTERN>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor._DOT_ATTR_PATTERN
          :summary:
   * - :py:obj:`__all__ <hlsfactory.flow_balor.__all__>`
     - .. autodoc2-docstring:: hlsfactory.flow_balor.__all__
          :summary:

API
~~~

.. py:data:: HLSFACTORY_BALOR_PATH_ENV_VAR
   :canonical: hlsfactory.flow_balor.HLSFACTORY_BALOR_PATH_ENV_VAR
   :value: 'HLSFACTORY_BALOR_PATH'

   .. autodoc2-docstring:: hlsfactory.flow_balor.HLSFACTORY_BALOR_PATH_ENV_VAR

.. py:data:: HLSFACTORY_BALOR_DOCKER_IMAGE_ENV_VAR
   :canonical: hlsfactory.flow_balor.HLSFACTORY_BALOR_DOCKER_IMAGE_ENV_VAR
   :value: 'HLSFACTORY_BALOR_DOCKER_IMAGE'

   .. autodoc2-docstring:: hlsfactory.flow_balor.HLSFACTORY_BALOR_DOCKER_IMAGE_ENV_VAR

.. py:data:: DEFAULT_BALOR_DOCKER_IMAGE
   :canonical: hlsfactory.flow_balor.DEFAULT_BALOR_DOCKER_IMAGE
   :value: 'emmetmurphy/balor:latest'

   .. autodoc2-docstring:: hlsfactory.flow_balor.DEFAULT_BALOR_DOCKER_IMAGE

.. py:data:: GRAPH_COMPILER_RELATIVE_PATH
   :canonical: hlsfactory.flow_balor.GRAPH_COMPILER_RELATIVE_PATH
   :value: 'Path(...)'

   .. autodoc2-docstring:: hlsfactory.flow_balor.GRAPH_COMPILER_RELATIVE_PATH

.. py:data:: GRAPH_FILENAME
   :canonical: hlsfactory.flow_balor.GRAPH_FILENAME
   :value: 'data_graph_balor.dot'

   .. autodoc2-docstring:: hlsfactory.flow_balor.GRAPH_FILENAME

.. py:data:: GRAPH_SUMMARY_FILENAME
   :canonical: hlsfactory.flow_balor.GRAPH_SUMMARY_FILENAME
   :value: 'data_graph_balor.json'

   .. autodoc2-docstring:: hlsfactory.flow_balor.GRAPH_SUMMARY_FILENAME

.. py:data:: VALID_MODES
   :canonical: hlsfactory.flow_balor.VALID_MODES
   :value: 'frozenset(...)'

   .. autodoc2-docstring:: hlsfactory.flow_balor.VALID_MODES

.. py:data:: _DOT_DEFAULT_DECLARATIONS
   :canonical: hlsfactory.flow_balor._DOT_DEFAULT_DECLARATIONS
   :value: 'frozenset(...)'

   .. autodoc2-docstring:: hlsfactory.flow_balor._DOT_DEFAULT_DECLARATIONS

.. py:data:: _DOT_NODE_PATTERN
   :canonical: hlsfactory.flow_balor._DOT_NODE_PATTERN
   :value: 'compile(...)'

   .. autodoc2-docstring:: hlsfactory.flow_balor._DOT_NODE_PATTERN

.. py:data:: _DOT_EDGE_PATTERN
   :canonical: hlsfactory.flow_balor._DOT_EDGE_PATTERN
   :value: 'compile(...)'

   .. autodoc2-docstring:: hlsfactory.flow_balor._DOT_EDGE_PATTERN

.. py:data:: _DOT_ATTR_PATTERN
   :canonical: hlsfactory.flow_balor._DOT_ATTR_PATTERN
   :value: 'compile(...)'

   .. autodoc2-docstring:: hlsfactory.flow_balor._DOT_ATTR_PATTERN

.. py:function:: extract_dot_graph(output: str) -> str
   :canonical: hlsfactory.flow_balor.extract_dot_graph

   .. autodoc2-docstring:: hlsfactory.flow_balor.extract_dot_graph

.. py:function:: flatten_dot_quoted_newlines(text: str) -> str
   :canonical: hlsfactory.flow_balor.flatten_dot_quoted_newlines

   .. autodoc2-docstring:: hlsfactory.flow_balor.flatten_dot_quoted_newlines

.. py:function:: get_balor_install_dir(balor_install_dir: str | pathlib.Path | None = None) -> pathlib.Path | None
   :canonical: hlsfactory.flow_balor.get_balor_install_dir

   .. autodoc2-docstring:: hlsfactory.flow_balor.get_balor_install_dir

.. py:function:: find_graph_compiler(balor_install_dir: pathlib.Path) -> pathlib.Path
   :canonical: hlsfactory.flow_balor.find_graph_compiler

   .. autodoc2-docstring:: hlsfactory.flow_balor.find_graph_compiler

.. py:function:: validate_mode(mode: str) -> str
   :canonical: hlsfactory.flow_balor.validate_mode

   .. autodoc2-docstring:: hlsfactory.flow_balor.validate_mode

.. py:function:: _parse_dot_attributes(attrs: str | None) -> dict[str, str]
   :canonical: hlsfactory.flow_balor._parse_dot_attributes

   .. autodoc2-docstring:: hlsfactory.flow_balor._parse_dot_attributes

.. py:class:: BalorGraphData
   :canonical: hlsfactory.flow_balor.BalorGraphData

   .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphData

   .. py:attribute:: format
      :canonical: hlsfactory.flow_balor.BalorGraphData.format
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphData.format

   .. py:attribute:: top
      :canonical: hlsfactory.flow_balor.BalorGraphData.top
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphData.top

   .. py:attribute:: mode
      :canonical: hlsfactory.flow_balor.BalorGraphData.mode
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphData.mode

   .. py:attribute:: graph_file
      :canonical: hlsfactory.flow_balor.BalorGraphData.graph_file
      :type: str
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphData.graph_file

   .. py:attribute:: num_nodes
      :canonical: hlsfactory.flow_balor.BalorGraphData.num_nodes
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphData.num_nodes

   .. py:attribute:: num_edges
      :canonical: hlsfactory.flow_balor.BalorGraphData.num_edges
      :type: int
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphData.num_edges

   .. py:attribute:: node_attribute_counts
      :canonical: hlsfactory.flow_balor.BalorGraphData.node_attribute_counts
      :type: dict[str, int]
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphData.node_attribute_counts

   .. py:attribute:: edge_attribute_counts
      :canonical: hlsfactory.flow_balor.BalorGraphData.edge_attribute_counts
      :type: dict[str, int]
      :value: None

      .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphData.edge_attribute_counts

   .. py:method:: parse_from_dot_file(fp: pathlib.Path, top: str, mode: str) -> hlsfactory.flow_balor.BalorGraphData
      :canonical: hlsfactory.flow_balor.BalorGraphData.parse_from_dot_file
      :classmethod:

      .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphData.parse_from_dot_file

.. py:class:: BalorGraphFlow(balor_install_dir: str | pathlib.Path | None = None, docker_image: str | None = None, python_bin: str = 'python', log_output: bool = False)
   :canonical: hlsfactory.flow_balor.BalorGraphFlow

   Bases: :py:obj:`hlsfactory.framework.ToolFlow`

   .. py:attribute:: name
      :canonical: hlsfactory.flow_balor.BalorGraphFlow.name
      :value: 'BalorGraphFlow'

      .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphFlow.name

   .. py:method:: build_command(source_path: pathlib.Path, top: str, mode: str, output_dir: pathlib.Path, generalize_types: bool, make_pdf: bool) -> str
      :canonical: hlsfactory.flow_balor.BalorGraphFlow.build_command

      .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphFlow.build_command

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.flow_balor.BalorGraphFlow.execute

      .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphFlow.execute

   .. py:method:: _log(design_dir: pathlib.Path, status: str, start_time: float, message: str | None) -> None
      :canonical: hlsfactory.flow_balor.BalorGraphFlow._log

      .. autodoc2-docstring:: hlsfactory.flow_balor.BalorGraphFlow._log

.. py:data:: __all__
   :canonical: hlsfactory.flow_balor.__all__
   :value: ['DEFAULT_BALOR_DOCKER_IMAGE', 'HLSFACTORY_BALOR_DOCKER_IMAGE_ENV_VAR', 'HLSFACTORY_BALOR_PATH_ENV_V...

   .. autodoc2-docstring:: hlsfactory.flow_balor.__all__
