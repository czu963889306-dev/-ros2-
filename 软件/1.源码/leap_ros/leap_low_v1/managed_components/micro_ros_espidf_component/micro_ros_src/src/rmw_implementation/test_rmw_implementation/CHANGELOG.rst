^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package test_rmw_implementation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.8.5 (2025-07-16)
------------------
* add ignore_local_publications_serialized test. (backport `#255 <https://github.com/ros2/rmw_implementation/issues/255>`_) (`#258 <https://github.com/ros2/rmw_implementation/issues/258>`_)
* Contributors: mergify[bot]

2.8.4 (2024-07-26)
------------------
* Add test creating two content filter topics with the same topic name … (backport `#233 <https://github.com/ros2/rmw_implementation/issues/233>`_) (`#236 <https://github.com/ros2/rmw_implementation/issues/236>`_)
* Contributors: mergify[bot]

2.8.3 (2024-05-15)
------------------

2.8.2 (2023-01-10)
------------------

2.8.1 (2022-03-28)
------------------
* add content-filtered-topic interfaces (`#181 <https://github.com/ros2/rmw_implementation/issues/181>`_)
* Contributors: Chen Lihui

2.8.0 (2022-03-01)
------------------

2.7.1 (2022-01-14)
------------------
* Fix linter issues (`#200 <https://github.com/ros2/rmw_implementation/issues/200>`_)
* Contributors: Jorge Perez

2.7.0 (2021-11-19)
------------------
* Add client/service QoS getters. (`#196 <https://github.com/ros2/rmw_implementation/issues/196>`_)
* Contributors: mauropasse

2.6.1 (2021-11-18)
------------------
* Added tests for bounded sequences serialization (`#193 <https://github.com/ros2/rmw_implementation/issues/193>`_)
* Contributors: Miguel Company

2.6.0 (2021-08-09)
------------------
* Add RMW_DURATION_INFINITE basic compliance test. (`#194 <https://github.com/ros2/rmw_implementation/issues/194>`_)
* Test SubscriptionOptions::ignore_local_publications. (`#192 <https://github.com/ros2/rmw_implementation/issues/192>`_)
* Add rmw_publisher_wait_for_all_acked. (`#188 <https://github.com/ros2/rmw_implementation/issues/188>`_)
* Wait for server in test_rmw_implementation service tests. (`#191 <https://github.com/ros2/rmw_implementation/issues/191>`_)
* Contributors: Barry Xu, Emerson Knapp, Jose Antonio Moral, Michel Hidalgo

2.5.0 (2021-05-05)
------------------

2.4.1 (2021-04-16)
------------------
* Implement test for subscription loaned messages (`#186 <https://github.com/ros2/rmw_implementation/issues/186>`_)
* Contributors: Miguel Company

2.4.0 (2021-04-06)
------------------

2.3.0 (2021-03-25)
------------------
* Remove rmw_connext_cpp. (`#183 <https://github.com/ros2/rmw_implementation/issues/183>`_)
* Add support for rmw_connextdds (`#182 <https://github.com/ros2/rmw_implementation/issues/182>`_)
* Contributors: Andrea Sorbini, Chris Lalancette

2.2.0 (2021-03-08)
------------------
* Add function for checking QoS profile compatibility (`#180 <https://github.com/ros2/rmw_implementation/issues/180>`_)
* Make sure to initialize the rmw_message_sequence after init. (`#175 <https://github.com/ros2/rmw_implementation/issues/175>`_)
* Set the value of is_available before entering the loop (`#173 <https://github.com/ros2/rmw_implementation/issues/173>`_)
* Contributors: Chris Lalancette, Jacob Perron

2.1.2 (2021-01-29)
------------------
* Set the return value of rmw_ret_t before entering the loop. (`#171 <https://github.com/ros2/rmw_implementation/issues/171>`_)
* Contributors: Chris Lalancette

2.1.1 (2021-01-25)
------------------

2.1.0 (2020-12-10)
------------------
* Add some additional checking that cleanup happens. (`#168 <https://github.com/ros2/rmw_implementation/issues/168>`_)
* Add test to check rmw_send_response when the client is gone (`#162 <https://github.com/ros2/rmw_implementation/issues/162>`_)
* Update maintainers (`#154 <https://github.com/ros2/rmw_implementation/issues/154>`_)
* Add fault injection tests to construction/destroy APIs.  (`#144 <https://github.com/ros2/rmw_implementation/issues/144>`_)
* Add tests bad type_support implementation (`#152 <https://github.com/ros2/rmw_implementation/issues/152>`_)
* Add tests for localhost-only node creation (`#150 <https://github.com/ros2/rmw_implementation/issues/150>`_)
* Added rmw_service_server_is_available tests (`#140 <https://github.com/ros2/rmw_implementation/issues/140>`_)
* Use 10x the intraprocess delay to wait for sent requests. (`#148 <https://github.com/ros2/rmw_implementation/issues/148>`_)
* Added rmw_wait, rmw_create_wait_set, and rmw_destroy_wait_set tests (`#139 <https://github.com/ros2/rmw_implementation/issues/139>`_)
* Add tests service/client request/response with bad arguments (`#141 <https://github.com/ros2/rmw_implementation/issues/141>`_)
* Added test for rmw_get_serialized_message_size (`#142 <https://github.com/ros2/rmw_implementation/issues/142>`_)
* Add service/client construction/destruction API test coverage. (`#138 <https://github.com/ros2/rmw_implementation/issues/138>`_)
* Added rmw_publisher_allocation and rmw_subscription_allocation related tests (`#137 <https://github.com/ros2/rmw_implementation/issues/137>`_)
* Add tests take serialized with info bad arguments (`#130 <https://github.com/ros2/rmw_implementation/issues/130>`_)
* Add gid API test coverage. (`#134 <https://github.com/ros2/rmw_implementation/issues/134>`_)
* Add tests take bad arguments  (`#125 <https://github.com/ros2/rmw_implementation/issues/125>`_)
* Bump graph API test coverage. (`#132 <https://github.com/ros2/rmw_implementation/issues/132>`_)
* Add tests take sequence serialized with bad arguments (`#129 <https://github.com/ros2/rmw_implementation/issues/129>`_)
* Add tests take sequence + take sequence with bad arguments (`#128 <https://github.com/ros2/rmw_implementation/issues/128>`_)
* Add tests take with info bad arguments (`#126 <https://github.com/ros2/rmw_implementation/issues/126>`_)
* Add tests for non-implemented rmw_take\_* functions (`#131 <https://github.com/ros2/rmw_implementation/issues/131>`_)
* Add tests publish serialized bad arguments (`#124 <https://github.com/ros2/rmw_implementation/issues/124>`_)
* Add tests publish bad arguments (`#123 <https://github.com/ros2/rmw_implementation/issues/123>`_)
* Add tests non-implemented functions + loan bad arguments (`#122 <https://github.com/ros2/rmw_implementation/issues/122>`_)
* Add missing empty topic name tests. (`#136 <https://github.com/ros2/rmw_implementation/issues/136>`_)
* Add rmw_get_serialization_format() smoke test. (`#133 <https://github.com/ros2/rmw_implementation/issues/133>`_)
* Complete publisher/subscription QoS query API test coverage. (`#120 <https://github.com/ros2/rmw_implementation/issues/120>`_)
* Remove duplicate assertions (`#121 <https://github.com/ros2/rmw_implementation/issues/121>`_)
* Add publisher/subscription matched count API test coverage. (`#119 <https://github.com/ros2/rmw_implementation/issues/119>`_)
* Add serialize/deserialize API test coverage. (`#118 <https://github.com/ros2/rmw_implementation/issues/118>`_)
* Add subscription API test coverage. (`#117 <https://github.com/ros2/rmw_implementation/issues/117>`_)
* Extend publisher API test coverage (`#115 <https://github.com/ros2/rmw_implementation/issues/115>`_)
* Add node construction/destruction API test coverage. (`#112 <https://github.com/ros2/rmw_implementation/issues/112>`_)
* Check that rmw_init() fails if no enclave is given. (`#113 <https://github.com/ros2/rmw_implementation/issues/113>`_)
* Contributors: Alejandro Hernández Cordero, Chris Lalancette, Geoffrey Biggs, Jose Tomas Lorente, José Luis Bueno López, Michel Hidalgo

2.0.0 (2020-07-08)
------------------
* Add init options API test coverage. (`#108 <https://github.com/ros2/rmw_implementation/issues/108>`_)
* Complete init/shutdown API test coverage. (`#107 <https://github.com/ros2/rmw_implementation/issues/107>`_)
* Add dependency on ament_cmake_gtest (`#109 <https://github.com/ros2/rmw_implementation/issues/109>`_)
* Add test_rmw_implementation package. (`#106 <https://github.com/ros2/rmw_implementation/issues/106>`_)
* Contributors: Ivan Santiago Paunovic, Michel Hidalgo, Shane Loretz
