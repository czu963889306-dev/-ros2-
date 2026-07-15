ament_mypy
============

Performs a static type analysis of Python source files using `mypy
<https://mypy.readthedocs.io/>`_.
Files with the following extensions are being considered: ``.py``.


How to run the check from the command line?
-------------------------------------------

.. code:: sh

    ament_mypy [<path> ...]


How to run the check from within a CMake ament package as part of the tests?
----------------------------------------------------------------------------

The CMake integration is provided by the package `ament_cmake_mypy
<https://github.com/ament/ament_lint>`_.
