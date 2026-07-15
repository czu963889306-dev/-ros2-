^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cpplint
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.12.15 (2026-02-13)
--------------------

0.12.14 (2025-07-18)
--------------------

0.12.13 (2025-07-16)
--------------------
* cpplint: update link to upstream cpplint repo (`#538 <https://github.com/ament/ament_lint/issues/538>`_) (`#543 <https://github.com/ament/ament_lint/issues/543>`_)
* Contributors: mergify[bot]

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
* Consider files with '.hh' extension as C++ headers (`#374 <https://github.com/ament/ament_lint/issues/374>`_) (`#381 <https://github.com/ament/ament_lint/issues/381>`_)
* Contributors: Jacob Perron

0.12.3 (2022-04-08)
-------------------

0.12.2 (2022-03-28)
-------------------
* ignore NOLINT comments with categories that come from clang-tidy (`#339 <https://github.com/ament/ament_lint/issues/339>`_)
* Contributors: William Woodall

0.12.1 (2022-03-01)
-------------------

0.12.0 (2022-02-18)
-------------------

0.11.4 (2022-01-14)
-------------------
* Update forthcoming version in changelogs
* Reapply patches
  Reapply parts of 232428752251de61e84ef013bcd643e35eb9038d that are still relevant.
* Update cpplint version
  Point to the fork https://github.com/cpplint/cpplint
  Contains updates for modern C++ standards (e.g. C++14 and C++17).
* Contributors: Audrow Nash, Dirk Thomas, Jacob Perron

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
* Convert list comprehension to generator (`#179 <https://github.com/ament/ament_lint/issues/179>`_)
  Addresses flake8 C412 errors introduced by flake8-comprehension 2.2.0
* Contributors: Dirk Thomas, Scott K Logan

0.7.4 (2019-07-31)
------------------
* Escape backslashes in cpplint.py
  This gets rid of DeprecationWarning on Windows.
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
* fix sorting of keys in same cases (`#127 <https://github.com/ament/ament_lint/issues/127>`_)
* Contributors: Dirk Thomas

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
* fix root logic of cpplint (`#94 <https://github.com/ament/ament_lint/issues/94>`_)
* Contributors: Dirk Thomas

0.4.0 (2017-12-08)
------------------
* remove test_suite, add pytest as test_requires
* add filters argument to cpplint (`#87 <https://github.com/ament/ament_lint/issues/87>`_)
  * add filters argument to cpplint
  * full ellipsis
  * string -> strings
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
* cpplint: allow using-directive for a whitelist of namespaces (`#67 <https://github.com/ament/ament_lint/issues/67>`_)
  This will permit the use of std::chrono and other useful new literals in C++14, which are most conveniently brought in via "using namespace"
* remove __future_\_ imports
* Merge pull request `#59 <https://github.com/ament/ament_lint/issues/59>`_ from ament/update_cpplint
  update cpplint
* reapply patches
* use new --headers option
* pull new version from upstream
* update schema url
* add schema to manifest files
* Merge pull request `#54 <https://github.com/ament/ament_lint/issues/54>`_ from ament/update_cpplint
  update cpplint
* find deepest vcs path instead of most top level
* update to latest upstream version of cpplint
* Merge pull request `#52 <https://github.com/ament/ament_lint/issues/52>`_ from ament/lint_generated_code
  fix handling of --root
* fix handling of --root
* Merge pull request `#46 <https://github.com/ament/ament_lint/issues/46>`_ from ament/fix_cpplint_root
  fix algo to determine --root for cpplint
* fix algo to determine --root for cpplint
* Merge pull request `#43 <https://github.com/ament/ament_lint/issues/43>`_ from ament/ignore_space_after_semicolon
  ignore space after a semi colon
* also advice cpplint to ignore whitespace after semicolon
* Merge pull request `#42 <https://github.com/ament/ament_lint/issues/42>`_ from ament/remove_second_extension
  remove result type extension from testsuite name
* remove result type extension from testsuite name
* Merge pull request `#40 <https://github.com/ament/ament_lint/issues/40>`_ from ament/cpplint_root
  add --root option to ament_cpplint
* add --root option to ament_cpplint
* Merge pull request `#39 <https://github.com/ament/ament_lint/issues/39>`_ from ament/cpplint_python3
  use Python 3 for cpplint and split into Python and CMake package
* fix matching root on Windows
* refactor ament_cpplint into Python and CMake package
* modify cpplint to work with Python 3
* Merge pull request `#37 <https://github.com/ament/ament_lint/issues/37>`_ from ament/patches
  enforce single line comments for closing namespaces
* fix infinite loop
* only allow single line comments for closing namespaces
* Merge pull request `#35 <https://github.com/ament/ament_lint/issues/35>`_ from ament/cpplint-int
  Reenable int/long check
* Reenable int/long check
* Merge pull request `#33 <https://github.com/ament/ament_lint/issues/33>`_ from ament/disable_cpplint_runtime_int
  ignore cpplint runtime/int error
* ignore cpplint runtime/int error
* Merge pull request `#32 <https://github.com/ament/ament_lint/issues/32>`_ from ament/cpplint_c_style_casts
  allow C-style casts in c code
* allow C-style casts in c code
* workaround to check guard variable for all header files
* Merge pull request `#31 <https://github.com/ament/ament_lint/issues/31>`_ from ament/config_cpplint
  update cpplint configuration
* fix --root for files which are directly in the include/src/test folder
* update cpplint options and implement custom include guard pattern
* update url for cpplint
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
* make testname argument optional for all linters
* use other linters for the linter packages where possible
* update cpplint to rev 141
* Merge pull request `#2 <https://github.com/ament/ament_lint/issues/2>`_ from ament/ament_lint_auto
  allow linting based on test dependencies only
* add ament_lint_auto and ament_lint_common, update all linter packages to implement extension point of ament_lint_auto
* use project(.. NONE)
* update to latest refactoring of ament_cmake
* add dependency on ament_cmake_environment
* add ament_pyflakes
* add ament_lint_cmake
* add ament_cpplint
* Contributors: Dirk Thomas, Esteve Fernandez, Guillaume Papin, William Woodall
