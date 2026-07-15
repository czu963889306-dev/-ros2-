ament_lint_auto
===============

The package simplifies using multiple linters as part of the CMake tests.
It reduces the amount of CMake code to a bare minimum.

``CMakeLists.txt``:

.. code:: cmake

    # this must happen before the invocation of ament_package()
    if(BUILD_TESTING)
      find_package(ament_lint_auto REQUIRED)
      ament_lint_auto_find_test_dependencies()
    endif()

The set of linters to be used is then only specified in the package manifest as
test dependencies.

``package.xml``:

.. code:: xml

    <test_depend>ament_lint_auto</test_depend>

    <!-- add test dependencies on any linter, e.g. -->
    <test_depend>ament_cmake_clang_format</test_depend>
    <test_depend>ament_cmake_cppcheck</test_depend>
    <test_depend>ament_cmake_pycodestyle</test_depend>

Since recursive dependencies are also being used a single test dependency is
sufficient to test with a set of common linters.

``package.xml``:

.. code:: xml

    <test_depend>ament_lint_auto</test_depend>

    <!-- this recursively depends on a set of common linters -->
    <test_depend>ament_lint_common</test_depend>

The documentation of the package `ament_cmake_test
<https://github.com/ament/ament_cmake>`_ provides more information on testing
in CMake ament packages.


How to exclude linter modules with ament_lint_auto?
---------------------------------------------------

Linter modules can be excluded via the CMake list variable `AMENT_LINT_AUTO_EXCLUDE`.

As an example to exclude the `copyright` linter:

``CMakeLists.txt``:

.. code:: cmake

    # this must happen before the invocation of ament_package()
    if(BUILD_TESTING)
      find_package(ament_lint_auto REQUIRED)
      list(APPEND AMENT_LINT_AUTO_EXCLUDE ament_cmake_copyright)
      ament_lint_auto_find_test_dependencies()
    endif()


How to exclude files with ament_lint_auto?
------------------------------------------

Linter hooks conform to the ament_lint_auto convention of excluding files
specified in the CMake list variable `AMENT_LINT_AUTO_FILE_EXCLUDE`.
As such, the CMake snippet from above can be modified to exclude files across
all linters with one addition.

``CMakeLists.txt``:

.. code:: cmake

    # this must happen before the invocation of ament_package()
    if(BUILD_TESTING)
      find_package(ament_lint_auto REQUIRED)
      set(AMENT_LINT_AUTO_FILE_EXCLUDE /path/to/ignored_file ...)
      ament_lint_auto_find_test_dependencies()
    endif()
