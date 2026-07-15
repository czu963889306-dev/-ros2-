^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cmake_flake8
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
* Add custom config file support for flake8 (`#331 <https://github.com/ament/ament_lint/issues/331>`_)
* Contributors: Audrow Nash, Kenji Miyake

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
* Contributors: Audrow Nash

0.10.1 (2021-01-25)
-------------------
* Update maintainer (`#274 <https://github.com/ament/ament_lint/issues/274>`_)
  * update maintainer
  * add authors
* Contributors: Claire Wang

0.10.0 (2020-09-18)
-------------------

0.9.4 (2020-05-26)
------------------

0.9.3 (2020-05-18)
------------------

0.9.2 (2020-05-08)
------------------

0.9.1 (2020-04-10)
------------------
* 0.9.0
* Contributors: Chris Lalancette

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
* Contributors: Dirk Thomas, dhood
