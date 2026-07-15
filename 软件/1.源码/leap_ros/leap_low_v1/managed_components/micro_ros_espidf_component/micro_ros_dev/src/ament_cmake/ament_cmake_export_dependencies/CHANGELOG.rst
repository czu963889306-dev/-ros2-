^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cmake_export_dependencies
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
* Resolve various ament_lint linter violations (`#360 <https://github.com/ament/ament_cmake/issues/360>`_)
  We can't add ament_lint linters in ament_cmake in the traditional way
  without creating a circular dependency between the repositories. Even
  though we can't automatically enforce linting, it's still a good idea to
  try to keep conformance where possible.
* Update maintainers to Michael Jeronimo and Michel Hidalgo (`#362 <https://github.com/ament/ament_cmake/issues/362>`_)
* Contributors: Audrow Nash, Scott K Logan

1.2.0 (2021-10-29)
------------------
* Use FindPython3 instead of FindPythonInterp (`#355 <https://github.com/ament/ament_cmake/issues/355>`_)
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
* fix cmake list(TRANSFORM ) is only available from version 3.12, (`#296 <https://github.com/ament/ament_cmake/issues/296>`_)
  convert to string instead
* fix imported targets with multiple configuration (`#290 <https://github.com/ament/ament_cmake/issues/290>`_)
  * fix imported targets with multiple configuration
  * taking into account DEBUG_CONFIGURATIONS global variable
* Update package maintainers. (`#286 <https://github.com/ament/ament_cmake/issues/286>`_)
* Contributors: Michel Hidalgo, siposcsaba89

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

0.8.1 (2019-10-23)
------------------

0.8.0 (2019-10-04)
------------------

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
* 0.0.3
* 0.0.2
* Merge pull request `#71 <https://github.com/ament/ament_cmake/issues/71>`_ from ament/export_link_flags
  add ament_cmake_export_link_flags package and use link flags in ament_target_dependencies
* add ament_cmake_export_link_flags package and use link flags in ament_target_dependencies
* update schema url
* add schema to manifest files
* Merge pull request `#72 <https://github.com/ament/ament_cmake/issues/72>`_ from ament/cmake35
  require CMake 3.5
* remove trailing spaces from comparisons, obsolete quotes and explicit variable expansion
* require CMake 3.5
* Merge pull request `#47 <https://github.com/ament/ament_cmake/issues/47>`_ from ament/dedup_info_from_depends
  deduplicate DEFINITIONS, INCLUDE_DIRS and LIBRARIES from exported dependencies
* deduplicate DEFINITIONS, INCLUDE_DIRS and LIBRARIES from exported dependencies
* add explicit build type
* Merge pull request `#15 <https://github.com/ament/ament_cmake/issues/15>`_ from ament/fix_message_dependencies
  export direct and recursive package dependencies
* export direct and recursive package dependencies
* disable debug output
* add missing copyright / license information, update format of existing license information
* use project(.. NONE)
* refactor several low-level packages into ament_cmake_core (environment, environment_hooks, index, package_templates, symlink_install)
* invert dependency between ament_cmake_environment and ament_cmake_environment_hooks, add dependency on ament_cmake_environment
* deal with CMake double expansion
* add definitions to exported variables for dependencies
* fix libraries when exporting package dependencies
* update cmake code style
* minor
* add ament_cmake_export_interfaces
* add ament_cmake_export_dependencies
* Contributors: Dirk Thomas
