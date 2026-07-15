^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ament_cmake_version
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* Fix handling of macro argument (`#220 <https://github.com/ament/ament_cmake/issues/220>`_)
  Macro invocations handle arguments differently than functions https://cmake.org/cmake/help/latest/command/macro.html#macro-vs-function
  This can cause a false positive like:
  ```
  -- Found rcutils: 0.8.4 (/opt/ros/master/install/share/rcutils/cmake)
  CMake Error at install/share/ament_cmake_version/cmake/ament_export_development_version_if_higher_than_manifest.cmake:36 (message):
  ament_export_development_version_if_higher_than_manifest() called with
  unused arguments:
  Call Stack (most recent call first):
  src/ros2/rmw/rmw/CMakeLists.txt:66 (ament_export_development_version_if_higher_than_manifest)
  ```
* Contributors: Dan Rose

0.8.1 (2019-10-23)
------------------
* add CMake macro ament_bump_development_version_if_necessary (`#204 <https://github.com/ament/ament_cmake/issues/204>`_)
  * add CMake macro ament_bump_development_version_if_necessary
  * Update ament_cmake_version/cmake/ament_bump_development_version_if_necessary.cmake
  Co-Authored-By: William Woodall <william@osrfoundation.org>
  * Update ament_cmake_version/cmake/ament_bump_development_version_if_necessary.cmake
  Co-Authored-By: William Woodall <william@osrfoundation.org>
  * quote versions in message
  * spelling: no-op
  * update macro name, add doc line about multiple invocations
* Contributors: Dirk Thomas

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
