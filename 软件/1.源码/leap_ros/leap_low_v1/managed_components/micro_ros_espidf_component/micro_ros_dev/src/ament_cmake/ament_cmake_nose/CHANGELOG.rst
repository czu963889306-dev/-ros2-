^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cmake_nose
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.3.14 (2026-02-23)
-------------------

1.3.13 (2026-01-13)
-------------------

1.3.12 (2025-06-27)
-------------------

1.3.11 (2024-11-25)
-------------------

1.3.10 (2024-07-26)
-------------------

1.3.9 (2024-05-15)
------------------

1.3.8 (2024-02-16)
------------------

1.3.7 (2024-01-24)
------------------

1.3.6 (2023-11-13)
------------------

1.3.5 (2023-06-22)
------------------

1.3.4 (2023-04-25)
------------------

1.3.3 (2022-11-07)
------------------

1.3.2 (2022-05-17)
------------------

1.3.1 (2022-03-28)
------------------

1.3.0 (2022-02-17)
------------------
* Update forthcoming version in changelog
* Contributors: Audrow Nash

1.2.1 (2022-01-14)
------------------
* Update maintainers to Michael Jeronimo and Michel Hidalgo (`#362 <https://github.com/ament/ament_cmake/issues/362>`_)
* Contributors: Audrow Nash

1.2.0 (2021-10-29)
------------------
* Use FindPython3 instead of FindPythonInterp (`#355 <https://github.com/ament/ament_cmake/issues/355>`_)
* Support commands with executable targets (`#352 <https://github.com/ament/ament_cmake/issues/352>`_)
* Update maintainers (`#336 <https://github.com/ament/ament_cmake/issues/336>`_)
* Contributors: Chris Lalancette, Shane Loretz

1.1.4 (2021-05-06)
------------------

1.1.3 (2021-03-09)
------------------

1.1.2 (2021-02-26 22:59)
------------------------

1.1.1 (2021-02-26 19:12)
------------------------

1.1.0 (2021-02-24)
------------------

1.0.4 (2021-01-25)
------------------

1.0.3 (2020-12-10)
------------------

1.0.2 (2020-12-07)
------------------
* Update package maintainers. (`#286 <https://github.com/ament/ament_cmake/issues/286>`_)
* Contributors: Michel Hidalgo

1.0.1 (2020-09-10)
------------------

1.0.0 (2020-07-22)
------------------

0.9.6 (2020-06-23)
------------------

0.9.5 (2020-06-02)
------------------

0.9.4 (2020-05-26)
------------------

0.9.3 (2020-05-19)
------------------

0.9.2 (2020-05-07)
------------------

0.9.1 (2020-04-24 15:45)
------------------------

0.9.0 (2020-04-24 12:25)
------------------------

0.8.1 (2019-10-23)
------------------

0.8.0 (2019-10-04)
------------------
* Add 'runner' option to ament_add_gmock / nose (`#177 <https://github.com/ament/ament_cmake/issues/177>`_)
  * Add 'runner' option to ament_add_gmock
  * Give ament_add_nose ability to specify a different runner, too
* Contributors: Peter Baughman

0.7.3 (2019-05-29)
------------------

0.7.2 (2019-05-20)
------------------

0.7.1 (2019-05-07)
------------------
* Fix unused-arg check in ament_cmake packages: (`#167 <https://github.com/ament/ament_cmake/issues/167>`_)
  Arguments to a macro are not variables, so it's not
  possible to do 'if(ARGN)' to check for arguments;
  however, copying ARGN to a variable works.
* Contributors: jpsamper2009

0.7.0 (2019-04-08)
------------------

0.6.0 (2018-11-13)
------------------

0.5.1 (2018-07-17)
------------------

0.5.0 (2018-06-13)
------------------

0.4.0 (2017-12-08)
------------------
* 0.0.3
* Merge pull request `#103 <https://github.com/ament/ament_cmake/issues/103>`_ from ament/resolve_some_todos
  Resolve some todos
* remove obsolete todos
* Get nose tests to immediately print console output (`#98 <https://github.com/ament/ament_cmake/issues/98>`_)
  This is useful for tests that timeout and get killed without an opportunity to print the console output that was captured
* 0.0.2
* Use python3-nose rosdep key. (`#95 <https://github.com/ament/ament_cmake/issues/95>`_)
* Merge pull request `#86 <https://github.com/ament/ament_cmake/issues/86>`_ from ament/remove_include
  remove unnecessary include
* remove unnecessary include
* Merge pull request `#85 <https://github.com/ament/ament_cmake/issues/85>`_ from ament/split_gtest_function
  Split ament_add_gtest function
* add doc for SKIP_TEST
* Skipped tests (`#80 <https://github.com/ament/ament_cmake/issues/80>`_)
  * support skipping tests
  * add SKIP_TEST to ament_add_nose_test
  * use keyword args not positional
  * discard positional args after first
* remove trailing whitespace
* update schema url
* add schema to manifest files
* Windows python debug (`#73 <https://github.com/ament/ament_cmake/issues/73>`_)
  * add python interpreter to nose test parameters
  * update doc
  * rename interpreter to executable and add doc
* Merge pull request `#72 <https://github.com/ament/ament_cmake/issues/72>`_ from ament/cmake35
  require CMake 3.5
* remove trailing spaces from comparisons, obsolete quotes and explicit variable expansion
* require CMake 3.5
* run nosetests with the python executable (`#70 <https://github.com/ament/ament_cmake/issues/70>`_)
  * run nosetests with the python executable
  * comment to describe the source of the issue
  * fixup
* Merge pull request `#55 <https://github.com/ament/ament_cmake/issues/55>`_ from ament/generator_expression
  allow tests with generator expression in the path
* allow tests with generator expression in the path
* Merge pull request `#54 <https://github.com/ament/ament_cmake/issues/54>`_ from ament/test_working_dir
  support WORKING_DIRECTORY in ament_add_nose_test
* add WORKING_DIRECTORY to ament_add_nose_test
* follow fixes from `#52 <https://github.com/ament/ament_cmake/issues/52>`_
* Merge pull request `#52 <https://github.com/ament/ament_cmake/issues/52>`_ from ament/add_test_append_env_option
  add APPEND_ENV and APPEND_LIBRARY_DIRS options to ament_add\_*test macros
* add APPEND_ENV and APPEND_LIBRARY_DIRS options to ament_add\_*test macros
* Merge pull request `#46 <https://github.com/ament/ament_cmake/issues/46>`_ from ament/nosetest_prefix_testsuite
  use --xunit-prefix-with-testsuite-name option of upcoming nosetests version
* use --xunit-prefix-with-testsuite-name option of upcoming nosetests version
* Merge pull request `#43 <https://github.com/ament/ament_cmake/issues/43>`_ from ament/fix_build_with_spaces
  invoke nosetest through Python executable
* invoke nosetest through Python executable
* Merge pull request `#37 <https://github.com/ament/ament_cmake/issues/37>`_ from ament/test_labels
  add labels to tests
* add labels to tests
* Merge pull request `#36 <https://github.com/ament/ament_cmake/issues/36>`_ from ament/version_less_cmake
  Use VERSION_LESS to test the Nose version
* Use VERSION_LESS to test the Nose version
  `VERSION_LESS` is used for checking versions:
  http://cmake.org/cmake/help/v2.8.12/cmake.html#command:if
* Merge pull request `#33 <https://github.com/ament/ament_cmake/issues/33>`_ from ament/nosetest_version
  determine nosetest version in CMake and use --xunit-testsuite-name when available
* determine nosetest version in CMake and use --xunit-testsuite-name when available
* Merge pull request `#28 <https://github.com/ament/ament_cmake/issues/28>`_ from ament/gtest_location
  fix location of gtest / gmock executables on Windows
* add type as extension to test result files
* fix name of nosetests output file
* Merge pull request `#19 <https://github.com/ament/ament_cmake/issues/19>`_ from ament/improve_test_runner
  improve test runner
* improve test runner
* add explicit build type
* disable debug output
* add missing copyright / license information, update format of existing license information
* update quoting of additional ament_add_test() arguments
* use project(.. NONE)
* refactor several low-level packages into ament_cmake_core (environment, environment_hooks, index, package_templates, symlink_install)
* invert dependency between ament_cmake_environment and ament_cmake_environment_hooks, add dependency on ament_cmake_environment
* deal with CMake double expansion
* update cmake code style
* add ament_cmake_environment_hooks
* add ament_cmake_test, ament_cmake_gtest, ament_cmake_nose
* Contributors: Dirk Thomas, Esteve Fernandez, Mikael Arguedas, Steven! Ragnarök, William Woodall, dhood
