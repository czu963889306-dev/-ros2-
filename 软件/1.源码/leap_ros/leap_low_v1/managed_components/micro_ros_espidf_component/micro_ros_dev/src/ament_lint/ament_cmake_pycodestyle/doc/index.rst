ament_cmake_pycodestyle
=======================

Checks the code of Python source files against some of the style
conventions in PEP 8 using `pycodestyle
<http://pycodestyle.readthedocs.org/>`_.
Files with the following extensions are being considered: ``.py``.


How to run the check from the command line?
-------------------------------------------

The command line tool is provided by the package `ament_pycodestyle
<https://github.com/ament/ament_lint>`_.


How to run the check from within a CMake ament package as part of the tests?
----------------------------------------------------------------------------

``package.xml``:

.. code:: xml

    <buildtool_depend>ament_cmake</buildtool_depend>
    <test_depend>ament_cmake_pycodestyle</test_depend>

``CMakeLists.txt``:

.. code:: cmake

    find_package(ament_cmake REQUIRED)
    if(BUILD_TESTING)
      find_package(ament_cmake_pycodestyle REQUIRED)
      ament_pycodestyle()
    endif()

When running multiple linters as part of the CMake tests the documentation of
the package `ament_lint_auto <https://github.com/ament/ament_lint>`_ might
contain some useful information.

The documentation of the package `ament_cmake_test
<https://github.com/ament/ament_cmake>`_ provides more information on testing
in CMake ament packages.
