^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_uncrustify
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
* [ament_uncrustify] Fix file exclusion behavior (`#334 <https://github.com/ament/ament_lint/issues/334>`_)
  * [ament_uncrustify] Fix file exclusion behavior
  This PR fixes the file exclusion behavior reported in `#326 <https://github.com/ament/ament_lint/issues/326>`_.
  Specifically, the exclusion list is matched against
  files/directories as the search path is traversed.
  Tries to maintain consistency with `#327 <https://github.com/ament/ament_lint/issues/327>`_.
  * [ament_uncrustify] Add file exclusion tests
  * [ament_uncrustify] Remove erroneous pytest marker
* Update maintainers to Michael Jeronimo and Michel Hidalgo (`#340 <https://github.com/ament/ament_lint/issues/340>`_)
* [ament_uncrustify] Add ament_lint tests (`#338 <https://github.com/ament/ament_lint/issues/338>`_)
  * Add `ament_lint` tests on `ament_uncrustify`
  * Address linter warnings in `ament_uncrustify`
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
* Allow 'C++' as language, but convert it to 'CPP' (`#302 <https://github.com/ament/ament_lint/issues/302>`_)
* Allow correct languages on uncrustify (`#272 <https://github.com/ament/ament_lint/issues/272>`_)
  * Allow correct languages on uncrustify.
  * Update dictionary.
* Contributors: Audrow Nash, Miguel Company

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

0.9.2 (2020-05-08)
------------------
* Allow AMENT_IGNORE markers to be directories (`#232 <https://github.com/ament/ament_lint/issues/232>`_)
* Contributors: Dan Rose

0.9.1 (2020-04-10)
------------------
* 0.9.0
* fix TypeError in ament_uncrustify (`#228 <https://github.com/ament/ament_lint/issues/228>`_)
* Generate xunit files valid for the junit10.xsd (`#220 <https://github.com/ament/ament_lint/issues/220>`_)
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
* remove status attribute from result XML (`#212 <https://github.com/ament/ament_lint/issues/212>`_)
* enable nl_func_call_start_multi_line in uncrustify (`#210 <https://github.com/ament/ament_lint/issues/210>`_)
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

0.7.3 (2019-05-09 14:08)
------------------------

0.7.2 (2019-05-09 09:30)
------------------------

0.7.1 (2019-05-07)
------------------
* use explicit encoding when reading files (`#138 <https://github.com/ament/ament_lint/issues/138>`_)
* update phrase of status messages (`#137 <https://github.com/ament/ament_lint/issues/137>`_)
* Contributors: Dirk Thomas

0.7.0 (2019-04-11)
------------------

0.6.3 (2019-01-14)
------------------
* [ament_uncrustify] Update uncrustiy configuration
  Added new options with defaults.
* Contributors: Jacob Perron

0.6.2 (2018-12-06)
------------------

0.6.1 (2018-11-15)
------------------

0.6.0 (2018-11-14)
------------------
* update configuration to 0.67 (`#103 <https://github.com/ament/ament_lint/issues/103>`_)
  * update configuration to 0.67
  * force sp_fparen_brace_initializer as it overrides sp_fparen_brace
* Contributors: Mikael Arguedas

0.5.2 (2018-06-27)
------------------
* use uncrustify_vendor as a dependency (`#102 <https://github.com/ament/ament_lint/issues/102>`_)
* Contributors: Mikael Arguedas

0.5.1 (2018-06-18 13:47)
------------------------
* level setup.py versions to 0.5.1
* Contributors: Mikael Arguedas

0.5.0 (2018-06-18 10:09)
------------------------
* set zip_safe to avoid warning during installation (`#96 <https://github.com/ament/ament_lint/issues/96>`_)
* Merge pull request `#95 <https://github.com/ament/ament_lint/issues/95>`_ from ament/uncrustify_0.66.1
  update uncrustify config to version 0.66.1
* work around overmatching of nl_fcall_brace, since we don't use it just ignore it (which is the default anyway)
* update uncrustify config to version 0.66.1
* Contributors: Dirk Thomas

0.4.0 (2017-12-08)
------------------
* remove test_suite, add pytest as test_requires
* Merge pull request `#85 <https://github.com/ament/ament_lint/issues/85>`_ from ament/uncrustify_master
  update uncrustify config
* shuffle order of option groups as in new uncrustify version
* update uncrustify config
* 0.0.3
* Merge pull request `#84 <https://github.com/ament/ament_lint/issues/84>`_ from ament/flake8_plugins
  update style to satisfy new flake8 plugins
* update style to satisfy new flake8 plugins
* Merge pull request `#81 <https://github.com/ament/ament_lint/issues/81>`_ from ament/ignore_build_spaces
  ignore folders with an AMENT_IGNORE file, e.g. build spaces
* ignore folders with an AMENT_IGNORE file, e.g. build spaces
* 0.0.2
* remove __future_\_ imports
* update schema url
* add schema to manifest files
* fix undefined variable in case of exceptions
* Merge pull request `#52 <https://github.com/ament/ament_lint/issues/52>`_ from ament/lint_generated_code
  fix handling of --root
* fix custom line length for uncrustify on Windows
* Merge pull request `#51 <https://github.com/ament/ament_lint/issues/51>`_ from ament/lint_generated_code
  extend linter API to allow overriding the max line length
* add CMake argument to override max line length for linters as well as the root for cpplint
* Merge pull request `#44 <https://github.com/ament/ament_lint/issues/44>`_ from ament/uncrustify_0.62
  update config to uncrustify 0.62
* update config to uncrustify 0.62
* Merge pull request `#43 <https://github.com/ament/ament_lint/issues/43>`_ from ament/ignore_space_after_semicolon
  ignore space after a semi colon
* ignore space after a semi colon
* Merge pull request `#42 <https://github.com/ament/ament_lint/issues/42>`_ from ament/remove_second_extension
  remove result type extension from testsuite name
* remove result type extension from testsuite name
* Merge pull request `#25 <https://github.com/ament/ament_lint/issues/25>`_ from ament/test_repeated_publisher_subscriber
  disable multi-line comment formatting since the result is just not good
* disable multi-line comment formatting since the result is just not good
* update style rule for preprocessor stringify operator
* apply normpath to prevent './' prefix (fix `#24 <https://github.com/ament/ament_lint/issues/24>`_)
* Merge pull request `#23 <https://github.com/ament/ament_lint/issues/23>`_ from ament/uncrustify_exclude_option
  add --exclude option to ament_uncrustify
* add --exclude option to ament_uncrustify
* treat enum like structs for newlines before {, allow newline in empty block
* fix overwriting later used list, abort on non-deterministic values
* update uncrustify config
* fix location of uncrustify files on Windows
* improve debugging of uncrustify errors
* Merge pull request `#19 <https://github.com/ament/ament_lint/issues/19>`_ from ament/split_linter_packages_in_python_and_cmake
  split linter packages in python and cmake
* move cmake part of ament_uncrustify to ament_cmake_uncrustify
* move cmake part of ament_pyflakes to ament_cmake_pyflakes
* move cmake part of ament_pep8 to ament_cmake_pep8
* move cmake part of ament_lint_cmake to ament_cmake_lint_cmake
* pass relative paths to uncrustify on Windows
* fix finding uncrustify on windows
* disable debug output
* update uncrustify config (related to `#18 <https://github.com/ament/ament_lint/issues/18>`_)
* update uncrustify config (related to `#18 <https://github.com/ament/ament_lint/issues/18>`_)
* update uncrustify config (related to `#18 <https://github.com/ament/ament_lint/issues/18>`_)
* update uncrustify config to version 0.61 (related to `#18 <https://github.com/ament/ament_lint/issues/18>`_)
* update uncrustify config (related to `#18 <https://github.com/ament/ament_lint/issues/18>`_)
* update uncrustify config (related to `#18 <https://github.com/ament/ament_lint/issues/18>`_)
* Merge pull request `#18 <https://github.com/ament/ament_lint/issues/18>`_ from ament/uncrustify_config
  update uncrustify config
* update uncrustify config
* fix uncrustify unit test results
* update uncrustify config: treat structs like classes instead of ifs
* add trailing newline to generated test result files
* add missing copyright / license information
* Merge pull request `#14 <https://github.com/ament/ament_lint/issues/14>`_ from ament/test_runner_windows
  change test runner to work on windows
* change test runner to work on windows
* set code_width in uncrustify to 100 characters
* update uncrustify configuration to enforce newlines before curly braces for namespaces, templates, classes and functions
* add configuration file for ament_pep8, set max-line-length to 99, don't use default ignores
* Merge pull request `#9 <https://github.com/ament/ament_lint/issues/9>`_ from ament/docs
  add docs for linters
* add docs for linters
* modify generated unit test files for a better hierarchy in Jenkins
* fix copy-n-pasted license names
* make testname argument optional for all linters
* use other linters for the linter packages where possible
* code style only
* Merge pull request `#2 <https://github.com/ament/ament_lint/issues/2>`_ from ament/ament_lint_auto
  allow linting based on test dependencies only
* add ament_lint_auto and ament_lint_common, update all linter packages to implement extension point of ament_lint_auto
* use project(.. NONE)
* update to latest refactoring of ament_cmake
* add dependency on ament_cmake_environment
* Merge pull request `#1 <https://github.com/ament/ament_lint/issues/1>`_ from ament/uncrustify_google
  update uncrustify config to reflect google code style
* add ament_clang_format
* update uncrustify config to reflect google code style
* add --reformat option to ament_uncrustify, run uncrustify multiple times if necessary
* add ament_uncrustify
* Contributors: Dirk Thomas, William Woodall
