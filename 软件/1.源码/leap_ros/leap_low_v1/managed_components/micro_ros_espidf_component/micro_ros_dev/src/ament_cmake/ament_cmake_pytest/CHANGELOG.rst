^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cmake_pytest
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.3.14 (2026-02-23)
-------------------

1.3.13 (2026-01-13)
-------------------

1.3.12 (2025-06-27)
-------------------

1.3.11 (2024-11-25)
-------------------

1.3.10 (2024-07-26)
-------------------

1.3.9 (2024-05-15)
------------------

1.3.8 (2024-02-16)
------------------

1.3.7 (2024-01-24)
------------------

1.3.6 (2023-11-13)
------------------
* backport merge `#381 <https://github.com/ament/ament_cmake/issues/381>`_ to humble (`#490 <https://github.com/ament/ament_cmake/issues/490>`_)
* Contributors: El Jawad Alaa

1.3.5 (2023-06-22)
------------------
* Fix pytest-cov version detection with pytest >=7.0.0 (`#459 <https://github.com/ament/ament_cmake/issues/459>`_)
* Contributors: Christophe Bedard

1.3.4 (2023-04-25)
------------------

1.3.3 (2022-11-07)
------------------

1.3.2 (2022-05-17)
------------------

1.3.1 (2022-03-28)
------------------

1.3.0 (2022-02-17)
------------------
* Update forthcoming version in changelog
* Contributors: Audrow Nash

1.2.1 (2022-01-14)
------------------
* Update maintainers to Michael Jeronimo and Michel Hidalgo (`#362 <https://github.com/ament/ament_cmake/issues/362>`_)
* Fix misleading comment (`#361 <https://github.com/ament/ament_cmake/issues/361>`_)
* Contributors: Audrow Nash, Tim Clephas

1.2.0 (2021-10-29)
------------------
* Use FindPython3 instead of FindPythonInterp (`#355 <https://github.com/ament/ament_cmake/issues/355>`_)
* Support commands with executable targets (`#352 <https://github.com/ament/ament_cmake/issues/352>`_)
* Mention other platforms in 'pytest/pytest-cov not found' warning (`#337 <https://github.com/ament/ament_cmake/issues/337>`_)
* Update maintainers (`#336 <https://github.com/ament/ament_cmake/issues/336>`_)
* Contributors: Chris Lalancette, Christophe Bedard, Shane Loretz

1.1.4 (2021-05-06)
------------------

1.1.3 (2021-03-09)
------------------

1.1.2 (2021-02-26 22:59)
------------------------

1.1.1 (2021-02-26 19:12)
------------------------

1.1.0 (2021-02-24)
------------------

1.0.4 (2021-01-25)
------------------
* Fix ament_get_pytest_cov_version for newer versions of pytest (`#315 <https://github.com/ament/ament_cmake/issues/315>`_)
* Contributors: Christophe Bedard

1.0.3 (2020-12-10)
------------------

1.0.2 (2020-12-07)
------------------
* Update package maintainers. (`#286 <https://github.com/ament/ament_cmake/issues/286>`_)
* Contributors: Michel Hidalgo

1.0.1 (2020-09-10)
------------------

1.0.0 (2020-07-22)
------------------

0.9.6 (2020-06-23)
------------------

0.9.5 (2020-06-02)
------------------

0.9.4 (2020-05-26)
------------------

0.9.3 (2020-05-19)
------------------

0.9.2 (2020-05-07)
------------------

0.9.1 (2020-04-24 15:45)
------------------------

0.9.0 (2020-04-24 12:25)
------------------------
* Enable coverage information generation for pytest tests with CMake (`#226 <https://github.com/ament/ament_cmake/issues/226>`_)
  * Enable coverage information generation for pytest tests with CMake
  * Add comment about pytest-cov version requirement for --cov-branch
  * Add --pytest-with-coverage to run_test.py and mention the env var
  * Rename to AMENT_CMAKE_TEST_PYTEST_WITH_COVERAGE
  * Fix missing quote
  * Exclude gtests from pytest coverage explicitly
  They were excluded before, but only because gtests didn't use --env or --append-end.
  * Append pytest-cov flags in ament_add_pytest_test() directly
  * Fix ament_has_pytest_cov()
  * Change default logic to avoid overriding CLI params
  * Remove --cov-append pytest_cov option
  * Simplify indentation
  * Remove QUIET arg from ament_has_pytest_cov()
  * Change ament_has_pytest_cov() to ament_get_pytest_cov_version()
  * Do not return() if pytest_cov is not found in ament_add_pytest_test()
  * Fix missing empty <options> argument
  * Simplify pytest_cov version regex match
  * Write pytest_cov results to test-specific directory
  * Make sure to create test-specific pytest_cov directory
* Contributors: Christophe Bedard

0.8.1 (2019-10-23)
------------------

0.8.0 (2019-10-04)
------------------
* Add runner option to ament_add_test (`#174 <https://github.com/ament/ament_cmake/issues/174>`_)
  * ament_cmake allow speficiation of a different test runner
  - By default, still uses run_test.py
  - Example use case: ament_cmake_ros can use a test runner that sets a ROS_DOMAIN_ID
  * ament_cmake move run_test.py to a python module
  - This should let us see the history
  * ament_cmake refactor run_test.py into an importable python module
  - Adds an ament_cmake_test python package
* Add WERROR option to ament_add_pytest_test (`#168 <https://github.com/ament/ament_cmake/issues/168>`_)
  This has the benefit of making deprecation warnings visible, which are not by default.
  Default value for the option is OFF.
* Contributors: Jacob Perron, Peter Baughman

0.7.3 (2019-05-29)
------------------

0.7.2 (2019-05-20)
------------------

0.7.1 (2019-05-07)
------------------

0.7.0 (2019-04-08)
------------------

0.6.0 (2018-11-13)
------------------

0.5.1 (2018-07-17)
------------------

0.5.0 (2018-06-13)
------------------

0.4.0 (2017-12-08)
------------------
* add ament_cmake_pytest package (`#116 <https://github.com/ament/ament_cmake/issues/116>`_)
  * add ament_cmake_pytest package
  * doc fixup
  * wrap comment
* Contributors: Dirk Thomas
