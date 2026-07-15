ament_pclint
==============

Performs a static code analysis of C / C++ source files using `PC-lint
<http://www.gimpel.com/html/index.htm>`_.


How to run the check from the command line?
-------------------------------------------

.. code:: sh

    ament_pclint [<path> ...]


How to run the check from within a CMake ament package as part of the tests?
----------------------------------------------------------------------------

The CMake integration is provided by the package `ament_cmake_pclint
<https://github.com/ament_lint/>`_.
