^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cmake_cppcheck
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* [ament_cmake_cppcheck] Fix file exclusion behavior (`#329 <https://github.com/ament/ament_lint/issues/329>`_)
  The `EXCLUDE` argument of the `ament_cppcheck` CMake function is
  a list, i.e. a multi-value keyword. As such, it needs to be placed
  out of the one-value keywords from the `cmake_parse_arguments`
  function call.
* Update maintainers to Michael Jeronimo and Michel Hidalgo (`#340 <https://github.com/ament/ament_lint/issues/340>`_)
* Add cppcheck libraries option (`#323 <https://github.com/ament/ament_lint/issues/323>`_)
  * adding ament_cppcheck libraries option
  * pass libraries option via CMake
  Co-authored-by: William Wedler <william.wedler@resquared.com>
* Contributors: Abrar Rahman Protyasha, Audrow Nash, Will

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
* Increase the ament_cppcheck timeout to 5 minutes. (`#271 <https://github.com/ament/ament_lint/issues/271>`_)
  This will avoid timeouts on some slower platforms that we've
  started to see.
* parse LANGUAGE argument case insensitive (`#255 <https://github.com/ament/ament_lint/issues/255>`_)
* Contributors: Chris Lalancette, Karsten Knese

0.9.4 (2020-05-26)
------------------
* Allow to configure language in cppcheck with a global var (`#239 <https://github.com/ament/ament_lint/issues/239>`_)
  * Allow to configure language in cppcheck with a global var
  * Added feedback
  * added feedback
  * added feedback
  * Added feedback
  * Fixed cpplint
  * Print language message if it's not empty
  * fixed language message
  * Improved comment
* Contributors: Alejandro Hernández Cordero

0.9.3 (2020-05-18)
------------------
* Fix lint_cmake errors (`#245 <https://github.com/ament/ament_lint/issues/245>`_)
* Updated cpplint and cppcheck to exclude directories and files (`#234 <https://github.com/ament/ament_lint/issues/234>`_)
  * UPted cpplint and cppcheck to exclude directories and files
  * setting a global variable to configure automatic linting
  * Fixed cmake docbloc
  * Added feedback
  * Allowed cpplint to exclude files
  * Restored cpplint.py
* Contributors: Alejandro Hernández Cordero, Michel Hidalgo

0.9.2 (2020-05-08)
------------------
* Increase timeout for cppcheck runs. (`#229 <https://github.com/ament/ament_lint/issues/229>`_)
  On some platforms the cppcheck for the test_rclcpp package is hitting
  the timeout. Bumping the timeout on those platforms shows that the build
  takes about 130 seconds. So 180 should be sufficient room to grow
  without raising it too high.
* Contributors: Steven! Ragnarök

0.9.1 (2020-04-10)
------------------
* 0.9.0
* Behave better in multi-project cmake (`#198 <https://github.com/ament/ament_lint/issues/198>`_)
  If a sub-project uses ament_lint, don't apply it to the top-level project headers
* Contributors: Chris Lalancette, Dan Rose

0.8.1 (2019-10-23)
------------------

0.8.0 (2019-09-17)
------------------
* Alternate approach to avoiding cppcheck 1.88 (`#175 <https://github.com/ament/ament_lint/issues/175>`_)
  This approach does not require cppcheck to be present at build time.
* avoid cppcheck version 1.88 due to performance issues (`#168 <https://github.com/ament/ament_lint/issues/168>`_)
  * avoid cppcheck version 1.88 due to performance issues
  * downgrade to status from warning when skipping
* Contributors: Scott K Logan, William Woodall

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
* use globally defined varible for cppcheck include dirs (`#125 <https://github.com/ament/ament_lint/issues/125>`_)
  * use globally defined varible for cppcheck include dirs
  * update docblock
  period
* Contributors: Karsten Knese

0.6.3 (2019-01-14)
------------------
* Account for INTERFACE libraries when getting target include directories (`#121 <https://github.com/ament/ament_lint/issues/121>`_)
  * Account for INTERFACE libraries when getting target include directories
  CMake does not allow getting the INCLUDE_DIRECTORIES property from
  INTERFACE libraries.
  Instead, first check if the property exists, if it does not then try to
  get the INTERFACE_INCLUDE_DIRECTORIES property.
  Note, if INTERFACE_INCLUDE_DIRECTORIES is not defined an empty list is
  returned, but we cannot assume the target is not an interface.
  This is why the implementation is conditional on INCLUDE_DIRECTORIES
  instead.
  * Use target type property as a condition on what include directories property to use
  * Increase cppcheck test timeout to 120s
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

0.5.0 (2018-06-18 10:09)
------------------------

0.4.0 (2017-12-08)
------------------
* 0.0.3
* 0.0.2
* opt to pass the language through to cppcheck (`#79 <https://github.com/ament/ament_lint/issues/79>`_)
  * opt to pass the language through to cppcheck
  * add explicit LANGUAGE argument
  * alpha ordering on arguments and typo
  * fixup
* update schema url
* add schema to manifest files
* Merge pull request `#56 <https://github.com/ament/ament_lint/issues/56>`_ from ament/cmake35
  require CMake 3.5
* require CMake 3.5
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
* move cmake part of ament_lint_cmake to ament_cmake_lint_cmake
* move cmake part of ament_cppcheck to ament_cmake_cppcheck
* Contributors: Dirk Thomas, William Woodall
