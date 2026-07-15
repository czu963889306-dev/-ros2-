^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cmake_target_dependencies
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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

1.3.5 (2023-06-22)
------------------

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
* Contributors: Audrow Nash

1.2.0 (2021-10-29)
------------------
* Use FindPython3 instead of FindPythonInterp (`#355 <https://github.com/ament/ament_cmake/issues/355>`_)
* Fix bug packages with multiple configurations (`#318 <https://github.com/ament/ament_cmake/issues/318>`_)
* Update maintainers (`#336 <https://github.com/ament/ament_cmake/issues/336>`_)
* Contributors: Chris Lalancette, Shane Loretz

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
* Force SYSTEM keyword in ament_target_dependencies() at the start. (`#303 <https://github.com/ament/ament_cmake/issues/303>`_)
* Add SYSTEM keyword option to ament_target_dependencies (`#297 <https://github.com/ament/ament_cmake/issues/297>`_)
  * Add SYSTEM keyword option to ament_target_dependencies
  * Add documentation of SYSTEM keyword for ament_target_dependencies
* Update package maintainers. (`#286 <https://github.com/ament/ament_cmake/issues/286>`_)
* Contributors: Andre Nguyen, Michel Hidalgo

1.0.1 (2020-09-10)
------------------

1.0.0 (2020-07-22)
------------------

0.9.6 (2020-06-23)
------------------
* ordered interface include dirs and use privately to ensure workspace order (`#260 <https://github.com/ament/ament_cmake/issues/260>`_)
* Contributors: Dirk Thomas

0.9.5 (2020-06-02)
------------------

0.9.4 (2020-05-26)
------------------
* add ament_get_recursive_properties (`#259 <https://github.com/ament/ament_cmake/issues/259>`_)
  * add ament_get_recursive_properties
  * fix spelling in docblock
* Contributors: Dirk Thomas

0.9.3 (2020-05-19)
------------------

0.9.2 (2020-05-07)
------------------
* redo use _TARGETS over deprecated _INTERFACES over classic CMake variables (`#251 <https://github.com/ament/ament_cmake/issues/251>`_)
  * redo use _TARGETS over deprecated _INTERFACES over classic CMake variables
  * update ament_export_dependencies accordingly
  * also add IMPORTED_LOCATION to the libraries
  * simplify conditions
  * consider IMPORTED_IMPLIB for Windows
* Contributors: Dirk Thomas

0.9.1 (2020-04-24 15:45)
------------------------

0.9.0 (2020-04-24 12:25)
------------------------
* Revert "use _TARGETS over deprecated _INTERFACES over classic CMake variables (`#249 <https://github.com/ament/ament_cmake/issues/249>`_)" (`#250 <https://github.com/ament/ament_cmake/issues/250>`_)
  This reverts commit 1abe330837cf98632225e4af23ac610af863fb3e.
* use _TARGETS over deprecated _INTERFACES over classic CMake variables (`#249 <https://github.com/ament/ament_cmake/issues/249>`_)
  * use _TARGETS over deprecated _INTERFACES over classic CMake variables
  * fix spelling of variable
* interface targets don't support compile definitions and LINK_FLAGS (`#247 <https://github.com/ament/ament_cmake/issues/247>`_)
* Handle non-library tokens in _LIBRARIES. (`#248 <https://github.com/ament/ament_cmake/issues/248>`_)
  * Handle non-library tokens in _LIBRARIES.
  * Drop spaces on  clauses.
* Use _LIBRARY_DIRS in ament_target_dependencies() (`#245 <https://github.com/ament/ament_cmake/issues/245>`_)
* add INTERFACE option to ament_target_dependencies() (`#246 <https://github.com/ament/ament_cmake/issues/246>`_)
* use modern interface targets if available, otherwise classic variables (`#235 <https://github.com/ament/ament_cmake/issues/235>`_)
  * use modern interface targets if available, otherwise classic variables, support interface keyword
  * remove INTERFACE keyword for now
* consider exported interfaces in ament_target_dependencies (`#232 <https://github.com/ament/ament_cmake/issues/232>`_)
* Contributors: Dirk Thomas, Michel Hidalgo

0.8.1 (2019-10-23)
------------------

0.8.0 (2019-10-04)
------------------
* Quote variable to avoid odd cmake if behavior (`#180 <https://github.com/ament/ament_cmake/issues/180>`_)
* Contributors: Shane Loretz

0.7.3 (2019-05-29)
------------------

0.7.2 (2019-05-20)
------------------

0.7.1 (2019-05-07)
------------------

0.7.0 (2019-04-08)
------------------
* Added PUBLIC option to ament_target_dependencies (`#161 <https://github.com/ament/ament_cmake/issues/161>`_)
  * Added PUBLIC option to ament_target_dependencies
  * Corrected with PR comments.
  * simplify patch
* Contributors: ivanpauno

0.6.0 (2018-11-13)
------------------

0.5.1 (2018-07-17)
------------------

0.5.0 (2018-06-13)
------------------

0.4.0 (2017-12-08)
------------------
* 0.0.3
* 0.0.2
* Merge pull request `#71 <https://github.com/ament/ament_cmake/issues/71>`_ from ament/export_link_flags
  add ament_cmake_export_link_flags package and use link flags in ament_target_dependencies
* add ament_cmake_export_link_flags package and use link flags in ament_target_dependencies
* update schema url
* add schema to manifest files
* Merge pull request `#72 <https://github.com/ament/ament_cmake/issues/72>`_ from ament/cmake35
  require CMake 3.5
* require CMake 3.5
* add explicit build type
* disable debug output
* add missing copyright / license information, update format of existing license information
* Merge pull request `#3 <https://github.com/ament/ament_cmake/issues/3>`_ from ament/windows
  Windows Support
* [windows] fixed installation of dll's
* use project(.. NONE)
* refactor several low-level packages into ament_cmake_core (environment, environment_hooks, index, package_templates, symlink_install)
* invert dependency between ament_cmake_environment and ament_cmake_environment_hooks, add dependency on ament_cmake_environment
* add ament_cmake_include_directories
* add ament_cmake_libraries
* add ament_cmake_target_dependencies
* Contributors: Dirk Thomas, William Woodall
