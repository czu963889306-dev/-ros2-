^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cmake_test
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
* backport merge `#381 <https://github.com/ament/ament_cmake/issues/381>`_ to humble (`#490 <https://github.com/ament/ament_cmake/issues/490>`_)
* Contributors: El Jawad Alaa

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
* Resolve various ament_lint linter violations (`#360 <https://github.com/ament/ament_cmake/issues/360>`_)
  We can't add ament_lint linters in ament_cmake in the traditional way
  without creating a circular dependency between the repositories. Even
  though we can't automatically enforce linting, it's still a good idea to
  try to keep conformance where possible.
* Update maintainers to Michael Jeronimo and Michel Hidalgo (`#362 <https://github.com/ament/ament_cmake/issues/362>`_)
* Contributors: Audrow Nash, Scott K Logan

1.2.0 (2021-10-29)
------------------
* Use FindPython3 instead of FindPythonInterp (`#355 <https://github.com/ament/ament_cmake/issues/355>`_)
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
* Fix skipped test reporting in CTest (`#279 <https://github.com/ament/ament_cmake/issues/279>`_)
  This is a follow-up to c67cdf2. When the SKIP_RETURN_CODE gets set to 0,
  the value is interpreted as 'false', and the test property is never
  actually added.
* limit test time to three decimals (`#271 <https://github.com/ament/ament_cmake/issues/271>`_)
* Add actual test time to xUnit result files (`#270 <https://github.com/ament/ament_cmake/issues/270>`_)
  * Add actual test time to xUnit result files
  Fixes `#269 <https://github.com/ament/ament_cmake/issues/269>`_
  * Report test_time even with skipped test
  * Set time attribute for testcase element
* Contributors: Dirk Thomas, Ruffin, Scott K Logan

1.0.0 (2020-07-22)
------------------
* Add SKIP_RETURN_CODE argument to ament_add_test (`#264 <https://github.com/ament/ament_cmake/issues/264>`_)
  This makes the `run_test.py` wrapper aware of the `SKIP_RETURN_CODE`
  property on CTest tests. In the existing implementation, the wrapper
  detects that no result file was generated and overrides the special
  return code coming from the test, making the the CTest feature fail
  completely.
  This change makes the wrapper script aware of the special return code,
  and when detected, will write a 'skipped' result file instead of a
  'failed' result file, and pass along the special return code as-is. Now
  the gtest result and the ctest results both show the test as 'skipped'
  when the special return flag is used.
  Note that none of this behavior is enabled by default, which is
  important because we wouldn't want a test to fail and return a code
  which we've decided is the special 'skip' return code. Only tests which
  are aware of this feature should use it.
* Contributors: Scott K Logan

0.9.6 (2020-06-23)
------------------

0.9.5 (2020-06-02)
------------------
* Merge pull request `#253 <https://github.com/ament/ament_cmake/issues/253>`_ from ament/use_errors_tag2
  Use errors attribute for problems when testing code (take II)
* Error message needs to be inside its own XML tag according to XSD
* Use DEPRECATION instead of WARNING for package deprecation messages
  This makes it possible to treat the warnings differently in downstream packages.
  Refer to the CMake documentation for more info: https://cmake.org/cmake/help/v3.0/command/message.html
* Contributors: Jose Luis Rivero

0.9.4 (2020-05-26)
------------------

0.9.3 (2020-05-19)
------------------

0.9.2 (2020-05-07)
------------------
* Fix parallel testing (`#254 <https://github.com/ament/ament_cmake/issues/254>`_)
  * Fix parallel testing
  We ran ctest . -j 10, and sometimes it happened that we got failing CI builds because the command in line 116 was executed in parallel.
  ```
  [2020-04-28T19:13:39.193Z] 1: Traceback (most recent call last):
  [2020-04-28T19:13:39.193Z] 1:   File "/opt/ros/eloquent/share/ament_cmake_test/cmake/run_test.py", line 23, in <module>
  [2020-04-28T19:13:39.193Z] 1:     sys.exit(ament_cmake_test.main())
  [2020-04-28T19:13:39.193Z] 1:   File "/opt/ros/eloquent/lib/python3.6/site-packages/ament_cmake_test/__init_\_.py", line 116, in main
  [2020-04-28T19:13:39.193Z] 1:     os.makedirs(output_path)
  [2020-04-28T19:13:39.193Z] 1:   File "/usr/lib/python3.6/os.py", line 220, in makedirs
  [2020-04-28T19:13:39.193Z] 1:     mkdir(name, mode)
  [2020-04-28T19:13:39.193Z] 1: FileExistsError: [Errno 17] File exists: 'some_dir/build/x86_debug/ros2/build_docker/functions/ament_cmake_gtest'
  ```
  * remove condition
* Contributors: Florian Berchtold

0.9.1 (2020-04-24 15:45)
------------------------

0.9.0 (2020-04-24 12:25)
------------------------
* Report skipped tests in CTest output (`#243 <https://github.com/ament/ament_cmake/issues/243>`_)
  When adding a test using `ament_add_test`, the `SKIP_TEST` argument
  results in the `--skip-test` argument being passed to the test wrapper
  script `run_test.py`. The wrapper script then writes a JUnit output
  describing that the test was skipped, and returns 0.
  As far as CTest knows, the test succeeded and shows `Passed` on the
  console. However, since we know that the test will be skipped by the
  wrapper, and we expect the wrapper to return 0 after it writes the JUnit
  file, we can set a test property that will mark the test as `Skipped`
  when the wrapper returns 0.
  This way, the JUnit output file is still written, but CTest displays the
  test as skipped as well.
* Drop duplicated <skipped/> element in result file (`#242 <https://github.com/ament/ament_cmake/issues/242>`_)
  The `<skipped/>` element was actually added as part of the
  `skipped_message` several lines earlier.
  While multiple `<skipped/>` elements doesn't violate the JUnit schema,
  there is no reason to have more than one.
* add CMake function ament_add_test_label() (`#240 <https://github.com/ament/ament_cmake/issues/240>`_)
* Merge pull request `#225 <https://github.com/ament/ament_cmake/issues/225>`_ from ament/junit10_xsd
  Generate xunit files valid for the junit10.xsd
* Generate xunit files valid for the junit10.xsd
* Declare AMENT_TEST_RESULTS_DIR as a PATH (`#221 <https://github.com/ament/ament_cmake/issues/221>`_)
* remove status attribute from result XML, add skipped tag instead (`#218 <https://github.com/ament/ament_cmake/issues/218>`_)
* Run tests in current binary directory, not global source directory (`#206 <https://github.com/ament/ament_cmake/issues/206>`_)
  Switch to CMAKE_CURRENT_BINARY_DIR for consistency with CTest
* Contributors: Dan Rose, Dirk Thomas, Jose Luis Rivero, Scott K Logan

0.8.1 (2019-10-23)
------------------

0.8.0 (2019-10-04)
------------------
* use deterministic order for updated env vars (`#196 <https://github.com/ament/ament_cmake/issues/196>`_)
* improve handling of encoding (`#181 <https://github.com/ament/ament_cmake/issues/181>`_)
* Add runner option to ament_add_test (`#174 <https://github.com/ament/ament_cmake/issues/174>`_)
  * ament_cmake allow speficiation of a different test runner
  - By default, still uses run_test.py
  - Example use case: ament_cmake_ros can use a test runner that sets a ROS_DOMAIN_ID
  * ament_cmake move run_test.py to a python module
  - This should let us see the history
  * ament_cmake refactor run_test.py into an importable python module
  - Adds an ament_cmake_test python package
* Contributors: Dirk Thomas, Peter Baughman

0.7.3 (2019-05-29)
------------------
* close output_handle explicitly (`#171 <https://github.com/ament/ament_cmake/issues/171>`_)
* Contributors: Dirk Thomas

0.7.2 (2019-05-20)
------------------

0.7.1 (2019-05-07)
------------------

0.7.0 (2019-04-08)
------------------
* Fix typo (`#163 <https://github.com/ament/ament_cmake/issues/163>`_)
* use enable_testing() insted of CTest module (`#153 <https://github.com/ament/ament_cmake/issues/153>`_)
  use enable_testing() instead of CTest module
* Contributors: Dirk Thomas, Esteve Fernandez

0.6.0 (2018-11-13)
------------------

0.5.1 (2018-07-17)
------------------

0.5.0 (2018-06-13)
------------------

0.4.0 (2017-12-08)
------------------
* Merge pull request `#117 <https://github.com/ament/ament_cmake/issues/117>`_ from ament/gtest_classname
  inject classname for gtest result files
* inject classname for gtest result files
* 0.0.3
* Merge pull request `#107 <https://github.com/ament/ament_cmake/issues/107>`_ from ament/flake8_plugins
  update style to satisfy new flake8 plugins
* update style to satisfy new flake8 plugins
* Merge pull request `#101 <https://github.com/ament/ament_cmake/issues/101>`_ from ament/pass_env_with_list_value
  merge env values which were split on semicolons
* print set env message all at once (`#102 <https://github.com/ament/ament_cmake/issues/102>`_)
  * print set env message all at once
  * address comments
* merge env values which were split on semicolons
* 0.0.2
* Merge pull request `#86 <https://github.com/ament/ament_cmake/issues/86>`_ from ament/remove_include
  remove unnecessary include
* remove unnecessary include
* Merge pull request `#85 <https://github.com/ament/ament_cmake/issues/85>`_ from ament/split_gtest_function
  Split ament_add_gtest function
* add doc for SKIP_TEST
* remove __future_\_ imports
* Skipped tests (`#80 <https://github.com/ament/ament_cmake/issues/80>`_)
  * support skipping tests
  * add SKIP_TEST to ament_add_nose_test
  * use keyword args not positional
  * discard positional args after first
* update schema url
* add schema to manifest files
* Merge pull request `#72 <https://github.com/ament/ament_cmake/issues/72>`_ from ament/cmake35
  require CMake 3.5
* require CMake 3.5
* Merge pull request `#68 <https://github.com/ament/ament_cmake/issues/68>`_ from ament/ctest_build_testing
  use CTest BUILD_TESTING
* use CTest BUILD_TESTING
* generate all ament index markers into <build>/ament_index_preinstall
  * use compliant layout for index resources in build space and allow using those
  * fix optional arguments of ament_index_register_package
  * allow to skip the AMENT_PREFIX_PATH and / or the folder in the binary dir
  * fix error handling error
  * allow overriding default prefix path for ament index CMake API
  * undo any ; -> \; substitution done to pass PATH lists on Windows
  * only replace : with ; when no on Windows
* Merge pull request `#53 <https://github.com/ament/ament_cmake/issues/53>`_ from ament/library_path_env_var
  change CMake logic to determine env var name for library path
* Merge pull request `#54 <https://github.com/ament/ament_cmake/issues/54>`_ from ament/test_working_dir
  support WORKING_DIRECTORY in ament_add_nose_test
* fix WORKING_DIRECTORY for ament_add_gtest/gmock
* change CMake logic to determine env var name for library path
* follow fixes from `#52 <https://github.com/ament/ament_cmake/issues/52>`_
* Merge pull request `#52 <https://github.com/ament/ament_cmake/issues/52>`_ from ament/add_test_append_env_option
  add APPEND_ENV and APPEND_LIBRARY_DIRS options to ament_add\_*test macros
* add APPEND_ENV and APPEND_LIBRARY_DIRS options to ament_add\_*test macros
* Merge pull request `#50 <https://github.com/ament/ament_cmake/issues/50>`_ from ament/pass_extra_env_to_tests
  add option to pass extra env to ament_add\_*test
* minor style change, changing split logic
* addressing comments
* Merge pull request `#48 <https://github.com/ament/ament_cmake/issues/48>`_ from ament/verify_tidy_all_result_files
  verify and tidy all result files
* add option to pass extra env to ament_add\_*test
* verify and tidy all result files
* Merge pull request `#32 <https://github.com/ament/ament_cmake/issues/32>`_ from ament/change_missing_result_file
  move '.missing_result' suffix from testsuite name to testcase name
* move '.missing_result' suffix from testsuite name to testcase name
* Merge pull request `#28 <https://github.com/ament/ament_cmake/issues/28>`_ from ament/gtest_location
  fix location of gtest / gmock executables on Windows
* add type as extension to test result files
* never truncate ctest dashboard summary
* Merge pull request `#24 <https://github.com/ament/ament_cmake/issues/24>`_ from ament/test_repeated_publisher_subscriber
  change reading from proc, add invoked command as well as return code / exception to output file
* change reading from proc, also write all printed messages to output file
* Merge pull request `#19 <https://github.com/ament/ament_cmake/issues/19>`_ from ament/improve_test_runner
  improve test runner
* improve test runner
* add explicit build type
* improve reporting of failing tests and tests missing a result file
* disable debug output
* Merge pull request `#10 <https://github.com/ament/ament_cmake/issues/10>`_ from ament/always_print_test_output
  always print test output to console
* always print test output to console
* add missing copyright / license information, update format of existing license information
* Merge pull request `#7 <https://github.com/ament/ament_cmake/issues/7>`_ from ament/test_runner_windows
  change test runner to work on windows
* change test runner to work on windows
* use project(.. NONE)
* refactor several low-level packages into ament_cmake_core (environment, environment_hooks, index, package_templates, symlink_install)
* invert dependency between ament_cmake_environment and ament_cmake_environment_hooks, add dependency on ament_cmake_environment
* deal with CMake double expansion
* update cmake code style
* minor fixes
* add ament_cmake_environment_hooks
* add ament_cmake_test, ament_cmake_gtest, ament_cmake_nose
* Contributors: Dirk Thomas, Mikael Arguedas, William Woodall
