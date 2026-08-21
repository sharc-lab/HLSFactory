:py:mod:`hlsfactory.jinja_frontend`
===================================

.. py:module:: hlsfactory.jinja_frontend

.. autodoc2-docstring:: hlsfactory.jinja_frontend
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`JinjaFrontend <hlsfactory.jinja_frontend.JinjaFrontend>`
     -

API
~~~

.. py:class:: JinjaFrontend(work_dir: pathlib.Path, jinja_configs: list[dict[str, typing.Any]], log_execution_time: bool = True)
   :canonical: hlsfactory.jinja_frontend.JinjaFrontend

   Bases: :py:obj:`hlsfactory.framework.Frontend`

   .. py:attribute:: name
      :canonical: hlsfactory.jinja_frontend.JinjaFrontend.name
      :value: 'JinjaFrontend'

      .. autodoc2-docstring:: hlsfactory.jinja_frontend.JinjaFrontend.name

   .. py:method:: load_configs_from_jsonl(fp_jsonl: pathlib.Path)
      :canonical: hlsfactory.jinja_frontend.JinjaFrontend.load_configs_from_jsonl

      .. autodoc2-docstring:: hlsfactory.jinja_frontend.JinjaFrontend.load_configs_from_jsonl

   .. py:method:: load_configs_from_json(fp_json: pathlib.Path)
      :canonical: hlsfactory.jinja_frontend.JinjaFrontend.load_configs_from_json

      .. autodoc2-docstring:: hlsfactory.jinja_frontend.JinjaFrontend.load_configs_from_json

   .. py:method:: execute(design: hlsfactory.framework.Design, _timeout: float | None = None) -> list[hlsfactory.framework.Design]
      :canonical: hlsfactory.jinja_frontend.JinjaFrontend.execute

      .. autodoc2-docstring:: hlsfactory.jinja_frontend.JinjaFrontend.execute
