ament_cpplint
==========

Checks the code style of C / C++ source files using `cpplint
<https://github.com/cpplint/cpplint>`_.
Files with the following extensions are being considered:
``.c``, ``.cc``, ``.cpp``, ``.cxx``, ``.h``, ``.hh``, ``.hpp``, ``.hxx``.


How to run the check from the command line?
-------------------------------------------

.. code:: sh

    ament_cpplint [<path> ...]


How to run the check from within a CMake ament package as part of the tests?
----------------------------------------------------------------------------

The CMake integration is provided by the package `ament_cmake_cpplint
<https://github.com/ament/ament_lint>`_.
