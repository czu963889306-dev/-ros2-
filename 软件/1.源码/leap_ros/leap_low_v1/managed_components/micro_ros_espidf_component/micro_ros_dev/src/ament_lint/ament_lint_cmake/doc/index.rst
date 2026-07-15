ament_lint_cmake
================

Checks the code style of CMake files using `CMakeLint
<https://github.com/richq/cmake-lint>`_.
Files with the following names / extensions are being considered:
``CMakeLists.txt``, ``.cmake``.


How to run the check from the command line?
-------------------------------------------

.. code:: sh

    ament_lint_cmake [<path> ...]


How to run the check from within a CMake ament package as part of the tests?
----------------------------------------------------------------------------

The CMake integration is provided by the package `ament_cmake_lint_cmake
<https://github.com/ament/ament_lint>`_.
