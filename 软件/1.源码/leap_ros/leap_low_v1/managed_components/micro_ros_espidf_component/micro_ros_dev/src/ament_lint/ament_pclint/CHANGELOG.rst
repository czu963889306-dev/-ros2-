^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_pclint
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.12.15 (2026-02-13)
--------------------
* fix setuptools deprecation (backport `#551 <https://github.com/ament/ament_lint/issues/551>`_) (`#560 <https://github.com/ament/ament_lint/issues/560>`_)
* Contributors: mergify[bot]

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
* Improve documentation by clarifying the purpose of different tools (`#357 <https://github.com/ament/ament_lint/issues/357>`_)
* Contributors: Bi0T1N

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
* Contributors: Audrow Nash

0.10.2 (2021-02-12)
-------------------
* Add Audrow as a maintainer (`#294 <https://github.com/ament/ament_lint/issues/294>`_)
* Add pytest marks to ament_pclint tests. (`#202 <https://github.com/ament/ament_lint/issues/202>`_)
  * Add pytest marks to ament_pclint tests.
  * fix failed tests
  Co-authored-by: Miaofei <miaofei@amazon.com>
* Drop trailing tab from package manifests (`#291 <https://github.com/ament/ament_lint/issues/291>`_)
  Follow-up to 8bf194aa1ac282db5483dd0d3fefff8f325b0db8
* Contributors: Audrow Nash, Scott K Logan, Steven! Ragnarök

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
* Fix another occurrence of flake8 f999. (`#123 <https://github.com/ament/ament_lint/issues/123>`_)
* Contributors: Chris Lalancette

0.6.3 (2019-01-14)
------------------

0.6.2 (2018-12-06)
------------------

0.6.1 (2018-11-15)
------------------

0.6.0 (2018-11-14)
------------------
* Improvements to ament_pclint configuration (`#112 <https://github.com/ament/ament_lint/issues/112>`_)
  * Improvements to ament_pclint configuration
  * Improvement to ament_cmake_pclint
* Contributors: jpsamper2009

0.5.2 (2018-06-27)
------------------
* Add ament_pclint and ament_cmake_pclint packages (`#101 <https://github.com/ament/ament_lint/issues/101>`_)
  * Add ament_pclint and ament_cmake_pclint packages
  * Skip pclint creation if pclint executable not found
  * Update license to Apache 2.0
  - Use config file with permissive license:
  http://www.gimpel.com/html/pub90/au-misra3.lnt
  * Fixing copyright and formatting issues.
  * Add test dependencies
  * Fix remaining copyright/license notices.
  * Version Bump to 0.5.1 for pclint packages.
* Contributors: Michael Carroll

0.5.1 (2018-06-18 13:47)
------------------------

0.5.0 (2018-06-18 10:09)
------------------------
* Revert "Add ament package for pclint" (`#100 <https://github.com/ament/ament_lint/issues/100>`_)
* Merge pull request `#98 <https://github.com/ament/ament_lint/issues/98>`_ from jpsamper2009/ament_pclint
  Add ament package for pclint
* Update license to Apache 2.0
  - Use config file with permissive license:
  http://www.gimpel.com/html/pub90/au-misra3.lnt
* Add ament_pclint and ament_cmake_pclint packages
* Contributors: Juan Pablo Samper, Michael Carroll

0.4.0 (2017-12-08)
------------------
