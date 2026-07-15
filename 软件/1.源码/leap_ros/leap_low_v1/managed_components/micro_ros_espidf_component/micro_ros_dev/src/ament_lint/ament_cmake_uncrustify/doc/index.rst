ament_uncrustify
================

Checks the code style of C / C++ source files using `Uncrustify
<http://uncrustify.sourceforge.net/>`_.
Files with the following extensions are being considered:
``.c``, ``.cc``, ``.cpp``, ``.cxx``, ``.h``, ``.hh``, ``.hpp``, ``.hxx``.


How to run the check from the command line?
-------------------------------------------

The command line tool is provided by the package `ament_uncrustify
<https://github.com/ament/ament_lint/tree/master/ament_uncrustify>`_.

Note that ament_uncrustify comes with a command line tool which can automatically reformat the code according to the style guide by calling

.. code:: sh

  ament_uncrustify --reformat <path_to_source_folders>

How to run the check from within a CMake ament package as part of the tests?
----------------------------------------------------------------------------

``package.xml``:

.. code:: xml

    <buildtool_depend>ament_cmake</buildtool_depend>
    <test_depend>ament_cmake_uncrustify</test_depend>

``CMakeLists.txt``:

.. code:: cmake

    find_package(ament_cmake REQUIRED)
    if(BUILD_TESTING)
      find_package(ament_cmake_uncrustify REQUIRED)
      ament_uncrustify()
    endif()

When running multiple linters as part of the CMake tests the documentation of
the package `ament_lint_auto <https://github.com/ament/ament_lint>`_ might
contain some useful information.

The documentation of the package `ament_cmake_test
<https://github.com/ament/ament_cmake>`_ provides more information on testing
in CMake ament packages.
