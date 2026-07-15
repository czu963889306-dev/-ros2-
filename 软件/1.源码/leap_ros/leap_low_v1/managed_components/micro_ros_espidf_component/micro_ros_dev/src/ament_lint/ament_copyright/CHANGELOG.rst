^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_copyright
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.12.15 (2026-02-13)
--------------------

0.12.14 (2025-07-18)
--------------------

0.12.13 (2025-07-16)
--------------------

0.12.12 (2025-03-25)
--------------------

0.12.11 (2024-05-15)
--------------------

0.12.10 (2024-01-24)
--------------------

0.12.9 (2023-11-13)
-------------------

0.12.8 (2023-09-19)
-------------------

0.12.7 (2023-07-17)
-------------------

0.12.6 (2023-04-25)
-------------------

0.12.5 (2023-01-12)
-------------------

0.12.4 (2022-05-09)
-------------------

0.12.3 (2022-04-08)
-------------------
* Fix importlib_metadata warning on Python 3.10. (`#365 <https://github.com/ament/ament_lint/issues/365>`_)
* Contributors: Chris Lalancette

0.12.2 (2022-03-28)
-------------------

0.12.1 (2022-03-01)
-------------------

0.12.0 (2022-02-18)
-------------------

0.11.4 (2022-01-14)
-------------------
* Update forthcoming version in changelogs
* Contributors: Audrow Nash

0.11.3 (2022-01-14)
-------------------
* Update maintainers to Michael Jeronimo and Michel Hidalgo (`#340 <https://github.com/ament/ament_lint/issues/340>`_)
* [ament_copyright] Fix file exclusion behavior (`#327 <https://github.com/ament/ament_lint/issues/327>`_)
  * [ament_copyright] Fix file exclusion behavior
  This commit fixes the faulty file exclusion behavior reported in
  https://github.com/ament/ament_lint/issues/326.
  Specifically, the exclusion list is matched against traversed
  files in the `crawler` module.
  Changes inspired by https://github.com/ament/ament_lint/pull/299/.
  * Update excluded file path in copyright tests
  Since file names are not indiscriminately matched throughout the
  search tree anymore, the excluded files listed in the copyright
  tests need to be updated relative to the root of the package.
  * Add test cases to check exclusion behavior
  Specifically, these tests check for:
  - Incorrect exclusion of single filenames.
  - Correct exclusion of relatively/absolutely addressed filenames.
  - Correct exclusion of wildcarded paths.
  * Add unit tests for crawler module
  These unit tests make sure both search and exclusion behaviors are
  correctly demonstrated by the `ament_copyright.crawler` module.
* Contributors: Abrar Rahman Protyasha, Audrow Nash

0.11.2 (2021-06-18)
-------------------

0.11.1 (2021-06-18)
-------------------

0.11.0 (2021-06-18)
-------------------
* Add SPDX identifiers to the licenses. (`#315 <https://github.com/ament/ament_lint/issues/315>`_)
* Contributors: Chris Lalancette

0.10.6 (2021-05-06)
-------------------

0.10.5 (2021-04-14)
-------------------
* Remove Claire as a maintainer. (`#312 <https://github.com/ament/ament_lint/issues/312>`_)
  * Remove Claire as a maintainer.
  * Remove dead email addresses.
  * Remove more dead email addresses.
  * Switch setup.py maintainer to Audrow.
* Contributors: Chris Lalancette

0.10.4 (2021-03-18)
-------------------
* Use non-blind except for open() (`#307 <https://github.com/ament/ament_lint/issues/307>`_)
* Add optional file header style (`#304 <https://github.com/ament/ament_lint/issues/304>`_)
  * Add optional file header style
  * Fix test on ament_copyright
* Contributors: Alfi Maulana, Christophe Bedard

0.10.3 (2021-03-03)
-------------------
* 0.10.3
* Contributors: Audrow Nash

0.10.2 (2021-02-12)
-------------------
* Add Audrow as a maintainer (`#294 <https://github.com/ament/ament_lint/issues/294>`_)
* Drop trailing tab from package manifests (`#291 <https://github.com/ament/ament_lint/issues/291>`_)
  Follow-up to 8bf194aa1ac282db5483dd0d3fefff8f325b0db8
* Contributors: Audrow Nash, Scott K Logan

0.10.1 (2021-01-25)
-------------------
* add mit-0 as a valid license to ament_copyright (`#284 <https://github.com/ament/ament_lint/issues/284>`_)
* Support Python 3.8-provided importlib.metadata (`#290 <https://github.com/ament/ament_lint/issues/290>`_)
  The importlib_metadata package is a backport of the importlib.metadata
  module from Python 3.8. Fedora (and possibly others) no longer package
  importlib_metadata because they ship Python versions which have the
  functionality built-in.
* Update maintainer (`#274 <https://github.com/ament/ament_lint/issues/274>`_)
  * update maintainer
  * add authors
* Contributors: Claire Wang, M. Mei, Scott K Logan

0.10.0 (2020-09-18)
-------------------
* added bsd 2 clause simplified license to ament_copyright (`#267 <https://github.com/ament/ament_lint/issues/267>`_)
  * added bsd 2 clause simplified license to ament_copyright
* Remove use of pkg_resources from ament_lint. (`#260 <https://github.com/ament/ament_lint/issues/260>`_)
  Replace it with the use of the more modern importlib_metadata
  library.  There are a couple of reasons to do this:
  1.  pkg_resources is quite slow to import; on my machine,
  just firing up the python interpreter takes ~35ms, while
  firing up the python interpreter and importing pkg_resources
  takes ~175ms.  Firing up the python interpreter and importing
  importlib_metadata takes ~70ms.  Removing 100ms per invocation
  of the command-line both makes it speedier for users, and
  will speed up our tests (which call out to the command-line
  quite a lot).
  2.  pkg_resources is somewhat deprecated and being replaced
  by importlib.  https://importlib-metadata.readthedocs.io/en/latest/using.html
  describes some of it
  Note: By itself, this change is not enough to completely remove our
  dependence on pkg_resources.  We'll also have to do something about
  the console_scripts that setup.py generates.  That will be
  a separate effort.
* Add pytest.ini so local tests don't display warning. (`#259 <https://github.com/ament/ament_lint/issues/259>`_)
* Contributors: Chris Lalancette, Evan Flynn

0.9.4 (2020-05-26)
------------------

0.9.3 (2020-05-18)
------------------
* Remove output on stderr for case that is not an error (`#248 <https://github.com/ament/ament_lint/issues/248>`_)
  * Remove output on stderr for case that is not an error
  * Remove early return to generate xml result file
* Contributors: Jorge Perez

0.9.2 (2020-05-08)
------------------
* Allow AMENT_IGNORE markers to be directories (`#232 <https://github.com/ament/ament_lint/issues/232>`_)
* Contributors: Dan Rose

0.9.1 (2020-04-10)
------------------
* 0.9.0
* more verbose test_flake8 error messages (same as `ros2/launch_ros#135 <https://github.com/ros2/launch_ros/issues/135>`_)
* Generate xunit files valid for the junit10.xsd (`#220 <https://github.com/ament/ament_lint/issues/220>`_)
* remove status attribute from result XML (`#212 <https://github.com/ament/ament_lint/issues/212>`_)
* Add test case for Apache 2 (`#208 <https://github.com/ament/ament_lint/issues/208>`_)
* Add support for Boost Software License in ament_copyright package (`#207 <https://github.com/ament/ament_lint/issues/207>`_)
  * Add support for the Boost Software License v1
  * Add tests for boost1 license
* Add support for the 3-Clause BSD license (`#205 <https://github.com/ament/ament_lint/issues/205>`_)
  * Add support for the 3-Clause BSD license
  * Add missing entry setup.py file for the added license
  * Add test cases for .cpp and .py files
  * Add a newline to increase readability
  * Fix missing addition needed to use 3-clause bsd tests
  * Remove line not usable
* Fix bug, allows using ament_copyright with bsd3 (`#206 <https://github.com/ament/ament_lint/issues/206>`_)
  * Fix bug, allows using ament_copyright with bsd3
  In the bs2 headers, exists a "{copyright_holder}" text that causes a problem
  when using the command ament_copyright to add headers to a source file.
  This fix adds a default value for that key, to match the original 3-Clause BSD
  text, and allowing to use the tool.
  * Add copyright_name if template includes copyright_holder reference
  * Revert comma addition
* Contributors: Chris Lalancette, Dirk Thomas, Jacob Perron, Jorge Perez, Jose Luis Rivero

0.8.1 (2019-10-23)
------------------
* install resource marker file for packages (`#193 <https://github.com/ament/ament_lint/issues/193>`_)
* Contributors: Dirk Thomas

0.8.0 (2019-09-17)
------------------
* install manifest files in Python packages (`#185 <https://github.com/ament/ament_lint/issues/185>`_)
* Convert list comprehension to generator (`#179 <https://github.com/ament/ament_lint/issues/179>`_)
  Addresses flake8 C412 errors introduced by flake8-comprehension 2.2.0
* Contributors: Dirk Thomas, Scott K Logan

0.7.4 (2019-07-31)
------------------
* declare pytest markers (`#164 <https://github.com/ament/ament_lint/issues/164>`_)
  * declare pytest markers
  * add markers to ament_xmllint tests
* Match copyright templates with differing whitespace (`#160 <https://github.com/ament/ament_lint/issues/160>`_)
  This change makes the template matching tolerant to more whitespace
  differences. In particular, it makes it tolerant in the presence of
  tabs, consecutive spaces (such as indentation) and EOL differences.
* Contributors: Dirk Thomas, Scott K Logan

0.7.3 (2019-05-09 14:08)
------------------------
* handle BOM properly (`#142 <https://github.com/ament/ament_lint/issues/142>`_)
* Contributors: Dirk Thomas

0.7.2 (2019-05-09 09:30)
------------------------

0.7.1 (2019-05-07)
------------------
* fix encoding of copyright result file (`#139 <https://github.com/ament/ament_lint/issues/139>`_)
* use explicit encoding when reading files (`#138 <https://github.com/ament/ament_lint/issues/138>`_)
* update phrase of status messages (`#137 <https://github.com/ament/ament_lint/issues/137>`_)
* Contributors: Dirk Thomas

0.7.0 (2019-04-11)
------------------
* Adding GPL (and LGPL) (`#126 <https://github.com/ament/ament_lint/issues/126>`_)
  Tested with:
  ros2 pkg create foobargpl --license GPLv3 --cpp-library-name foobargpl
  ament_copyright ./foobargpl/
  foobargpl/include/foobargpl/foobargpl.hpp: could not find copyright notice
  foobargpl/include/foobargpl/visibility_control.h: could not find copyright notice
  foobargpl/src/foobargpl.cpp: could not find copyright notice
  3 errors, checked 3 files
  Manually copied header to `foobargpl/include/foobargpl/foobargpl.hpp`.
  foobargpl/include/foobargpl/visibility_control.h: could not find copyright notice
  foobargpl/src/foobargpl.cpp: could not find copyright notice
  2 errors, checked 3 files
  ament_copyright ./foobargpl/ --add-missing "Copyright 2019, FooBar, Ltd." gplv3
  ament_copyright ./foobargpl/
  No errors, checked 3 files
* Contributors: Joshua Whitley

0.6.3 (2019-01-14)
------------------

0.6.2 (2018-12-06)
------------------

0.6.1 (2018-11-15)
------------------

0.6.0 (2018-11-14)
------------------
* Fix lint warnings from invalid escape sequences (`#111 <https://github.com/ament/ament_lint/issues/111>`_)
  Use raw strings for regex patterns to avoid warnings.
* Extend copyright checker to allow for doxygen-style copyright (`#108 <https://github.com/ament/ament_lint/issues/108>`_)
* Bsd clause3 fixup (`#106 <https://github.com/ament/ament_lint/issues/106>`_)
  * relax clause 3 matching by removing 'the' in front og the copyright holding entity
  * copyright holder doesn't have to be a company
* Adding MIT license templates. (`#105 <https://github.com/ament/ament_lint/issues/105>`_)
  * Adding MIT license templates.
  * Ommitting 'All Rights Reserved' not in actual license.
* Contributors: Jacob Perron, Joshua Whitley, Mikael Arguedas, jpsamper2009

0.5.2 (2018-06-27)
------------------

0.5.1 (2018-06-18 13:47)
------------------------
* level setup.py versions to 0.5.1
* Contributors: Mikael Arguedas

0.5.0 (2018-06-18 10:09)
------------------------
* add pytest markers to linter tests
* fix flake8 warning (`#99 <https://github.com/ament/ament_lint/issues/99>`_)
* Avoid use of builtin 'license' as variable name (`#97 <https://github.com/ament/ament_lint/issues/97>`_)
* set zip_safe to avoid warning during installation (`#96 <https://github.com/ament/ament_lint/issues/96>`_)
* Contributors: Dirk Thomas, dhood

0.4.0 (2017-12-08)
------------------
* remove test_suite, add pytest as test_requires
* 0.0.3
* Merge pull request `#84 <https://github.com/ament/ament_lint/issues/84>`_ from ament/flake8_plugins
  update style to satisfy new flake8 plugins
* update style to satisfy new flake8 plugins
* implicitly inherit from object (only in files not copied from somewhe… (`#83 <https://github.com/ament/ament_lint/issues/83>`_)
  * implicitly inherit from object (only in files not copied from somewhere else)
  * don't modify file copied from elsewhere
* Merge pull request `#81 <https://github.com/ament/ament_lint/issues/81>`_ from ament/ignore_build_spaces
  ignore folders with an AMENT_IGNORE file, e.g. build spaces
* ignore folders with an AMENT_IGNORE file, e.g. build spaces
* 0.0.2
* use OSI website as reference for license (`#80 <https://github.com/ament/ament_lint/issues/80>`_)
* Merge pull request `#78 <https://github.com/ament/ament_lint/issues/78>`_ from ament/use_flake8
  use flake8 instead of pep8 and pyflakes
* use flake8 instead of pep8 and pyflakes
* Add in support for the BSD2 license.
  This allows ament_copyright to properly support the BSD2
  license when doing copyright checking.
* Change the copyright regex to allow a (c) after the "Copyright" word.
  This is what is recommended by the BSD license.
* Change the copyright regex to allow a comma after the year(s).
* remove __future_\_ imports
* Merge pull request `#66 <https://github.com/ament/ament_lint/issues/66>`_ from ament/multiple_copyrights
  support multiple copyright lines
* support multiple copyright lines
* update schema url
* add schema to manifest files
* Merge pull request `#42 <https://github.com/ament/ament_lint/issues/42>`_ from ament/remove_second_extension
  remove result type extension from testsuite name
* remove result type extension from testsuite name
* Merge pull request `#28 <https://github.com/ament/ament_lint/issues/28>`_ from ament/pep257
  add packages to check pep257 compliance
* use ament_pep257
* remove debug output
* apply normpath to prevent './' prefix (fix `#24 <https://github.com/ament/ament_lint/issues/24>`_)
* Merge pull request `#19 <https://github.com/ament/ament_lint/issues/19>`_ from ament/split_linter_packages_in_python_and_cmake
  split linter packages in python and cmake
* make use of python linter packages
* support excluding filenames from copyright, pep8, pyflakes check
* fix variable name
* Merge pull request `#15 <https://github.com/ament/ament_lint/issues/15>`_ from ament/ament_copyright_reloaded
  add support for licenses
* update doc
* move apache2 snippets into separate files
* add support for licenses
* add trailing newline to generated test result files
* Merge pull request `#9 <https://github.com/ament/ament_lint/issues/9>`_ from ament/docs
  add docs for linters
* add docs for linters
* Merge pull request `#8 <https://github.com/ament/ament_lint/issues/8>`_ from ament/ament_copyright
  add more options to ament_copyright
* also handle \r\n newline
* remove python3 dependencies
* update url
* update package description
* add more options to ament_copyright
* modify generated unit test files for a better hierarchy in Jenkins
* make testname argument optional for all linters
* use other linters for the linter packages where possible
* Merge pull request `#3 <https://github.com/ament/ament_lint/issues/3>`_ from ament/copyright_headers
  adding copyright headers, which are failing this module
* adding copyright headers, which are failing this module
* run ament_copyright and ament_pyflakes with Python 3
* Merge pull request `#2 <https://github.com/ament/ament_lint/issues/2>`_ from ament/ament_lint_auto
  allow linting based on test dependencies only
* add ament_lint_auto and ament_lint_common, update all linter packages to implement extension point of ament_lint_auto
* use project(.. NONE)
* update to latest refactoring of ament_cmake
* add dependency on ament_cmake_environment
* add ament_copyright
* Contributors: Chris Lalancette, Dirk Thomas, Mikael Arguedas, Tully Foote
