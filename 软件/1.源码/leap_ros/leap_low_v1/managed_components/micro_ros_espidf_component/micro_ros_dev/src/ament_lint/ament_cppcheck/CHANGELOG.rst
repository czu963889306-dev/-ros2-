^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cppcheck
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* Disable cppcheck 2.x. (`#345 <https://github.com/ament/ament_lint/issues/345>`_)
* Contributors: Chris Lalancette

0.11.4 (2022-01-14)
-------------------
* Update forthcoming version in changelogs
* Contributors: Audrow Nash

0.11.3 (2022-01-14)
-------------------
* Update maintainers to Michael Jeronimo and Michel Hidalgo (`#340 <https://github.com/ament/ament_lint/issues/340>`_)
* Add cppcheck libraries option (`#323 <https://github.com/ament/ament_lint/issues/323>`_)
  * adding ament_cppcheck libraries option
  * pass libraries option via CMake
  Co-authored-by: William Wedler <william.wedler@resquared.com>
* Contributors: Audrow Nash, Will

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
* Fix file exclusion behavior in ament_cppcheck and ament_cpplint (`#299 <https://github.com/ament/ament_lint/issues/299>`_)
  * fix exclude behavior in ament_cppcheck and ament_cpplint
  * fix flake8 errors
  * add missing realpath() conversion
* Contributors: Audrow Nash, M. Mei

0.10.2 (2021-02-12)
-------------------
* Add Audrow as a maintainer (`#294 <https://github.com/ament/ament_lint/issues/294>`_)
* Drop trailing tab from package manifests (`#291 <https://github.com/ament/ament_lint/issues/291>`_)
  Follow-up to 8bf194aa1ac282db5483dd0d3fefff8f325b0db8
* Contributors: Audrow Nash, Scott K Logan

0.10.1 (2021-01-25)
-------------------
* Suppress unknownMacro (`#268 <https://github.com/ament/ament_lint/issues/268>`_)
  cppcheck creates an unknownMacro error when it cannot resolve a macro.
  Since we don't pass in all dependent headers, we don't expect all macros to be discoverable by cppcheck.
* Update maintainer (`#274 <https://github.com/ament/ament_lint/issues/274>`_)
  * update maintainer
  * add authors
* Contributors: Claire Wang, Dan Rose

0.10.0 (2020-09-18)
-------------------
* Add pytest.ini so local tests don't display warning. (`#259 <https://github.com/ament/ament_lint/issues/259>`_)
* Contributors: Chris Lalancette

0.9.4 (2020-05-26)
------------------

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
* Allow AMENT_IGNORE markers to be directories (`#232 <https://github.com/ament/ament_lint/issues/232>`_)
* Contributors: Dan Rose

0.9.1 (2020-04-10)
------------------
* 0.9.0
* Generate xunit files valid for the junit10.xsd (`#220 <https://github.com/ament/ament_lint/issues/220>`_)
* Suppress internalAstError (`#219 <https://github.com/ament/ament_lint/issues/219>`_)
  `cppcheck` can, in some cases, construct an invalid parse tree.
  http://build.ros2.org/user/rotu/my-views/view/CycloneDDS/job/Fci__nightly-cyclonedds_ubuntu_focal_amd64/lastCompletedBuild/testReport/rclcpp/cppcheck/error__internalAstError__src_rclcpp_clock_cpp_159\_/
  ```
  - rclcpp.cppcheck error: internalAstError (src/rclcpp/clock.cpp:159)
  <<< failure message
  Syntax Error: AST broken, 'if' doesn't have two operands.
  >>>
  ```
  This error sounds like a syntax error, but is in fact, not. (cppcheck doesn't seem to even find the syntax error in `if(1,1){}`)
  This commit causes such errors to be silently ignored, as they are not actionable.
* [ament_cppcheck] Report errors from additional includes (`#216 <https://github.com/ament/ament_lint/issues/216>`_)
  * [ament_cppcheck] Report errors from additional includes
  Before, if an error was found in an additional include we get a KeyError exception.
  * Use defaultdict
* remove status attribute from result XML (`#212 <https://github.com/ament/ament_lint/issues/212>`_)
* Contributors: Chris Lalancette, Dan Rose, Dirk Thomas, Jacob Perron, Jose Luis Rivero

0.8.1 (2019-10-23)
------------------
* install resource marker file for packages (`#193 <https://github.com/ament/ament_lint/issues/193>`_)
* Contributors: Dirk Thomas

0.8.0 (2019-09-17)
------------------
* fix handling mixed relative/absolute path in ament_cppcheck (`#188 <https://github.com/ament/ament_lint/issues/188>`_)
  * fix handling mixed relative/absolute path in ament_cppcheck
  * use 'in' operator
  * use os.path.samefile so it doesn't matter which of the two args is relative / absolute / symlinked
  * skip printing duplicate errors
* install manifest files in Python packages (`#185 <https://github.com/ament/ament_lint/issues/185>`_)
* Alternate approach to avoiding cppcheck 1.88 (`#175 <https://github.com/ament/ament_lint/issues/175>`_)
  This approach does not require cppcheck to be present at build time.
* avoid cppcheck version 1.88 due to performance issues (`#168 <https://github.com/ament/ament_lint/issues/168>`_)
  * avoid cppcheck version 1.88 due to performance issues
  * downgrade to status from warning when skipping
* Contributors: Dirk Thomas, Scott K Logan, William Woodall

0.7.4 (2019-07-31)
------------------

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
* fix cppcheck report to list checked files (`#134 <https://github.com/ament/ament_lint/issues/134>`_)
* Contributors: Dirk Thomas

0.6.3 (2019-01-14)
------------------
* Pass include paths to cppcheck (`#117 <https://github.com/ament/ament_lint/issues/117>`_)
  * Use BUILDSYSTEM_TARGETS list for getting include directories
  * Only pass include directories that are subdirectories of the package being tested
  This eliminates the need for a longer test timeout and avoids cppcheck from testing external files.
  Reverted prior changes accordingly.
  * Handle case when cppcheck reports error in filename with arbitrary path
  * Add find_package and dependency tag for ament_cmake_core
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
* level setup.py versions to 0.5.1
* Contributors: Mikael Arguedas

0.5.0 (2018-06-18 10:09)
------------------------
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
* opt to pass the language through to cppcheck (`#79 <https://github.com/ament/ament_lint/issues/79>`_)
  * opt to pass the language through to cppcheck
  * add explicit LANGUAGE argument
  * alpha ordering on arguments and typo
  * fixup
* [cppcheck] Remove xml warning (`#69 <https://github.com/ament/ament_lint/issues/69>`_)
  * update to xml version 2, v1 being deprecated
  * parse xml v2 error outputs
  * remove unnecessary variable
* remove __future_\_ imports
* update schema url
* add schema to manifest files
* Merge pull request `#42 <https://github.com/ament/ament_lint/issues/42>`_ from ament/remove_second_extension
  remove result type extension from testsuite name
* remove result type extension from testsuite name
* apply normpath to prevent './' prefix (fix `#24 <https://github.com/ament/ament_lint/issues/24>`_)
* clarify help for ament_cppcheck
* improve error messages
* Merge pull request `#19 <https://github.com/ament/ament_lint/issues/19>`_ from ament/split_linter_packages_in_python_and_cmake
  split linter packages in python and cmake
* move cmake part of ament_cppcheck to ament_cmake_cppcheck
* disable debug output
* Merge pull request `#17 <https://github.com/ament/ament_lint/issues/17>`_ from ament/cppcheck_windows
  find cppcheck on windows
* find cppcheck on windows, use env var for program files location
* add trailing newline to generated test result files
* add missing copyright / license information
* Merge pull request `#14 <https://github.com/ament/ament_lint/issues/14>`_ from ament/test_runner_windows
  change test runner to work on windows
* change test runner to work on windows
* Merge pull request `#9 <https://github.com/ament/ament_lint/issues/9>`_ from ament/docs
  add docs for linters
* add docs for linters
* modify generated unit test files for a better hierarchy in Jenkins
* fix copy-n-pasted license names
* make testname argument optional for all linters
* use other linters for the linter packages where possible
* Merge pull request `#2 <https://github.com/ament/ament_lint/issues/2>`_ from ament/ament_lint_auto
  allow linting based on test dependencies only
* add ament_lint_auto and ament_lint_common, update all linter packages to implement extension point of ament_lint_auto
* use project(.. NONE)
* update to latest refactoring of ament_cmake
* add dependency on ament_cmake_environment
* add ament_cppcheck
* Contributors: Dirk Thomas, Mikael Arguedas, William Woodall
