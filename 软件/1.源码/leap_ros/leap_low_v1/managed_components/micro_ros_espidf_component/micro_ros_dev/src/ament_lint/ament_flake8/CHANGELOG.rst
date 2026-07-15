^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_flake8
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* Changes to make ament_flake8 work with v6+ (`#459 <https://github.com/ament/ament_lint/issues/459>`_) (`#464 <https://github.com/ament/ament_lint/issues/464>`_)
* Contributors: mergify[bot]

0.12.9 (2023-11-13)
-------------------
* [backport humble] Fix compatibility with flake8 version 5 (`#387 <https://github.com/ament/ament_lint/issues/387>`_, `#410 <https://github.com/ament/ament_lint/issues/410>`_) (`#451 <https://github.com/ament/ament_lint/issues/451>`_)
* Contributors: Emerson Knapp

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
* Remove use of distutils.version.LooseVersion. (`#346 <https://github.com/ament/ament_lint/issues/346>`_)
* Contributors: Chris Lalancette

0.11.4 (2022-01-14)
-------------------
* Update forthcoming version in changelogs
* Contributors: Audrow Nash

0.11.3 (2022-01-14)
-------------------
* Ignore .*/_* dirs in ament_flake8 (`#335 <https://github.com/ament/ament_lint/issues/335>`_)
  Other ament\_* linters specifically ignore directories starting with a
  dot or underscore when crawling for files to lint. They also do so
  implicitly, so this change mimics that same pattern so that the behavior
  is consistent.
* Update maintainers to Michael Jeronimo and Michel Hidalgo (`#340 <https://github.com/ament/ament_lint/issues/340>`_)
* Contributors: Audrow Nash, Scott K Logan

0.11.2 (2021-06-18)
-------------------

0.11.1 (2021-06-18)
-------------------

0.11.0 (2021-06-18)
-------------------
* Ignore flake8-blind-except B902 (`#292 <https://github.com/ament/ament_lint/issues/292>`_)
* Contributors: Scott K Logan

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
* Contributors: Audrow Nash

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
* support flake8 3.8 (`#242 <https://github.com/ament/ament_lint/issues/242>`_)
* Contributors: Dirk Thomas

0.9.2 (2020-05-08)
------------------
* Allow AMENT_IGNORE markers to be directories (`#232 <https://github.com/ament/ament_lint/issues/232>`_)
* Contributors: Dan Rose

0.9.1 (2020-04-10)
------------------
* 0.9.0
* more verbose test_flake8 error messages (same as `ros2/launch_ros#135 <https://github.com/ros2/launch_ros/issues/135>`_)
* add new main_with_errors() API that also returns the error strings if any (`#221 <https://github.com/ament/ament_lint/issues/221>`_)
  * return custom int subclass to include the error strings
  * expose errors through separate function rather than magic return code object
* Generate xunit files valid for the junit10.xsd (`#220 <https://github.com/ament/ament_lint/issues/220>`_)
* Exclude folders having AMENT_IGNORE in ament_flake8 script (`#211 <https://github.com/ament/ament_lint/issues/211>`_)
* remove status attribute from result XML (`#212 <https://github.com/ament/ament_lint/issues/212>`_)
* Contributors: Chris Lalancette, Dirk Thomas, Hao Peng, Jose Luis Rivero

0.8.1 (2019-10-23)
------------------
* install resource marker file for packages (`#193 <https://github.com/ament/ament_lint/issues/193>`_)
* Contributors: Dirk Thomas

0.8.0 (2019-09-17)
------------------
* Install manifest file in ament_flake8 (`#189 <https://github.com/ament/ament_lint/issues/189>`_)
  Follow-up to `#185 <https://github.com/ament/ament_lint/issues/185>`_
* re-add path insertion removed from wrong package (`#178 <https://github.com/ament/ament_lint/issues/178>`_)
* add mypy support for linters/testing (`#154 <https://github.com/ament/ament_lint/issues/154>`_)
  * add mypy support for linters/testing
  * Update ament_cmake_mypy/doc/index.rst
  Co-Authored-By: Kyle Fazzari <github@status.e4ward.com>
  * fix whitespace in file and in generated xml
  * fixes, package versioning, and test suite
  * fix wrong separator in cmake file
  * readd copied from comment
  * Update ament_mypy/ament_mypy/main.py
  Co-Authored-By: Kyle Fazzari <github@status.e4ward.com>
  * remove mypypath auto populating
  * add default configuration ignoring missing imports
  * update test to remove MYPYPATH check, default config check
  * remove extraneous path insert statement
  * remove extraneous path insert statement
  * update test cases for default config file change
  * added tests for error code forwarding, fixed linter errors
  * linter failures relating to quotes and docs
  * add handling for notes
  * remove ament_lint dep
  * update regex to match drive letter on windows
* Contributors: Scott K Logan, Ted Kern

0.7.4 (2019-07-31)
------------------
* declare pytest markers (`#164 <https://github.com/ament/ament_lint/issues/164>`_)
  * declare pytest markers
  * add markers to ament_xmllint tests
* Contributors: Dirk Thomas

0.7.3 (2019-05-09 14:08)
------------------------

0.7.2 (2019-05-09 09:30)
------------------------
* readd listener if available (`#141 <https://github.com/ament/ament_lint/issues/141>`_)
* Contributors: Dirk Thomas

0.7.1 (2019-05-07)
------------------
* update phrase of status messages (`#137 <https://github.com/ament/ament_lint/issues/137>`_)
* Contributors: Dirk Thomas

0.7.0 (2019-04-11)
------------------
* Remove make_notifier() call (`#124 <https://github.com/ament/ament_lint/issues/124>`_)
* use --extend-ignore for flake8 to keep default ignores (`#122 <https://github.com/ament/ament_lint/issues/122>`_)
* Contributors: Dirk Thomas, Shane Loretz

0.6.3 (2019-01-14)
------------------

0.6.2 (2018-12-06)
------------------
* Change spelling of maintainer name. (`#115 <https://github.com/ament/ament_lint/issues/115>`_)
  This is causing failures in CI at the moment. Until I figure out where
  the issue lies let's just mispell it to unbreak CI.
* Contributors: Steven! Ragnarök

0.6.1 (2018-11-15)
------------------
* Update maintainer for ament{,_cmake}_flake8. (`#114 <https://github.com/ament/ament_lint/issues/114>`_)
* Contributors: Steven! Ragnarök

0.6.0 (2018-11-14)
------------------
* Ignore flake8 W504 (`#110 <https://github.com/ament/ament_lint/issues/110>`_)
* Contributors: Jacob Perron

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
* Add I202 to the list of ignored flake8 errors. (`#89 <https://github.com/ament/ament_lint/issues/89>`_)
  I202 warns about newlines between groups of imports in python.
  A recent change in flake8
  (https://github.com/PyCQA/flake8-import-order/commit/37dafcc35eec9343641d489ac01d316cd10a6c03)
  made this start showing up in ROS2.  Since we use whitespace
  between imports in lots of places in ROS2, disable this
  warning, which should get rid of this error almost everywhere.
* Adapt to flake8 v3.5.0 changes (`#88 <https://github.com/ament/ament_lint/issues/88>`_)
  * Adapt to flake8 v3.5.0 changes
  * Add comment why this fork of get_style_guide exists
* Add new D106, D107 linter errors to ignored list (`#86 <https://github.com/ament/ament_lint/issues/86>`_)
  * Add new D107 linter error to ignored list
  * Ignore D106 also
* 0.0.3
* Merge pull request `#84 <https://github.com/ament/ament_lint/issues/84>`_ from ament/flake8_plugins
  update style to satisfy new flake8 plugins
* update style to satisfy new flake8 plugins
* implicitly inherit from object (only in files not copied from somewhe… (`#83 <https://github.com/ament/ament_lint/issues/83>`_)
  * implicitly inherit from object (only in files not copied from somewhere else)
  * don't modify file copied from elsewhere
* 0.0.2
* Merge pull request `#77 <https://github.com/ament/ament_lint/issues/77>`_ from ament/fix_flake8_excludes
  fix --exclude with ament_flake8
* fix --exclude with ament_flake8
* Make get_error_type_counts work for legacy api too (`#70 <https://github.com/ament/ament_lint/issues/70>`_)
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
* Contributors: Chris Lalancette, Dirk Thomas, Mikael Arguedas, dhood
