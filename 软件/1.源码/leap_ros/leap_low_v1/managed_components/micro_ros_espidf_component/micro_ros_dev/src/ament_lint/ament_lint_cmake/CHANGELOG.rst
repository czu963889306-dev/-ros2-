^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_lint_cmake
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* ament_lint_cmake: default linelength in argumentparser for consistency (`#306 <https://github.com/ament/ament_lint/issues/306>`_)
* Contributors: Emerson Knapp

0.10.3 (2021-03-03)
-------------------
* 0.10.3
* Contributors: Audrow Nash

0.10.2 (2021-02-12)
-------------------
* Fix ament_lint_cmake line length expression (`#236 <https://github.com/ament/ament_lint/issues/236>`_)
  This regular expression is using the re.VERBOSE flag, meaning that
  characters after an un-escaped '#' character are interpreted as a
  comment and are not part of the expression.
  Also set the default maximum line length to 140 columns.
* Add Audrow as a maintainer (`#294 <https://github.com/ament/ament_lint/issues/294>`_)
* Make CMake linter line length configurable (`#235 <https://github.com/ament/ament_lint/issues/235>`_)
  Co-authored-by: Miaofei <miaofei@amazon.com>
* Drop trailing tab from package manifests (`#291 <https://github.com/ament/ament_lint/issues/291>`_)
  Follow-up to 8bf194aa1ac282db5483dd0d3fefff8f325b0db8
* Contributors: Audrow Nash, Scott K Logan

0.10.1 (2021-01-25)
-------------------
* Update maintainer (`#274 <https://github.com/ament/ament_lint/issues/274>`_)
  * update maintainer
  * add authors
* Contributors: Claire Wang

0.10.0 (2020-09-18)
-------------------
* Add pytest.ini so local tests don't display warning. (`#259 <https://github.com/ament/ament_lint/issues/259>`_)
* Contributors: Chris Lalancette

0.9.4 (2020-05-26)
------------------

0.9.3 (2020-05-18)
------------------
* Close source files after reading them (`#249 <https://github.com/ament/ament_lint/issues/249>`_)
  Resolves the ResourceWarning messages coming to the console during
  testing with debug-enabled Python.
* Contributors: Scott K Logan

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
* Contributors: Chris Lalancette, Dirk Thomas, Jose Luis Rivero

0.8.1 (2019-10-23)
------------------
* install resource marker file for packages (`#193 <https://github.com/ament/ament_lint/issues/193>`_)
* Contributors: Dirk Thomas

0.8.0 (2019-09-17)
------------------
* install manifest files in Python packages (`#185 <https://github.com/ament/ament_lint/issues/185>`_)
* Contributors: Dirk Thomas

0.7.4 (2019-07-31)
------------------
* Raw strings in cmakelint.py.
  This fixes DeprecationWarning on Windows.
* Contributors: Chris Lalancette

0.7.3 (2019-05-09 14:08)
------------------------

0.7.2 (2019-05-09 09:30)
------------------------

0.7.1 (2019-05-07)
------------------
* update phrase of status messages (`#137 <https://github.com/ament/ament_lint/issues/137>`_)
* Contributors: Dirk Thomas

0.7.0 (2019-04-11)
------------------

0.6.3 (2019-01-14)
------------------

0.6.2 (2018-12-06)
------------------

0.6.1 (2018-11-15)
------------------

0.6.0 (2018-11-14)
------------------

0.5.2 (2018-06-27)
------------------

0.5.1 (2018-06-18 13:47)
------------------------
* level setup.py versions to 0.5.1
* Contributors: Mikael Arguedas

0.5.0 (2018-06-18 10:09)
------------------------
* add pytest markers to linter tests
* set zip_safe to avoid warning during installation (`#96 <https://github.com/ament/ament_lint/issues/96>`_)
* Contributors: Dirk Thomas

0.4.0 (2017-12-08)
------------------
* remove test_suite, add pytest as test_requires
* 0.0.3
* Merge pull request `#84 <https://github.com/ament/ament_lint/issues/84>`_ from ament/flake8_plugins
  update style to satisfy new flake8 plugins
* update style to satisfy new flake8 plugins
* Merge pull request `#81 <https://github.com/ament/ament_lint/issues/81>`_ from ament/ignore_build_spaces
  ignore folders with an AMENT_IGNORE file, e.g. build spaces
* ignore folders with an AMENT_IGNORE file, e.g. build spaces
* 0.0.2
* Merge pull request `#78 <https://github.com/ament/ament_lint/issues/78>`_ from ament/use_flake8
  use flake8 instead of pep8 and pyflakes
* fix style warnings
* Test python module import order using flake8 (`#63 <https://github.com/ament/ament_lint/issues/63>`_)
  * Add flake8 linter
  * Don't deal with flake8-import-order just yet
  * Debugging prints
  * Reinstate import order rule
  * Fix reporting bug by using the inner flake8 style guide
  * Fixup
  * Add comment on wrapper StyleGuide use
  * use flake8 v3 (`#1 <https://github.com/ament/ament_lint/issues/1>`_)
  * Reorder package.xml
  * Get the filenames from the file checkers because input_file isn't called by flake8 anymore
  * Output count of all error types
  * Get flake8 to use the config file
  The current implementation of get_style_guide does not process the config file correctly.
  * Error when flake8 v2 found
  * Print errors like pep8
  * remove __future_\_ imports
  * add schema to manifest files
  * Support flake8 v2 as well as v3
  * Output checked files
  otherwise it's not present in xunit files for tests run directly with nose (not ament_cmake_flake8)
  * Prevent v2 imports from happening on systems with v3
  * Flake8 replaces pep8+pyflakes
* remove __future_\_ imports
* update schema url
* add schema to manifest files
* Merge pull request `#42 <https://github.com/ament/ament_lint/issues/42>`_ from ament/remove_second_extension
  remove result type extension from testsuite name
* remove result type extension from testsuite name
* Merge pull request `#28 <https://github.com/ament/ament_lint/issues/28>`_ from ament/pep257
  add packages to check pep257 compliance
* use ament_pep257
* apply normpath to prevent './' prefix (fix `#24 <https://github.com/ament/ament_lint/issues/24>`_)
* Merge pull request `#22 <https://github.com/ament/ament_lint/issues/22>`_ from ament/`fix_tests-ros2/ros2#21 <https://github.com/fix_tests-ros2/ros2/issues/21>`_
  add missing error category in lint_cmake
* add missing error category in lint_cmake
* also check style of .cmake.in files
* Merge pull request `#19 <https://github.com/ament/ament_lint/issues/19>`_ from ament/split_linter_packages_in_python_and_cmake
  split linter packages in python and cmake
* make use of python linter packages
* move cmake part of ament_lint_cmake to ament_cmake_lint_cmake
* disable debug output
* Merge pull request `#16 <https://github.com/ament/ament_lint/issues/16>`_ from ament/fixup_ament_lint_cmake
  Some fixes to ament_lint_cmake
* add trailing newline to generated test result files
* add note about change from upstream
* fixup file name ends with check
* add --filters to ament_lint_cmake
* filter errors in a file using a CMake comment
  For example you can do something like:
  # lint_cmake: -package/consistency, -convention/filename
  To suppress those two categories of warnings.
  The filter is limited to the current file.
* improve SetFilters ability to parse new filters
* fix typo
* add missing copyright / license information
* Merge pull request `#14 <https://github.com/ament/ament_lint/issues/14>`_ from ament/test_runner_windows
  change test runner to work on windows
* update cmakelint to work on windows
* change test runner to work on windows
* Merge pull request `#9 <https://github.com/ament/ament_lint/issues/9>`_ from ament/docs
  add docs for linters
* add docs for linters
* modify generated unit test files for a better hierarchy in Jenkins
* make testname argument optional for all linters
* use other linters for the linter packages where possible
* Merge pull request `#2 <https://github.com/ament/ament_lint/issues/2>`_ from ament/ament_lint_auto
  allow linting based on test dependencies only
* add ament_lint_auto and ament_lint_common, update all linter packages to implement extension point of ament_lint_auto
* avoid unnecessary newlines
* use project(.. NONE)
* update to latest refactoring of ament_cmake
* add dependency on ament_cmake_environment
* add ament_lint_cmake
* Contributors: Dirk Thomas, William Woodall, dhood
