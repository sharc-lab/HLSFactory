:py:mod:`hlsfactory.autodse_vitis.flow_autodse_vitis`
=====================================================

.. py:module:: hlsfactory.autodse_vitis.flow_autodse_vitis

.. autodoc2-docstring:: hlsfactory.autodse_vitis.flow_autodse_vitis
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`AutoDSEFlow <hlsfactory.autodse_vitis.flow_autodse_vitis.AutoDSEFlow>`
     -

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`_extract_top_module_from_tcl <hlsfactory.autodse_vitis.flow_autodse_vitis._extract_top_module_from_tcl>`
     - .. autodoc2-docstring:: hlsfactory.autodse_vitis.flow_autodse_vitis._extract_top_module_from_tcl
          :summary:

API
~~~

.. py:function:: _extract_top_module_from_tcl(template_dir: pathlib.Path) -> str | None
   :canonical: hlsfactory.autodse_vitis.flow_autodse_vitis._extract_top_module_from_tcl

   .. autodoc2-docstring:: hlsfactory.autodse_vitis.flow_autodse_vitis._extract_top_module_from_tcl

.. py:class:: AutoDSEFlow(work_dir: pathlib.Path, *, output_folder_name: str = 'autodse', max_iterations: int = 3, timeout_min: int = 10)
   :canonical: hlsfactory.autodse_vitis.flow_autodse_vitis.AutoDSEFlow

   Bases: :py:obj:`hlsfactory.framework.Flow`

   .. py:attribute:: name
      :canonical: hlsfactory.autodse_vitis.flow_autodse_vitis.AutoDSEFlow.name
      :type: str
      :value: 'AutoDSEFlow'

      .. autodoc2-docstring:: hlsfactory.autodse_vitis.flow_autodse_vitis.AutoDSEFlow.name

   .. py:method:: execute(design: hlsfactory.framework.Design, timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.autodse_vitis.flow_autodse_vitis.AutoDSEFlow.execute

      .. autodoc2-docstring:: hlsfactory.autodse_vitis.flow_autodse_vitis.AutoDSEFlow.execute
