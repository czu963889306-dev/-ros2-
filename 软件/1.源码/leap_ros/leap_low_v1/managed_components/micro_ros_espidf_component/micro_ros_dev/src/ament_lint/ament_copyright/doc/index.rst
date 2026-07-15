ament_copyright
===============

Checks C / C++ / CMake / Python source files for the existence of a copyright
and license notice.
Files with the following extensions are being considered:
``.c``, ``.cc``, ``.cpp``, ``.cxx``, ``.h``, ``.hh``, ``.hpp``, ``.hxx``,
``.cmake``, ``.py``.

When being searched for recursively the following directories and files are
being excluded:

- directories starting with ``.`` (e.g. ``.git``) or ``_`` (e.g.
  ``__pycache__``)
- ``setup.py`` when being a sibling of ``package.xml``

Additionally it checks if the root of a repository contains a ``LICENSE`` and
``CONTRIBUTING.md`` file.


How to run the check from the command line?
-------------------------------------------

.. code:: sh

    ament_copyright [<path> ...]

When using the option ``--list-copyright-names`` a list of known copyright
holders is shown. The option ``--list-licenses`` outputs a list of known
licenses.

When using the option
``--add-missing <KNOWN_COPYRIGHT_NAME | "Copyright holder string"> <KNOWN_LICENSE>``
a copyright notice and license is added to all files which lack one.
The first argument can either be a name from the list returned by
``--list-copyright-names`` or a custom string. The second argument must be a
name from the list returned by ``--list-licenses``.

When using the option ``--add-copyright-year`` existing copyright notices are
being updated to include the current year.


How to run the check from within a CMake ament package as part of the tests?
----------------------------------------------------------------------------

The CMake integration is provided by the package `ament_cmake_copyright
<https://github.com/ament/ament_lint>`_.


How to add support for more licenses or copyright holders?
----------------------------------------------------------

The package uses Python entry points to get all list of known licenses and
copyright holder.
You can implement a custom package and contribute more implementations to these
entry points or extend this package with more licenses.


Why are my existing copyright / license notices not detected?
-------------------------------------------------------------

This script currently only checks line comments (lines starting with ``#`` /
``//`` depending on the language). Block comments / C-style comment (starting
with ``/*``) are not being detected to keep the complexity minimal.
Also the content must match the templates exactly.
