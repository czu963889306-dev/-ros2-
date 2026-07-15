^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_clang_tidy
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
* ament_clang_tidy - Fix Reporting when WarningsAsErrors is specified in config (`#397 <https://github.com/ament/ament_lint/issues/397>`_) (`#488 <https://github.com/ament/ament_lint/issues/488>`_)
* Contributors: mergify[bot]

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
* remove google style from clang-tidy default settings, removing need for default config file (`#337 <https://github.com/ament/ament_lint/issues/337>`_)
* Improvements to ament_lint_clang_tidy. (`#316 <https://github.com/ament/ament_lint/issues/316>`_)
* Contributors: Audrow Nash, Steven! Ragnarök, William Woodall

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
* Add multiprocessing support to ament_clang_tidy (`#288 <https://github.com/ament/ament_lint/issues/288>`_)
  * add multiprocessing support
  * fix stylistic lint issues
* Contributors: Audrow Nash, M. Mei

0.10.1 (2021-01-25)
-------------------
* Add --packages-select argument to ament_clang_tidy (`#287 <https://github.com/ament/ament_lint/issues/287>`_)
  Add comment explaining handling quoted list of space separated package names
  Update documentation for ament_clang_tidy
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

0.9.2 (2020-05-08)
------------------
* Allow AMENT_IGNORE markers to be directories (`#232 <https://github.com/ament/ament_lint/issues/232>`_)
* Contributors: Dan Rose

0.9.1 (2020-04-10)
------------------
* 0.9.0
* Merge pull request `#213 <https://github.com/ament/ament_lint/issues/213>`_ from mm318/master
  Fix ament_clang_tidy
* fix KeyError crash in ament_clang_tidy script
* put back "checked files" section of xunit output file
* fix bug in constructing arguments for clang-tidy command
* more verbose test_flake8 error messages (same as `ros2/launch_ros#135 <https://github.com/ros2/launch_ros/issues/135>`_)
* fix flake8 linter issue
* reduce amount of changes in pull request again
* reduce amount of changes in pull request
* put back --fix-errors and --header-filter options (--fix-errors is still a no-op)
* fix python lint issues
* remove some unsupported features
* fix ament_clang_tidy
* Generate xunit files valid for the junit10.xsd (`#220 <https://github.com/ament/ament_lint/issues/220>`_)
* remove status attribute from result XML (`#212 <https://github.com/ament/ament_lint/issues/212>`_)
* Contributors: Chris Lalancette, Claire Wang, Dirk Thomas, Jose Luis Rivero, Miaofei

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
* Fix version num for release (`#169 <https://github.com/ament/ament_lint/issues/169>`_)
* Add python3-yaml depends to ament_clang_tidy/format (`#166 <https://github.com/ament/ament_lint/issues/166>`_)
  depend -> exec_depend
  Remove whitespace
* Add error output, header out option, fix null error (`#163 <https://github.com/ament/ament_lint/issues/163>`_)
  * Add error output, header out option, fix null error
  * Add quiet option to `ament_clang_tidy`
  * Add header filter and system header option
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

0.5.2 (2018-06-27)
------------------

0.5.1 (2018-06-18 13:47)
------------------------

0.5.0 (2018-06-18 10:09)
------------------------

0.4.0 (2017-12-08)
------------------
