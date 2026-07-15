^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_pep257
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

0.12.9 (2023-11-13)
-------------------

0.12.8 (2023-09-19)
-------------------
* Convert linenumber to string when printing errors (`#443 <https://github.com/ament/ament_lint/issues/443>`_) (`#446 <https://github.com/ament/ament_lint/issues/446>`_)
* Contributors: mergify[bot]

0.12.7 (2023-07-17)
-------------------

0.12.6 (2023-04-25)
-------------------

0.12.5 (2023-01-12)
-------------------
* Added undescore to ignore new pydocstyle item (backport `#428 <https://github.com/ament/ament_lint/issues/428>`_) (`#429 <https://github.com/ament/ament_lint/issues/429>`_)
* Contributors: mergify[bot]

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
* Remove use of distutils.version.LooseVersion. (`#346 <https://github.com/ament/ament_lint/issues/346>`_)
* Contributors: Chris Lalancette

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
* Update maintainer (`#274 <https://github.com/ament/ament_lint/issues/274>`_)
  * update maintainer
  * add authors
* Contributors: Claire Wang

0.10.0 (2020-09-18)
-------------------
* remove use of "extend" action in argparse (`#262 <https://github.com/ament/ament_lint/issues/262>`_)
* Expand ignores to pep257 definition. (`#241 <https://github.com/ament/ament_lint/issues/241>`_)
  * Expand ignores to pep257 definition. (ament `#240 <https://github.com/ament/ament_lint/issues/240>`_)
  * add '--allow-undocumented' flag to enforce pep257
  * restore existing default error codes to check
  * fix no-ignores logic
  * expose options from pydocstyle
  * allow user to explicitly set convention to "ament"
  * fix typo in populating argv for pydocstyle
  * reformat ament convention list
  * Add help info for ament convention
* Add pytest.ini so local tests don't display warning. (`#259 <https://github.com/ament/ament_lint/issues/259>`_)
* remove match args to allow pydocstyle defaults (`#243 <https://github.com/ament/ament_lint/issues/243>`_)
* Contributors: Chris Lalancette, Ted Kern

0.9.4 (2020-05-26)
------------------

0.9.3 (2020-05-18)
------------------

0.9.2 (2020-05-08)
------------------

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
* Ability to exclude folders with ament_pep257 (`#176 <https://github.com/ament/ament_lint/issues/176>`_)
* Contributors: Dirk Thomas, Dmitriy Vornychev

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

0.7.1 (2019-05-07)
------------------
* update phrase of status messages (`#137 <https://github.com/ament/ament_lint/issues/137>`_)
* Contributors: Dirk Thomas

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
* Fix lint warnings from invalid escape sequences (`#111 <https://github.com/ament/ament_lint/issues/111>`_)
  Use raw strings for regex patterns to avoid warnings.
* Contributors: Jacob Perron

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
* Add new D106, D107 linter errors to ignored list (`#86 <https://github.com/ament/ament_lint/issues/86>`_)
  * Add new D107 linter error to ignored list
  * Ignore D106 also
* 0.0.3
* Merge pull request `#84 <https://github.com/ament/ament_lint/issues/84>`_ from ament/flake8_plugins
  update style to satisfy new flake8 plugins
* update style to satisfy new flake8 plugins
* 0.0.2
* Merge pull request `#78 <https://github.com/ament/ament_lint/issues/78>`_ from ament/use_flake8
  use flake8 instead of pep8 and pyflakes
* fix style warnings
* use flake8 instead of pep8 and pyflakes
* Fix pydocstyle 2.0.0 (`#71 <https://github.com/ament/ament_lint/issues/71>`_)
  * works with 2.0.0
  * works with 1.1.1 and 2.0.0
* remove __future_\_ imports
* Merge pull request `#61 <https://github.com/ament/ament_lint/issues/61>`_ from ament/multistring_format
  (dev) enforcing multiline docstring format
* ignore D404
* (dev) enforcing multiline docstring format
  by ignoring D212 we implicitely enforce D213 as the multistring comment
  format
* Merge pull request `#58 <https://github.com/ament/ament_lint/issues/58>`_ from Karsten1987/master
  (fix) correct pydocstyle import for version 1.1.0
* (fix) correct pydocstyle import for version 1.1.0
* update schema url
* add schema to manifest files
* fix pydocstyle paths (`#57 <https://github.com/ament/ament_lint/issues/57>`_)
* Merge pull request `#49 <https://github.com/ament/ament_lint/issues/49>`_ from ament/xenial
  use upstream pydocstyle
* use new pydocstyle
* Merge pull request `#47 <https://github.com/ament/ament_lint/issues/47>`_ from ament/wjwwood-patch-1
  ignore pep257 error D203 by default
* ignore pep257 error D203 by default
* Merge pull request `#42 <https://github.com/ament/ament_lint/issues/42>`_ from ament/remove_second_extension
  remove result type extension from testsuite name
* remove result type extension from testsuite name
* Merge pull request `#28 <https://github.com/ament/ament_lint/issues/28>`_ from ament/pep257
  add packages to check pep257 compliance
* add ament_pep257 package
* Contributors: Dirk Thomas, Karsten Knese, William Woodall, dhood
