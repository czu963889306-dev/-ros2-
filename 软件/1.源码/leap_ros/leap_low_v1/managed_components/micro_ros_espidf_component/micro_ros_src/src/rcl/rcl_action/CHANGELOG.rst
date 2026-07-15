^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rcl_action
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

5.3.10 (2025-07-16)
-------------------

5.3.9 (2024-07-26)
------------------
* Generate version header using `ament_generate_version_header(..)` (backport `#1141 <https://github.com/ros2/rcl/issues/1141>`_) (`#1145 <https://github.com/ros2/rcl/issues/1145>`_)
* Contributors: mergify[bot]

5.3.8 (2024-05-15)
------------------
* add RCL_RET_TIMEOUT to action service response. (`#1138 <https://github.com/ros2/rcl/issues/1138>`_) (`#1153 <https://github.com/ros2/rcl/issues/1153>`_)
* Contributors: mergify[bot]

5.3.7 (2024-01-24)
------------------

5.3.6 (2023-11-13)
------------------

5.3.5 (2023-09-19)
------------------

5.3.4 (2023-07-17)
------------------

5.3.3 (2023-04-25)
------------------

5.3.2 (2022-09-08)
------------------

5.3.1 (2022-04-26)
------------------

5.3.0 (2022-04-05)
------------------

5.2.1 (2022-03-31)
------------------

5.2.0 (2022-03-24)
------------------

5.1.0 (2022-03-01)
------------------
* Add Events Executor (`#839 <https://github.com/ros2/rcl/issues/839>`_)
* Install includes it include/${PROJECT_NAME} (`#959 <https://github.com/ros2/rcl/issues/959>`_)
* Contributors: Shane Loretz, iRobot ROS

5.0.1 (2022-01-14)
------------------

5.0.0 (2021-12-14)
------------------
* Fix up documentation build for rcl_action when using rosdoc2 (`#937 <https://github.com/ros2/rcl/issues/937>`_)
* Contributors: Michel Hidalgo

4.0.0 (2021-09-16)
------------------
* Fix expired goals capacity of action server (`#931 <https://github.com/ros2/rcl/issues/931>`_)
* Contributors: spiralray

3.2.0 (2021-09-02)
------------------
* Wait for action server in rcl_action comm tests. (`#919 <https://github.com/ros2/rcl/issues/919>`_)
* Contributors: Michel Hidalgo

3.1.2 (2021-04-26)
------------------

3.1.1 (2021-04-12)
------------------

3.1.0 (2021-04-06)
------------------
* updating quality declaration links (re: `ros2/docs.ros2.org#52 <https://github.com/ros2/docs.ros2.org/issues/52>`_) (`#909 <https://github.com/ros2/rcl/issues/909>`_)
* Contributors: shonigmann

3.0.1 (2021-03-25)
------------------

3.0.0 (2021-03-23)
------------------

2.6.0 (2021-03-18)
------------------
* Don't expect RCL_RET_TIMEOUT to set an error string (`#900 <https://github.com/ros2/rcl/issues/900>`_)
* Add support for rmw_connextdds (`#895 <https://github.com/ros2/rcl/issues/895>`_)
* Contributors: Andrea Sorbini

2.5.2 (2021-02-05)
------------------
* Avoid setting error message twice. (`#887 <https://github.com/ros2/rcl/issues/887>`_)
* Contributors: Chen Lihui

2.5.1 (2021-01-25)
------------------

2.5.0 (2020-12-08)
------------------
* Address various clang static analysis fixes (`#864 <https://github.com/ros2/rcl/issues/864>`_)
* Update QDs to QL 1 (`#866 <https://github.com/ros2/rcl/issues/866>`_)
* Update QL (`#858 <https://github.com/ros2/rcl/issues/858>`_)
* Make sure to always check return values (`#840 <https://github.com/ros2/rcl/issues/840>`_)
* Update deprecated gtest macros (`#818 <https://github.com/ros2/rcl/issues/818>`_)
* Contributors: Alejandro Hernández Cordero, Audrow Nash, Chris Lalancette, Stephen Brawner

2.4.0 (2020-10-19)
------------------
* Make sure to check the return value of rcl APIs. (`#838 <https://github.com/ros2/rcl/issues/838>`_)
* Contributors: Chris Lalancette

2.3.0 (2020-10-19)
------------------
* Update maintainers (`#825 <https://github.com/ros2/rcl/issues/825>`_)
* Store reference to rcl_clock_t instead of copy (`#797 <https://github.com/ros2/rcl/issues/797>`_)
* Use valid clock in case of issue in rcl_timer_init (`#795 <https://github.com/ros2/rcl/issues/795>`_)
* Contributors: Ivan Santiago Paunovic, Shane Loretz, brawner

2.2.0 (2020-09-02)
------------------
* Add fault injection macros and unit tests to rcl_action (`#730 <https://github.com/ros2/rcl/issues/730>`_)
* Change some EXPECT_EQ to ASSERT_EQ in test_action_server. (`#759 <https://github.com/ros2/rcl/issues/759>`_)
* Contributors: Chris Lalancette, brawner

2.1.0 (2020-07-22)
------------------
* Removed doxygen warnings (`#712 <https://github.com/ros2/rcl/issues/712>`_)
* Address issue 716 by zero initializing pointers and freeing memory (`#717 <https://github.com/ros2/rcl/issues/717>`_)
* Contributors: Alejandro Hernández Cordero, brawner

2.0.0 (2020-07-09)
------------------
* Update quality declaration and coverage (`#674 <https://github.com/ros2/rcl/issues/674>`_)
* Contributors: Alejandro Hernández Cordero

1.2.0 (2020-06-18)
------------------
* Fixed doxygen warnings (`#677 <https://github.com/ros2/rcl/issues/677>`_)
* Contributors: Alejandro Hernández Cordero

1.1.5 (2020-06-03)
------------------

1.1.4 (2020-06-02)
------------------

1.1.3 (2020-06-01)
------------------
* Add Security Vulnerability Policy pointing to REP-2006 (`#661 <https://github.com/ros2/rcl/issues/661>`_)
* Address unused parameter warnings (`#666 <https://github.com/ros2/rcl/issues/666>`_)
* Increase test coverage of rcl_action (`#663 <https://github.com/ros2/rcl/issues/663>`_)
* Contributors: Chris Lalancette, Stephen Brawner

1.1.2 (2020-05-28)
------------------

1.1.1 (2020-05-26)
------------------

1.1.0 (2020-05-22)
------------------
* Update Quality Declaration for 1.0 (`#647 <https://github.com/ros2/rcl/issues/647>`_)
* Contributors: Stephen Brawner

1.0.0 (2020-05-12)
------------------

0.9.1 (2020-05-08)
------------------
* Included features (`#644 <https://github.com/ros2/rcl/issues/644>`_)
* Quality Declarations for rcl_action, rcl_lifecycle, yaml_parser (`#641 <https://github.com/ros2/rcl/issues/641>`_)
* Contributors: Alejandro Hernández Cordero, Stephen Brawner

0.9.0 (2020-04-29)
------------------
* Export targets in a addition to include directories / libraries (`#632 <https://github.com/ros2/rcl/issues/632>`_)
* Rename rosidl_generator_c namespace to rosidl_runtime_c (`#616 <https://github.com/ros2/rcl/issues/616>`_)
* Rename rosidl_generator_cpp namespace to rosidl_runtime_cpp (`#615 <https://github.com/ros2/rcl/issues/615>`_)
* Changed rosidl_generator_c/cpp to rosidl_runtime_c/cpp (`#588 <https://github.com/ros2/rcl/issues/588>`_)
* Changed build_depend and build_depend_export dependencies to depend (`#577 <https://github.com/ros2/rcl/issues/577>`_)
* Code style only: wrap after open parenthesis if not in one line (`#565 <https://github.com/ros2/rcl/issues/565>`_)
* Check if action status publisher is ready (`#541 <https://github.com/ros2/rcl/issues/541>`_)
* Contributors: Alejandro Hernández Cordero, Dirk Thomas, Tomoya Fujita

0.8.3 (2019-11-08)
------------------

0.8.2 (2019-10-23)
------------------
* Correct action server documentation (`#519 <https://github.com/ros2/rcl/issues/519>`_)
* Add mechanism to pass rmw impl specific payloads during pub/sub creation (`#513 <https://github.com/ros2/rcl/issues/513>`_)
* Contributors: Jacob Perron, William Woodall

0.8.1 (2019-10-08)
------------------

0.8.0 (2019-09-26)
------------------
* Fix rcl_action test_graph (`#504 <https://github.com/ros2/rcl/issues/504>`_)
* remove unused CMake code (`#475 <https://github.com/ros2/rcl/issues/475>`_)
* Contributors: Mikael Arguedas, ivanpauno

0.7.4 (2019-05-29)
------------------
* rcl_action - user friendly error messages for invalid transitions (`#448 <https://github.com/ros2/rcl/issues/448>`_)
* Contributors: Siddharth Kucheria

0.7.3 (2019-05-20)
------------------
* Fixed memory leaks in ``rcl_action`` unit tests (`#442 <https://github.com/ros2/rcl/issues/442>`_)
* Contributors: Prajakta Gokhale

0.7.2 (2019-05-08)
------------------
* Update graph test for change to rmw names and types struct (`#407 <https://github.com/ros2/rcl/issues/407>`_)
* New interfaces and their implementations for QoS features (`#408 <https://github.com/ros2/rcl/issues/408>`_)
* Add return code to CancelGoal service response (`#422 <https://github.com/ros2/rcl/issues/422>`_)
* Rmw preallocate (`#428 <https://github.com/ros2/rcl/issues/428>`_)
* Contributors: Jacob Perron, M. M, Michael Carroll

0.7.1 (2019-04-29)
------------------
* Renamed action state transitions (`#409 <https://github.com/ros2/rcl/issues/409>`_)
* Updated initialization of rmw_qos_profile_t struct instances. (`#416 <https://github.com/ros2/rcl/issues/416>`_)
* Contributors: Jacob Perron, M. M

0.7.0 (2019-04-14)
------------------
* Added Action graph API (`#411 <https://github.com/ros2/rcl/issues/411>`_)
* Updated to use ament_target_dependencies where possible. (`#400 <https://github.com/ros2/rcl/issues/400>`_)
* Fixed typo in Doxyfile. (`#398 <https://github.com/ros2/rcl/issues/398>`_)
* Updated tests to use separated action types. (`#340 <https://github.com/ros2/rcl/issues/340>`_)
* Fixed minor documentation issues. (`#397 <https://github.com/ros2/rcl/issues/397>`_)
* Set symbol visibility to hidden for rcl. (`#391 <https://github.com/ros2/rcl/issues/391>`_)
* Fixed rcl_action documentation. (`#380 <https://github.com/ros2/rcl/issues/380>`_)
* Removed now unused test executable . (`#382 <https://github.com/ros2/rcl/issues/382>`_)
* Removed unused action server option 'clock_type'. (`#382 <https://github.com/ros2/rcl/issues/382>`_)
* Set error message when there is an invalid goal transition. (`#382 <https://github.com/ros2/rcl/issues/382>`_)
* Updated to pass context to wait set, and fini rmw context (`#373 <https://github.com/ros2/rcl/issues/373>`_)
* Contributors: Dirk Thomas, Jacob Perron, Sachin Suresh Bhat, William Woodall, ivanpauno

0.6.4 (2019-01-11)
------------------
* Added parentheses around use of zerouuid macro (`#371 <https://github.com/ros2/rcl/issues/371>`_)
* Fixed logic that moves goal handles when one expires (`#360 <https://github.com/ros2/rcl/issues/360>`_)
* Updated to avoid timer period being set to 0 (`#359 <https://github.com/ros2/rcl/issues/359>`_)
* Contributors: Jacob Perron, Shane Loretz

0.6.3 (2018-12-13)
------------------

0.6.2 (2018-12-13)
------------------
* [rcl_action] Bugfix: arithmetic error
* Contributors: Jacob Perron

0.6.1 (2018-12-07)
------------------
* Added wait_for_action_server() for action clients (`#349 <https://github.com/ros2/rcl/issues/349>`_)
* Updated to adapt to action implicit changes (`#353 <https://github.com/ros2/rcl/issues/353>`_)
* Added action interaction tests (`#352 <https://github.com/ros2/rcl/issues/352>`_)
* Enabled test_action_communication to compile against available rmw. (`#351 <https://github.com/ros2/rcl/issues/351>`_)
* Changed UUID type in action msgs (`#338 <https://github.com/ros2/rcl/issues/338>`_)
* Added rcl_action_server_is_valid_except_context (`#348 <https://github.com/ros2/rcl/issues/348>`_)
* Updated to fini even if node context is invalid and reset error (`#346 <https://github.com/ros2/rcl/issues/346>`_)
* Added timer to action server to check expired goals + asan fixes (`#343 <https://github.com/ros2/rcl/issues/343>`_)
* Increased timeout for rcl_wait in action tests (`#344 <https://github.com/ros2/rcl/issues/344>`_)
* Refactored init to not be global (`#336 <https://github.com/ros2/rcl/issues/336>`_)
* Completes integration tests for action client/server (`#331 <https://github.com/ros2/rcl/issues/331>`_)
* Updated rcl_action_expire_goals() to output list of expired goals. (`#342 <https://github.com/ros2/rcl/issues/342>`_)
* Updated process_cancel_request to no longer change goal state (`#341 <https://github.com/ros2/rcl/issues/341>`_)
* Add action server implementation (`#323 <https://github.com/ros2/rcl/issues/323>`_)
* Contributors: Alexis Pojomovsky, Jacob Perron, Michel Hidalgo, Shane Loretz, William Woodall

0.6.0 (2018-11-16)
------------------
* Made rcl_action_get\_*_name() functions check for empty action names. `#329 <https://github.com/ros2/rcl/issues/329>`_
* Implemented Action client `#319 <https://github.com/ros2/rcl/issues/319>`_
* Added function to check if goal can be transitioned to CANCELING (`#325 <https://github.com/ros2/rcl/issues/325>`_)
* Implement goal handle (`#320 <https://github.com/ros2/rcl/issues/320>`_)
* Update to use new error handling API from rcutils (`#314 <https://github.com/ros2/rcl/issues/314>`_)
* Add action services and topics name getters `#317 <https://github.com/ros2/rcl/issues/317>`_
* Implement init/fini functions for types (`#312 <https://github.com/ros2/rcl/issues/312>`_)
* Refactor goal state machine implementation and add unit tests (`#311 <https://github.com/ros2/rcl/issues/311>`_)
* Add missing visibilty control definitions (`#315 <https://github.com/ros2/rcl/issues/315>`_)
* Add rcl_action package and headers (`#307 <https://github.com/ros2/rcl/issues/307>`_)
* Contributors: Jacob Perron, Michel Hidalgo, William Woodall
