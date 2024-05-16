:py:mod:`hlsfactory.gather_data`
================================

.. py:module:: hlsfactory.gather_data

.. autodoc2-docstring:: hlsfactory.gather_data
   :allowtitles:

Module Contents
---------------

Classes
~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`FileFormat <hlsfactory.gather_data.FileFormat>`
     -

Functions
~~~~~~~~~

.. list-table::
   :class: autosummary longtable
   :align: left

   * - :py:obj:`main <hlsfactory.gather_data.main>`
     - .. autodoc2-docstring:: hlsfactory.gather_data.main
          :summary:

API
~~~

.. py:function:: main(args: argparse.Namespace) -> None
   :canonical: hlsfactory.gather_data.main

   .. autodoc2-docstring:: hlsfactory.gather_data.main

.. py:class:: FileFormat
   :canonical: hlsfactory.gather_data.FileFormat

   Bases: :py:obj:`enum.Enum`

   .. py:attribute:: CSV
      :canonical: hlsfactory.gather_data.FileFormat.CSV
      :value: 'csv'

      .. autodoc2-docstring:: hlsfactory.gather_data.FileFormat.CSV

   .. py:attribute:: JSON
      :canonical: hlsfactory.gather_data.FileFormat.JSON
      :value: 'json'

      .. autodoc2-docstring:: hlsfactory.gather_data.FileFormat.JSON

   .. py:attribute:: SQLITE
      :canonical: hlsfactory.gather_data.FileFormat.SQLITE
      :value: 'sqlite'

      .. autodoc2-docstring:: hlsfactory.gather_data.FileFormat.SQLITE
