^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_lint_common
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* Fix typo in ament_lint_common/package.xml (`#336 <https://github.com/ament/ament_lint/issues/336>`_)
* Contributors: Audrow Nash, Kenji Miyake

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
* Update maintainer (`#274 <https://github.com/ament/ament_lint/issues/274>`_)
  * update maintainer
  * add authors
* Contributors: Claire Wang

0.10.0 (2020-09-18)
-------------------

0.9.4 (2020-05-26)
------------------

0.9.3 (2020-05-18)
------------------

0.9.2 (2020-05-08)
------------------

0.9.1 (2020-04-10)
------------------
* 0.9.0
* add ament_lint_common doc (`#218 <https://github.com/ament/ament_lint/issues/218>`_)
  * adding ament_lint_common doc
  * add links to common linters
  * change ament_uncrustify link
* Contributors: Chris Lalancette, Marya Belanger

0.8.1 (2019-10-23)
------------------

0.8.0 (2019-09-17)
------------------

0.7.4 (2019-07-31)
------------------
* Add clang tidy to ament linters (`#155 <https://github.com/ament/ament_lint/issues/155>`_)
  * Basic clang-lint functionality
  * Add clang tidy cmake dir
  * Add Google/default config file
  * Add xunit output
  * Add lint tests
  * Update copyright, maintainer, verbiage
* Contributors: John

0.7.3 (2019-05-09 14:08)
------------------------

0.7.2 (2019-05-09 09:30)
------------------------

0.7.1 (2019-05-07)
------------------

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
* add ament\_(cmake\_)xmllint packages (`#104 <https://github.com/ament/ament_lint/issues/104>`_)
  * add ament\_(cmake\_)xmllint packages
  * consider xsi:noNamespaceSchemaLocation of root tag
* Contributors: Dirk Thomas

0.5.2 (2018-06-27)
------------------

0.5.1 (2018-06-18 13:47)
------------------------

0.5.0 (2018-06-18 10:09)
------------------------

0.4.0 (2017-12-08)
------------------
* 0.0.3
* 0.0.2
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
* update schema url
* add schema to manifest files
* Merge pull request `#56 <https://github.com/ament/ament_lint/issues/56>`_ from ament/cmake35
  require CMake 3.5
* require CMake 3.5
* Merge pull request `#39 <https://github.com/ament/ament_lint/issues/39>`_ from ament/cpplint_python3
  use Python 3 for cpplint and split into Python and CMake package
* refactor ament_cpplint into Python and CMake package
* Merge pull request `#36 <https://github.com/ament/ament_lint/issues/36>`_ from ament/enable_cpplint
  add ament_cpplint as default linter
* add ament_cpplint as default linter
* Merge pull request `#28 <https://github.com/ament/ament_lint/issues/28>`_ from ament/pep257
  add packages to check pep257 compliance
* use ament_cmake_pep257 as a common linter
* add explicit build type
* Merge pull request `#19 <https://github.com/ament/ament_lint/issues/19>`_ from ament/split_linter_packages_in_python_and_cmake
  split linter packages in python and cmake
* move cmake part of ament_uncrustify to ament_cmake_uncrustify
* move cmake part of ament_pyflakes to ament_cmake_pyflakes
* move cmake part of ament_pep8 to ament_cmake_pep8
* move cmake part of ament_lint_cmake to ament_cmake_lint_cmake
* move cmake part of ament_cppcheck to ament_cmake_cppcheck
* Merge pull request `#8 <https://github.com/ament/ament_lint/issues/8>`_ from ament/ament_copyright
  add more options to ament_copyright
* add more options to ament_copyright
* disable clang_format and cpplint for now (fix `ros2/ros2#15 <https://github.com/ros2/ros2/issues/15>`_)
* Merge pull request `#2 <https://github.com/ament/ament_lint/issues/2>`_ from ament/ament_lint_auto
  allow linting based on test dependencies only
* add ament_lint_auto and ament_lint_common, update all linter packages to implement extension point of ament_lint_auto
* Contributors: Dirk Thomas, dhood
