^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cmake_uncrustify
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* [ament_lint_auto] General file exclusion with AMENT_LINT_AUTO_FILE_EXCLUDE (`#386 <https://github.com/ament/ament_lint/issues/386>`_) (`#445 <https://github.com/ament/ament_lint/issues/445>`_)
* Contributors: mergify[bot]

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
* [ament_cmake_uncrustify] Add file exclude support (`#330 <https://github.com/ament/ament_lint/issues/330>`_)
  In the `ament_uncrustify` CMake function, the optional list
  argument `EXCLUDE` can now be used as an exclusion specifier.
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
* parse LANGUAGE argument case insensitive (`#255 <https://github.com/ament/ament_lint/issues/255>`_)
* Contributors: Karsten Knese

0.9.4 (2020-05-26)
------------------
* aditional -> additional (`#250 <https://github.com/ament/ament_lint/issues/250>`_)
* Contributors: Chris Lalancette

0.9.3 (2020-05-18)
------------------
* Updated cpplint and cppcheck to exclude directories and files (`#234 <https://github.com/ament/ament_lint/issues/234>`_)
  * UPted cpplint and cppcheck to exclude directories and files
  * setting a global variable to configure automatic linting
  * Fixed cmake docbloc
  * Added feedback
  * Allowed cpplint to exclude files
  * Restored cpplint.py
* Contributors: Alejandro Hernández Cordero

0.9.2 (2020-05-08)
------------------

0.9.1 (2020-04-10)
------------------
* 0.9.0
* add ament_lint_common doc (`#218 <https://github.com/ament/ament_lint/issues/218>`_)
  * adding ament_lint_common doc
  * add links to common linters
  * change ament_uncrustify link
* pass explicit language to uncrustify (`#214 <https://github.com/ament/ament_lint/issues/214>`_)
  * pass explicitl language to uncrustify
  * remove dst before renaming file to work on Windows
  * use dict comprehension
  * conditionally pass -l in both commands
  * remove unncessary line
  * fix comprehension
  * fix dict after fiddling with comprehension
  * add CMake option for force specific language for uncrustify
  * feedback about help message
* Allow custom config file for ament_uncrustify (`#200 <https://github.com/ament/ament_lint/issues/200>`_)
  * parse arguements for CONFIG_FILE and append to cmd
  * removed newline
  * fixed description
  * corrected location of default configuration file in docblock
  * fixed alignment in docblock
  * reverted deletion of empty line after copyright header
* Contributors: Chris Lalancette, Dirk Thomas, Marya Belanger, Yadu

0.8.1 (2019-10-23)
------------------

0.8.0 (2019-09-17)
------------------

0.7.4 (2019-07-31)
------------------

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
* [ament_uncrustify] Account for zero value in line length cmake arg
  A zero value will cause uncrustify to ignore line length.
* Contributors: Jacob Perron

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

0.5.0 (2018-06-18 10:09)
------------------------

0.4.0 (2017-12-08)
------------------
* 0.0.3
* 0.0.2
* update schema url
* add schema to manifest files
* Merge pull request `#56 <https://github.com/ament/ament_lint/issues/56>`_ from ament/cmake35
  require CMake 3.5
* require CMake 3.5
* Merge pull request `#51 <https://github.com/ament/ament_lint/issues/51>`_ from ament/lint_generated_code
  extend linter API to allow overriding the max line length
* add CMake argument to override max line length for linters as well as the root for cpplint
* Merge pull request `#50 <https://github.com/ament/ament_lint/issues/50>`_ from ament/ctest_build_testing
  use CTest BUILD_TESTING
* use CTest BUILD_TESTING
* Merge pull request `#41 <https://github.com/ament/ament_lint/issues/41>`_ from ament/use_message_status
  avoid using message without STATUS
* avoid using message without STATUS
* Merge pull request `#30 <https://github.com/ament/ament_lint/issues/30>`_ from ament/test_labels
  add labels to tests
* add labels to tests
* Merge pull request `#29 <https://github.com/ament/ament_lint/issues/29>`_ from ament/change_test_dependencies
  update documentation for linters
* update documentation for linters
* Merge pull request `#27 <https://github.com/ament/ament_lint/issues/27>`_ from ament/gtest_location
  add type as extension to test result files
* add type as extension to test result files
* add explicit build type
* Merge pull request `#19 <https://github.com/ament/ament_lint/issues/19>`_ from ament/split_linter_packages_in_python_and_cmake
  split linter packages in python and cmake
* make use of python linter packages
* move cmake part of ament_uncrustify to ament_cmake_uncrustify
* Contributors: Dirk Thomas
