ament_pclint
==============

Performs a static code analysis of C / C++ source files using `PC-lint
<http://www.gimpel.com/html/index.htm>`_.

How to run the check from the command line?
-------------------------------------------

The command line tool is provided by the package `ament_pclint`_.


How to run the check from within a CMake ament package as part of the tests?
----------------------------------------------------------------------------

``package.xml``:

.. code:: xml

    <buildtool_depend>ament_cmake</buildtool_depend>
    <test_depend>ament_cmake_pclint</test_depend>

``CMakeLists.txt``:

.. code:: cmake

    find_package(ament_cmake REQUIRED)
    if(BUILD_TESTING)
      find_package(ament_cmake_pclint REQUIRED)
      ament_pclint()
    endif()

When running multiple linters as part of the CMake tests the documentation of
the package `ament_lint_auto <https://github.com/ament/ament_lint>`_ might
contain some useful information.

The documentation of the package `ament_cmake_test
<https://github.com/ament/ament_cmake>`_ provides more information on testing
in CMake ament packages.
