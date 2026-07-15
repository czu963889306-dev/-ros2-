ament_cppcheck
==============

Performs a static code analysis of C / C++ source files using `CppCheck
<http://cppcheck.sourceforge.net/>`_.
Files with the following extensions are being considered:
``.c``, ``.cc``, ``.cpp``, ``.cxx``, ``.h``, ``.hh``, ``.hpp``, ``.hxx``.


How to run the check from the command line?
-------------------------------------------

.. code:: sh

    ament_cppcheck [<path> ...]


How to run the check from within a CMake ament package as part of the tests?
----------------------------------------------------------------------------

The CMake integration is provided by the package `ament_cmake_cppcheck
<https://github.com/ament/ament_lint>`_.
