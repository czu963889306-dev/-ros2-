ament_clang_format
==================

Checks the code style of C / C++ source files using `ClangFormat
<http://clang.llvm.org/docs/ClangFormat.html>`_.
Files with the following extensions are being considered:
``.c``, ``.cc``, ``.cpp``, ``.cxx``, ``.h``, ``.hh``, ``.hpp``, ``.hxx``.


How to run the check from the command line?
-------------------------------------------

.. code:: sh

    ament_clang_format [-h] [--config path]
                       [--clang-format-version CLANG_FORMAT_VERSION]
                       [--reformat] [--xunit-file XUNIT_FILE]
                       [paths [paths ...]]

``paths`` are directories to recursively search for files to run clang-format
on.  If no ``paths`` option is set the current directory will be used.

The ``--config`` allows you to set the path to the .clang-format file to use.

The ``--clang-format-version`` enables you to set a different version of
clang-format to use.

When using the option ``--reformat`` the proposed changes are applied in place.

The ``--xunit-file`` option is used to generate a xunit output file.


How to run the check from within a CMake ament package as part of the tests?
----------------------------------------------------------------------------

The CMake integration is provided by the package `ament_cmake_clang_format
<https://github.com/ament/ament_lint>`_.
