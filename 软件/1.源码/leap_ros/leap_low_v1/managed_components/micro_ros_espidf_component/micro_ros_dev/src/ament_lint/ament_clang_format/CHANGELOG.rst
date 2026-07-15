^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_clang_format
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* Contributors: Audrow Nash

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
* Add clang-format-version option to ament_clang_format (`#282 <https://github.com/ament/ament_lint/issues/282>`_)
* Update maintainer (`#274 <https://github.com/ament/ament_lint/issues/274>`_)
  * update maintainer
  * add authors
* Contributors: Claire Wang, Tyler Weaver

0.10.0 (2020-09-18)
-------------------
* Add pytest.ini so local tests don't display warning. (`#259 <https://github.com/ament/ament_lint/issues/259>`_)
* Contributors: Chris Lalancette

0.9.4 (2020-05-26)
------------------

0.9.3 (2020-05-18)
------------------
* Fix new flake8 errors. (`#244 <https://github.com/ament/ament_lint/issues/244>`_)
* Contributors: Michel Hidalgo

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
* Convert list comprehension to generator (`#179 <https://github.com/ament/ament_lint/issues/179>`_)
  Addresses flake8 C412 errors introduced by flake8-comprehension 2.2.0
* Bring clang-format in line with developer guide (`#147 <https://github.com/ament/ament_lint/issues/147>`_)
  * Bring clang-format in line with developer guide
  https://index.ros.org/doc/ros2/Contributing/Developer-Guide/#id26
  * Fix brace wrapping
  * Fix indent for initializer continuation
  * Minor changes to clang-format
  * Alphabetize options
* Contributors: Dan Rose, Dirk Thomas, Scott K Logan

0.7.4 (2019-07-31)
------------------
* Add python3-yaml depends to ament_clang_tidy/format (`#166 <https://github.com/ament/ament_lint/issues/166>`_)
  depend -> exec_depend
  Remove whitespace
* Add missing pyyaml dependency (`#150 <https://github.com/ament/ament_lint/issues/150>`_)
* Contributors: Dan Rose, John

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
* use safe_load instead of deprecated load (`#135 <https://github.com/ament/ament_lint/issues/135>`_)
* Contributors: Mikael Arguedas

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
* use flake8 instead of pep8 and pyflakes
* remove __future_\_ imports
* update schema url
* add schema to manifest files
* Merge pull request `#55 <https://github.com/ament/ament_lint/issues/55>`_ from ament/ament_clang_format
  update clang format
* add clang-format binary name for OS X
* use config file for clang_format (currently google style)
* update ament\_(cmake\_)clang_format
* split ament_clang_format into Python and CMake package, plain move / copy without changes
* Merge pull request `#50 <https://github.com/ament/ament_lint/issues/50>`_ from ament/ctest_build_testing
  use CTest BUILD_TESTING
* use CTest BUILD_TESTING
* Merge pull request `#42 <https://github.com/ament/ament_lint/issues/42>`_ from ament/remove_second_extension
  remove result type extension from testsuite name
* remove result type extension from testsuite name
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
* apply normpath to prevent './' prefix (fix `#24 <https://github.com/ament/ament_lint/issues/24>`_)
* improve error messages
* Merge pull request `#19 <https://github.com/ament/ament_lint/issues/19>`_ from ament/split_linter_packages_in_python_and_cmake
  split linter packages in python and cmake
* move cmake part of ament_pyflakes to ament_cmake_pyflakes
* move cmake part of ament_pep8 to ament_cmake_pep8
* move cmake part of ament_lint_cmake to ament_cmake_lint_cmake
* disable debug output
* add trailing newline to generated test result files
* add missing copyright / license information
* Merge pull request `#14 <https://github.com/ament/ament_lint/issues/14>`_ from ament/test_runner_windows
  change test runner to work on windows
* change test runner to work on windows
* Merge pull request `#9 <https://github.com/ament/ament_lint/issues/9>`_ from ament/docs
  add docs for linters
* add docs for linters
* modify generated unit test files for a better hierarchy in Jenkins
* improve information in clang_format test result files
* fix copy-n-pasted license names
* make testname argument optional for all linters
* use other linters for the linter packages where possible
* Merge pull request `#2 <https://github.com/ament/ament_lint/issues/2>`_ from ament/ament_lint_auto
  allow linting based on test dependencies only
* add ament_lint_auto and ament_lint_common, update all linter packages to implement extension point of ament_lint_auto
* improve clang-format output, convert absolute offset to line number and offset-in-line, also show line before and after modification
* use project(.. NONE)
* update to latest refactoring of ament_cmake
* add dependency on ament_cmake_environment
* add ament_clang_format
* Contributors: Dirk Thomas
