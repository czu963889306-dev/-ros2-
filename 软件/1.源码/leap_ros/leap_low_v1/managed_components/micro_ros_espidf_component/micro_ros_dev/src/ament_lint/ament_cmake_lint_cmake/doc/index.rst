ament_lint_cmake
================

Checks the code style of CMake files using `CMakeLint
<https://github.com/richq/cmake-lint>`_.
Files with the following names / extensions are being considered:
``CMakeLists.txt``, ``.cmake``.


How to run the check from the command line?
-------------------------------------------

The command line tool is provided by the package `ament_lint_cmake
<https://github.com/ament/ament_lint>`_.


How to run the check from within a CMake ament package as part of the tests?
----------------------------------------------------------------------------

``package.xml``:

.. code:: xml

    <buildtool_depend>ament_cmake</buildtool_depend>
    <test_depend>ament_cmake_lint_cmake</test_depend>

``CMakeLists.txt``:

.. code:: cmake

    find_package(ament_cmake REQUIRED)
    if(BUILD_TESTING)
      find_package(ament_cmake_lint_cmake REQUIRED)
      ament_lint_cmake()
    endif()

When running multiple linters as part of the CMake tests the documentation of
the package `ament_lint_auto <https://github.com/ament/ament_lint>`_ might
contain some useful information.

The documentation of the package `ament_cmake_test
<https://github.com/ament/ament_cmake>`_ provides more information on testing
in CMake ament packages.
