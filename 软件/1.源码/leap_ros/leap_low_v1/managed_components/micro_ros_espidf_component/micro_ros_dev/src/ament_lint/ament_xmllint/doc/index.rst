ament_xmllint
=============

Checks XML markup files using `xmllint <http://xmlsoft.org/xmllint.html>`_.
Files with the following extensions are being considered: ``.xml``.


How to run the check from the command line?
-------------------------------------------

.. code:: sh

    ament_xmllint [<path> ...]


How to run the check from within a CMake ament package as part of the tests?
----------------------------------------------------------------------------

The CMake integration is provided by the package `ament_cmake_xmllint
<https://github.com/ament/ament_lint>`_.
