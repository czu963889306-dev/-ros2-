^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cmake_auto
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.3.14 (2026-02-23)
-------------------

1.3.13 (2026-01-13)
-------------------

1.3.12 (2025-06-27)
-------------------
* Add `USE_SCOPED_HEADER_INSTALL_DIR` option to ament_auto_package (`#577 <https://github.com/ament/ament_cmake/issues/577>`_)
* Contributors: Kotaro Yoshimoto

1.3.11 (2024-11-25)
-------------------

1.3.10 (2024-07-26)
-------------------
* More specific prefix in some cmake_parse_argument calls (`#523 <https://github.com/ament/ament_cmake/issues/523>`_) (`#538 <https://github.com/ament/ament_cmake/issues/538>`_)
* Contributors: mergify[bot]

1.3.9 (2024-05-15)
------------------

1.3.8 (2024-02-16)
------------------

1.3.7 (2024-01-24)
------------------
* Add ament_auto_add_gmock to ament_cmake_auto (`#482 <https://github.com/ament/ament_cmake/issues/482>`_) (`#493 <https://github.com/ament/ament_cmake/issues/493>`_)
* Contributors: mergify[bot]

1.3.6 (2023-11-13)
------------------

1.3.5 (2023-06-22)
------------------

1.3.4 (2023-04-25)
------------------
* Fix ament_auto_add_gtest's parameter passing (backport `#421 <https://github.com/ament/ament_cmake/issues/421>`_) (`#424 <https://github.com/ament/ament_cmake/issues/424>`_)
* Contributors: mergify[bot]

1.3.3 (2022-11-07)
------------------
* Rolling: ament_cmake_auto should include dependencies as SYSTEM (`#385 <https://github.com/ament/ament_cmake/issues/385>`_) (`#404 <https://github.com/ament/ament_cmake/issues/404>`_)
* Contributors: mergify[bot]

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
* Fix typo in ament_auto_find_test_dependencies (`#363 <https://github.com/ament/ament_cmake/issues/363>`_)
* Update maintainers to Michael Jeronimo and Michel Hidalgo (`#362 <https://github.com/ament/ament_cmake/issues/362>`_)
* Contributors: Audrow Nash, Daisuke Nishimatsu

1.2.0 (2021-10-29)
------------------
* Add ament_auto_add_gtest (`#344 <https://github.com/ament/ament_cmake/issues/344>`_)
* Use FindPython3 instead of FindPythonInterp (`#355 <https://github.com/ament/ament_cmake/issues/355>`_)
* Update maintainers (`#336 <https://github.com/ament/ament_cmake/issues/336>`_)
* Contributors: Chris Lalancette, Joshua Whitley, Shane Loretz

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

0.8.1 (2019-10-23)
------------------

0.8.0 (2019-10-04)
------------------
* pass unparsed argument of ament_auto_package() to ament_package() (`#194 <https://github.com/ament/ament_cmake/issues/194>`_)
* Contributors: Dirk Thomas

0.7.3 (2019-05-29)
------------------

0.7.2 (2019-05-20)
------------------

0.7.1 (2019-05-07)
------------------
* Add option to ament_auto_package to install to share folder: (`#166 <https://github.com/ament/ament_cmake/issues/166>`_)
  - This will simplify installing folders like 'cmake' and 'launch'
  into a package's shared folder
* Contributors: jpsamper2009

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
* 0.0.3
* Install ament_cmake_auto executables to libexec by default (`#97 <https://github.com/ament/ament_cmake/issues/97>`_)
  * Install ament_cmake_auto executables to libexec by default
  * update docblock
  * simplify installing executables
* 0.0.2
* Add optional list of required packages for ament_auto_find_build_dependencies (`#93 <https://github.com/ament/ament_cmake/issues/93>`_)
  * Add optional list of required packages
  * Prefix ARG variables + fixup
  * REQUIRED_PACKAGES -> REQUIRED
  * Output all ignored packages at once
  * Pass REQUIRED in addition to QUIET, not instead of
  * _ignored_pacakges -> _additional_packages
  * De-duplicate the find_package call
  * rename var and small changes
* Merge pull request `#86 <https://github.com/ament/ament_cmake/issues/86>`_ from ament/remove_include
  remove unnecessary include
* remove unnecessary include
* Merge pull request `#84 <https://github.com/ament/ament_cmake/issues/84>`_ from ament/use_in_list
  use IN_LIST
* use IN_LIST
* update schema url
* add schema to manifest files
* Merge pull request `#72 <https://github.com/ament/ament_cmake/issues/72>`_ from ament/cmake35
  require CMake 3.5
* remove trailing spaces from comparisons, obsolete quotes and explicit variable expansion
* require CMake 3.5
* add explicit build type
* disable debug output
* add missing copyright / license information, update format of existing license information
* Merge pull request `#3 <https://github.com/ament/ament_cmake/issues/3>`_ from ament/windows
  Windows Support
* [windows] fixed installation of dll's
* use project(.. NONE)
* deal with CMake double expansion
* add ament_cmake_libraries
* update cmake code style
* add ament_cmake_auto
* Contributors: Dirk Thomas, William Woodall, dhood
