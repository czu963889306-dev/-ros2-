^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rosidl_typesupport_cpp
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.0.2 (2024-11-25)
------------------
* Added C interfaces to obtain service and action type support. (`#149 <https://github.com/ros2/rosidl_typesupport/issues/149>`_)
* Contributors: Stefan Fabian

2.0.1 (2023-07-18)
------------------
* Don't override user provided compile definitions (backport `#145 <https://github.com/ros2/rosidl_typesupport/issues/145>`_) (`#147 <https://github.com/ros2/rosidl_typesupport/issues/147>`_)
* Contributors: mergify[bot]

* Use target_link_libraries(... PRIVATE ...) in single typesupport case (`#124 <https://github.com/ros2/rosidl_typesupport/issues/124>`_)
* rosidl CMake cleanup in rosidl_typesupport_cpp (`#123 <https://github.com/ros2/rosidl_typesupport/issues/123>`_)
* Contributors: Shane Loretz

1.5.0 (2022-03-01)
------------------
* Install headers to include/${PROJECT_NAME} (`#121 <https://github.com/ros2/rosidl_typesupport/issues/121>`_)
* Contributors: Shane Loretz

1.4.2 (2022-01-13)
------------------
* Make sure to check typesupport handles against nullptr properly (`#119 <https://github.com/ros2/rosidl_typesupport/issues/119>`_)
* Use FindPython3 (`#118 <https://github.com/ros2/rosidl_typesupport/issues/118>`_)
* Contributors: Chris Lalancette, Shane Loretz

1.4.1 (2021-08-09)
------------------
* Revert "Bundle and ensure the exportation of rosidl generated targets" (`#116 <https://github.com/ros2/rosidl_typesupport/issues/116>`_)
* Contributors: Michel Hidalgo

1.4.0 (2021-07-16)
------------------
* Support available typesupport specification in CLI extensions (`#112 <https://github.com/ros2/rosidl_typesupport/issues/112>`_)
* Bundle and ensure the exportation of rosidl generated targets (`#113 <https://github.com/ros2/rosidl_typesupport/issues/113>`_)
* Contributors: Michel Hidalgo

1.3.0 (2021-06-11)
------------------
* Fix C and C++ typesupports CLI extensions (`#111 <https://github.com/ros2/rosidl_typesupport/issues/111>`_)
* Contributors: Michel Hidalgo

1.2.1 (2021-04-06)
------------------
* updating quality declaration links (re: `ros2/docs.ros2.org#52 <https://github.com/ros2/docs.ros2.org/issues/52>`_) (`#108 <https://github.com/ros2/rosidl_typesupport/issues/108>`_)
* Contributors: shonigmann

1.2.0 (2021-03-25)
------------------
* Remove dependencies from Connext type support (`#106 <https://github.com/ros2/rosidl_typesupport/issues/106>`_)
* Contributors: Andrea Sorbini

1.1.2 (2021-03-18)
------------------
* Expose C++ typesupport generation via rosidl generate CLI (`#104 <https://github.com/ros2/rosidl_typesupport/issues/104>`_)
* Contributors: Michel Hidalgo

1.1.1 (2021-01-25)
------------------
* Remove type_support_dispatch.cpp files. (`#101 <https://github.com/ros2/rosidl_typesupport/issues/101>`_)
* Defer path resolution of rosidl typesupport libraries to dynamic linker. (`#98 <https://github.com/ros2/rosidl_typesupport/issues/98>`_)
* Ensure typesupport handle functions do not throw. (`#99 <https://github.com/ros2/rosidl_typesupport/issues/99>`_)
* Contributors: Chris Lalancette, Michel Hidalgo

1.1.0 (2020-12-08)
------------------
* Explicitly check lib pointer for null (`#95 <https://github.com/ros2/rosidl_typesupport/issues/95>`_)
* Update Quality Declaration to QL 1 (`#96 <https://github.com/ros2/rosidl_typesupport/issues/96>`_)
* Update the maintainers (`#89 <https://github.com/ros2/rosidl_typesupport/issues/89>`_)
* Added benchmark test to rosidl_typesupport_c/cpp (`#84 <https://github.com/ros2/rosidl_typesupport/issues/84>`_)
* Handle rcpputils::find_library_path() failure (`#85 <https://github.com/ros2/rosidl_typesupport/issues/85>`_)
* De-duplicate type_support_map.h header (`#81 <https://github.com/ros2/rosidl_typesupport/issues/81>`_) 
* Add fault injection macros and unit tests (`#80 <https://github.com/ros2/rosidl_typesupport/issues/80>`_)
* Add Security Vulnerability Policy pointing to REP-2006 (`#76 <https://github.com/ros2/rosidl_typesupport/issues/76>`_)
* Contributors: Alejandro Hernández Cordero, Chris Lalancette, Jose Luis Rivero, Louise Poubel, Michel Hidalgo, Stephen Brawner

1.0.0 (2020-05-26)
------------------
* Addresses test failures in release mode (`#75 <https://github.com/ros2/rosidl_typesupport/issues/75>`_)
* Add tests for type_support functions (`#63 <https://github.com/ros2/rosidl_typesupport/issues/63>`_)
* Contributors: Stephen Brawner

0.9.2 (2020-05-19)
------------------

0.9.1 (2020-05-19)
------------------
* Force extension points to be registered in order (`#73 <https://github.com/ros2/rosidl_typesupport/issues/73>`_)
* Update API documentation and QDs (`#74 <https://github.com/ros2/rosidl_typesupport/issues/74>`_)
* Complete feature documentation (`#72 <https://github.com/ros2/rosidl_typesupport/issues/72>`_)
* Add API documentation for public functions (`#64 <https://github.com/ros2/rosidl_typesupport/issues/64>`_)
* Add current quality level declarations (`#67 <https://github.com/ros2/rosidl_typesupport/issues/67>`_)
* Contributors: Ivan Santiago Paunovic, brawner

0.9.0 (2020-04-24)
------------------
* Fix single typesupport build exposing build directory in include dirs (`#71 <https://github.com/ros2/rosidl_typesupport/issues/71>`_)
* Export targets in addition to include directories / libraries (`#69 <https://github.com/ros2/rosidl_typesupport/issues/69>`_ `#70 <https://github.com/ros2/rosidl_typesupport/issues/70>`_)
* Fix build with single introspection typesupport (`#68 <https://github.com/ros2/rosidl_typesupport/issues/68>`_)
* Update includes to use non-entry point headers from detail subdirectory (`#66 <https://github.com/ros2/rosidl_typesupport/issues/66>`_)
* Rename rosidl_generator_c namespace to rosidl_runtime_c (`#65 <https://github.com/ros2/rosidl_typesupport/issues/65>`_)
* Remove dependency on rmw_implementation (`#62 <https://github.com/ros2/rosidl_typesupport/issues/62>`_)
* Added rosidl_runtime c depencency (`#58 <https://github.com/ros2/rosidl_typesupport/issues/58>`_)
* Removed poco dependency (`#59 <https://github.com/ros2/rosidl_typesupport/issues/59>`_)
* Remove OpenSplice dependencies (`#56 <https://github.com/ros2/rosidl_typesupport/issues/56>`_)
* Depend on rcpputils for find_library (`#47 <https://github.com/ros2/rosidl_typesupport/issues/47>`_)
* Contributors: Alejandro Hernández Cordero, Dirk Thomas, Eric Cousineau, Jacob Perron, Sean Kelly

0.8.0 (2019-09-26)
------------------

0.7.1 (2019-05-08)
------------------
* update code to match refactoring of rosidl definitions (`#49 <https://github.com/ros2/rosidl_typesupport/issues/49>`_)
* remove usage of UnknownMessageType (`#48 <https://github.com/ros2/rosidl_typesupport/issues/48>`_)
* Contributors: Dirk Thomas

0.7.0 (2019-04-14)
------------------
* Using ament_target_dependencies where possible (`#46 <https://github.com/ros2/rosidl_typesupport/issues/46>`_)
* change generators to IDL-based pipeline (`#39 <https://github.com/ros2/rosidl_typesupport/issues/39>`_)
* Don't validate overall actions (`#44 <https://github.com/ros2/rosidl_typesupport/issues/44>`_)
* Contributors: Dirk Thomas, Shane Loretz, ivanpauno

0.6.2 (2019-01-11)
------------------
* include available typesuppports in error message (`#43 <https://github.com/ros2/rosidl_typesupport/issues/43>`_)
* Change uncrustify max line length to 0 (`#42 <https://github.com/ros2/rosidl_typesupport/issues/42>`_)
* Contributors: Dirk Thomas, Jacob Perron

0.6.1 (2018-12-07)
------------------
* Merge pull request `#41 <https://github.com/ros2/rosidl_typesupport/issues/41>`_ from ros2/hidmic/trim-action-targets-names
* Contributors: Michel Hidalgo

0.6.0 (2018-11-16)
------------------
* Add typesupport for actions in c and c++ (`#36 <https://github.com/ros2/rosidl_typesupport/issues/36>`_)
* Allow generated IDL files (`#35 <https://github.com/ros2/rosidl_typesupport/issues/35>`_)
* Merge pull request `#33 <https://github.com/ros2/rosidl_typesupport/issues/33>`_ from ros2/hidmic/prepare_for_action_generation
* Removes remaininig srv folder assumptions.
* Makes rosidl interfaces generation action folder aware.
* expose symbol for cpp typesupport (`#32 <https://github.com/ros2/rosidl_typesupport/issues/32>`_)
* update manifest to adhere to tag order in schema (`#30 <https://github.com/ros2/rosidl_typesupport/issues/30>`_)
* Contributors: Alexis Pojomovsky, Dirk Thomas, Karsten Knese, Michel Hidalgo, Shane Loretz

0.5.0 (2018-06-24)
------------------
* Prepare dependencies for bouncy release. (`#27 <https://github.com/ros2/rosidl_typesupport/issues/27>`_)
* add and use groups for generator and runtime packages (`#25 <https://github.com/ros2/rosidl_typesupport/issues/25>`_)
* Merge pull request `#23 <https://github.com/ros2/rosidl_typesupport/issues/23>`_ from ros2/misra_fixup
* Merge pull request `#22 <https://github.com/ros2/rosidl_typesupport/issues/22>`_ from ros2/use_typesupport_group
* use CMAKE_CURRENT_BINARY_DIR for arguments json (`#21 <https://github.com/ros2/rosidl_typesupport/issues/21>`_)
* use ament_cmake_ros (`#19 <https://github.com/ros2/rosidl_typesupport/issues/19>`_)
* Contributors: Dirk Thomas, Michael Carroll, Steven! Ragnarök

0.4.0 (2017-12-08)
------------------
* update service type support header name (`#15 <https://github.com/ros2/rosidl_typesupport/issues/15>`_)
* Contributors: Dirk Thomas, Mikael Arguedas, Morgan Quigley, William Woodall
