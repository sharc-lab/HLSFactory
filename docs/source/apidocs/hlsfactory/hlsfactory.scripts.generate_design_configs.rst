:py:mod:`hlsfactory.scripts.generate_design_configs`
====================================================

.. py:module:: hlsfactory.scripts.generate_design_configs

.. autodoc2-docstring:: hlsfactory.scripts.generate_design_configs
   :allowtitles:

Module Contents
---------------

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`detect_tcl_files <hlsfactory.scripts.generate_design_configs.detect_tcl_files>`
     - .. autodoc2-docstring:: hlsfactory.scripts.generate_design_configs.detect_tcl_files
          :summary:
   * - :py:obj:`generate_flow_configs <hlsfactory.scripts.generate_design_configs.generate_flow_configs>`
     - .. autodoc2-docstring:: hlsfactory.scripts.generate_design_configs.generate_flow_configs
          :summary:
   * - :py:obj:`process_design_directory <hlsfactory.scripts.generate_design_configs.process_design_directory>`
     - .. autodoc2-docstring:: hlsfactory.scripts.generate_design_configs.process_design_directory
          :summary:
   * - :py:obj:`main <hlsfactory.scripts.generate_design_configs.main>`
     - .. autodoc2-docstring:: hlsfactory.scripts.generate_design_configs.main
          :summary:

Data
~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`DATASET_NAME_MAP <hlsfactory.scripts.generate_design_configs.DATASET_NAME_MAP>`
     - .. autodoc2-docstring:: hlsfactory.scripts.generate_design_configs.DATASET_NAME_MAP
          :summary:
   * - :py:obj:`TCL_PATTERNS <hlsfactory.scripts.generate_design_configs.TCL_PATTERNS>`
     - .. autodoc2-docstring:: hlsfactory.scripts.generate_design_configs.TCL_PATTERNS
          :summary:

API
~~~

.. py:data:: DATASET_NAME_MAP
   :canonical: hlsfactory.scripts.generate_design_configs.DATASET_NAME_MAP
   :type: dict[str, str]
   :value: None

   .. autodoc2-docstring:: hlsfactory.scripts.generate_design_configs.DATASET_NAME_MAP

.. py:data:: TCL_PATTERNS
   :canonical: hlsfactory.scripts.generate_design_configs.TCL_PATTERNS
   :type: dict[str, list[str]]
   :value: None

   .. autodoc2-docstring:: hlsfactory.scripts.generate_design_configs.TCL_PATTERNS

.. py:function:: detect_tcl_files(design_dir: pathlib.Path) -> dict[str, str]
   :canonical: hlsfactory.scripts.generate_design_configs.detect_tcl_files

   .. autodoc2-docstring:: hlsfactory.scripts.generate_design_configs.detect_tcl_files

.. py:function:: generate_flow_configs(tcl_files: dict[str, str]) -> list[hlsfactory.design_config.FlowConfig]
   :canonical: hlsfactory.scripts.generate_design_configs.generate_flow_configs

   .. autodoc2-docstring:: hlsfactory.scripts.generate_design_configs.generate_flow_configs

.. py:function:: process_design_directory(design_dir: pathlib.Path, dataset_name: str, dry_run: bool = False) -> tuple[bool, str]
   :canonical: hlsfactory.scripts.generate_design_configs.process_design_directory

   .. autodoc2-docstring:: hlsfactory.scripts.generate_design_configs.process_design_directory

.. py:function:: main() -> int
   :canonical: hlsfactory.scripts.generate_design_configs.main

   .. autodoc2-docstring:: hlsfactory.scripts.generate_design_configs.main
