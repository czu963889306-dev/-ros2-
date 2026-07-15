ament_flake8
============

Checks the code syntax and style of Python source files using `flake8
<http://flake8.readthedocs.org/>`_.
Files with the following extensions are being considered: ``.py``.


How to run the check from the command line?
-------------------------------------------

.. code:: sh

    ament_flake8 [<path> ...]


How to run the check from within a CMake ament package as part of the tests?
----------------------------------------------------------------------------

The CMake integration is provided by the package `ament_cmake_flake8
<https://github.com/ament/ament_lint>`_.
