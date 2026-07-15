^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rmw
^^^^^^^^^^^^^^^^^^^^^^^^^

6.1.2 (2024-05-15)
------------------
* rmw_send_reqponse returns RMW_RET_TIMEOUT. (`#350 <https://github.com/ros2/rmw/issues/350>`_) (`#367 <https://github.com/ros2/rmw/issues/367>`_)
* Contributors: mergify[bot]

6.1.1 (2022-11-07)
------------------
* callback can be NULL to clear in Listener APIs. (`#332 <https://github.com/ros2/rmw/issues/332>`_) (`#333 <https://github.com/ros2/rmw/issues/333>`_)
* Contributors: mergify[bot]

6.1.0 (2022-03-25)
------------------
* Add content filtered topics support. (`#302 <https://github.com/ros2/rmw/issues/302>`_)
* Add sequence numbers to rmw_message_info_t. (`#318 <https://github.com/ros2/rmw/issues/318>`_)
* Add rmw_feature_supported(). (`#318 <https://github.com/ros2/rmw/issues/318>`_)
* Contributors: Chen Lihui, Ivan Santiago Paunovic

6.0.0 (2022-03-01)
------------------
* Add EventsExecutor (`#286 <https://github.com/ros2/rmw/issues/286>`_)
* Document that rmw_wait() SHOULD use a monotonic clock (`#316 <https://github.com/ros2/rmw/issues/316>`_)
* Install headers to include/${PROJECT_NAME} (`#317 <https://github.com/ros2/rmw/issues/317>`_)
* Update rmw_server_is_available() API documentation. (`#277 <https://github.com/ros2/rmw/issues/277>`_)
* Contributors: Ivan Santiago Paunovic, Michel Hidalgo, Shane Loretz, iRobot ROS

5.1.0 (2021-11-19)
------------------
* Add client/service QoS getters. (`#314 <https://github.com/ros2/rmw/issues/314>`_)
* Contributors: mauropasse

5.0.0 (2021-09-15)
------------------
* Fix up documentation build for rmw when using rosdoc2 (`#313 <https://github.com/ros2/rmw/issues/313>`_)
* Fix up errors in doxygen documentation (`#311 <https://github.com/ros2/rmw/issues/311>`_)
* Fix copy-paste error in API doc for rmw_get_gid_for_publisher (`#310 <https://github.com/ros2/rmw/issues/310>`_)
* Contributors: Chris Lalancette, Christophe Bedard, Michel Hidalgo

4.0.0 (2021-06-02)
------------------
* Add rmw_publisher_wait_for_all_acked support. (`#296 <https://github.com/ros2/rmw/issues/296>`_)
* Contributors: Barry Xu

3.3.1 (2021-04-26)
------------------
* Document which QoS policies are correctly read by rmw_get_publishers/subscriptions_info_by_topic (`#308 <https://github.com/ros2/rmw/issues/308>`_)
* Contributors: Ivan Santiago Paunovic

3.3.0 (2021-04-06)
------------------
* Unique network flows (`#294 <https://github.com/ros2/rmw/issues/294>`_)
* updating quality declaration links (re: `ros2/docs.ros2.org#52 <https://github.com/ros2/docs.ros2.org/issues/52>`_) (`#307 <https://github.com/ros2/rmw/issues/307>`_)
* Contributors: Ananya Muddukrishna, shonigmann

3.2.0 (2021-03-11)
------------------
* Introduce RMW_DURATION_INFINITE constant and API return value promise (`#301 <https://github.com/ros2/rmw/issues/301>`_)
* Contributors: Emerson Knapp

3.1.0 (2021-02-25)
------------------
* Add declaration for function to check QoS profile compatibility (`#299 <https://github.com/ros2/rmw/issues/299>`_)
* Update the rmw_take_sequence documentation. (`#297 <https://github.com/ros2/rmw/issues/297>`_)
* Contributors: Chris Lalancette, Jacob Perron

3.0.0 (2021-01-25)
------------------

2.2.1 (2020-12-10)
------------------
* Update rmw QD to QL 1 (`#289 <https://github.com/ros2/rmw/issues/289>`_)
* Contributors: Stephen Brawner

2.2.0 (2020-11-04)
------------------
* Extend rmw_qos_policy_kind_t, add functions to convert it to/from a string (`#285 <https://github.com/ros2/rmw/issues/285>`_)
* Add functions to convert between qos policy values and strings (`#284 <https://github.com/ros2/rmw/issues/284>`_)
* Update maintainers (`#282 <https://github.com/ros2/rmw/issues/282>`_)
* Update service request/response API documentation (`#279 <https://github.com/ros2/rmw/issues/279>`_)
* Update rmw_get_serialized_message_size docblock (`#281 <https://github.com/ros2/rmw/issues/281>`_)
* Update rmw_service_server_is_available doc (`#280 <https://github.com/ros2/rmw/issues/280>`_)
* Update wait and wait sets' API documentation (`#275 <https://github.com/ros2/rmw/issues/275>`_)
* Update graph API documentation (`#272 <https://github.com/ros2/rmw/issues/272>`_)
* Update service server/client creation/destruction API documentation. (`#276 <https://github.com/ros2/rmw/issues/276>`_)
* Update rmw\_*_*_allocation return values (`#278 <https://github.com/ros2/rmw/issues/278>`_)
* Update gid API documentation (`#274 <https://github.com/ros2/rmw/issues/274>`_)
* Do not link against pthread on Android (`#267 <https://github.com/ros2/rmw/issues/267>`_)
* Update taking API documentation (`#271 <https://github.com/ros2/rmw/issues/271>`_)
* Update publishing API documentation (`#270 <https://github.com/ros2/rmw/issues/270>`_)
* Add fault injection macros for use in other packages (`#254 <https://github.com/ros2/rmw/issues/254>`_)
* Add bad_alloc return to topic_endpoint_info functions (`#269 <https://github.com/ros2/rmw/issues/269>`_)
* Update publisher/subscription matched count API documentation (`#262 <https://github.com/ros2/rmw/issues/262>`_)
* Update publisher/subscription QoS query API documentation (`#263 <https://github.com/ros2/rmw/issues/263>`_)
* Extend rmw_serialized_message_t tests (`#261 <https://github.com/ros2/rmw/issues/261>`_)
* Update serialization/deserialization API documentation (`#258 <https://github.com/ros2/rmw/issues/258>`_)
* Update subscription API documentation (`#256 <https://github.com/ros2/rmw/issues/256>`_)
* Update publisher creation/destruction API documentation (`#252 <https://github.com/ros2/rmw/issues/252>`_)
* Contributors: Alejandro Hernández Cordero, Ivan Santiago Paunovic, Jacob Perron, Michel Hidalgo, brawner

2.1.0 (2020-07-22)
------------------
* Add actual domain id to rmw_context_t (`#251 <https://github.com/ros2/rmw/issues/251>`_)
* Update node creation/destruction API documentation. (`#249 <https://github.com/ros2/rmw/issues/249>`_)
* Correct parameter names to match documentation (`#250 <https://github.com/ros2/rmw/issues/250>`_)
* Contributors: Geoffrey Biggs, Ivan Santiago Paunovic, Michel Hidalgo

2.0.0 (2020-07-08)
------------------
* Remove domain_id and localhost_only from node API (`#248 <https://github.com/ros2/rmw/issues/248>`_)
* Require enclave upon rmw_init() call. (`#247 <https://github.com/ros2/rmw/issues/247>`_)
* Update init/shutdown API documentation. (`#243 <https://github.com/ros2/rmw/issues/243>`_)
* Update init options API documentation. (`#244 <https://github.com/ros2/rmw/issues/244>`_)
* Contributors: Ivan Santiago Paunovic, Michel Hidalgo

1.1.0 (2020-06-18)
------------------
* Add message lost subscription event (`#232 <https://github.com/ros2/rmw/issues/232>`_)
* Move statuses definitions to rmw/events_statuses/*.h (`#232 <https://github.com/ros2/rmw/issues/232>`_)
* Increase rmw testing coverage above 95% (`#238 <https://github.com/ros2/rmw/issues/238>`_)
* Handle zero-length names_and_types properly (`#239 <https://github.com/ros2/rmw/issues/239>`_)
* Add missing RMW_PUBLIC to security_options_set_root_path (`#236 <https://github.com/ros2/rmw/issues/236>`_)
* Update Quality Declaration for QL 2 (`#233 <https://github.com/ros2/rmw/issues/233>`_)
* Add Security Vulnerability Policy pointing to REP-2006. (`#230 <https://github.com/ros2/rmw/issues/230>`_)
* Contributors: Chris Lalancette, Ivan Santiago Paunovic, Karsten Knese, Scott K Logan, Stephen Brawner, brawner

1.0.1 (2020-05-19)
------------------
* Fix cppcheck error (`#229 <https://github.com/ros2/rmw/issues/229>`_)
* Update Quality Declaration to reflect 1.0 (`#228 <https://github.com/ros2/rmw/issues/228>`_)
* Contributors: Michel Hidalgo, Stephen Brawner

1.0.0 (2020-05-12)
------------------
* Remove MANUAL_BY_NODE liveliness API (`#227 <https://github.com/ros2/rmw/issues/227>`_)
* Improved Quality declarations (`#225 <https://github.com/ros2/rmw/issues/225>`_)
* Quality declarations for rmw and rmw_implementation_cmake (`#205 <https://github.com/ros2/rmw/issues/205>`_)
* Add tests for untested public functionality (`#203 <https://github.com/ros2/rmw/issues/203>`_)
* Contributors: Alejandro Hernández Cordero, Ivan Santiago Paunovic, Stephen Brawner

0.9.0 (2020-04-24)
------------------
* Delete superfluous empty line (`#222 <https://github.com/ros2/rmw/issues/222>`_)
* Fix linter warning (`#224 <https://github.com/ros2/rmw/issues/224>`_)
* Rename rosidl_message_bounds_t (`#223 <https://github.com/ros2/rmw/issues/223>`_)
* Adding doxygen documentation and READMEs to packages (`#204 <https://github.com/ros2/rmw/issues/204>`_)
* Service timestamps (`#217 <https://github.com/ros2/rmw/issues/217>`_)
* Add API for taking a sequence of messages (`#212 <https://github.com/ros2/rmw/issues/212>`_)
* Add timestamps to message info (`#214 <https://github.com/ros2/rmw/issues/214>`_)
* Add build dep on rosidl_runtime_c to work with CMake < 3.13 (`#221 <https://github.com/ros2/rmw/issues/221>`_)
* Fix missing target dependency on rosidl_runtime_c (`#220 <https://github.com/ros2/rmw/issues/220>`_)
* Export targets in addition to include directories / libraries (`#218 <https://github.com/ros2/rmw/issues/218>`_)
* Document destroy_node may assume correct destruction order (`#216 <https://github.com/ros2/rmw/issues/216>`_)
* security-context -> enclave (`#211 <https://github.com/ros2/rmw/issues/211>`_)
* Rename rosidl_generator_c namespace to rosidl_runtime_c (`#213 <https://github.com/ros2/rmw/issues/213>`_)
* Added the right dependency rosidl runtime c instead of rosidl generator c (`#198 <https://github.com/ros2/rmw/issues/198>`_)
* Use one participant per context API changes (`#189 <https://github.com/ros2/rmw/issues/189>`_)
* Add comment about RMW_RET_UNSUPPORTED for loaned_message. (`#208 <https://github.com/ros2/rmw/issues/208>`_)
* Support for ON_REQUESTED_INCOMPATIBLE_QOS and ON_OFFERED_INCOMPATIBLE_QOS events (`#193 <https://github.com/ros2/rmw/issues/193>`_)
* Move rmw\_*_event_init() functions to rmw_implementation (`#202 <https://github.com/ros2/rmw/issues/202>`_)
* Rename rmw_topic_endpoint_info_array `count` to `size`, and initialize it (`#196 <https://github.com/ros2/rmw/issues/196>`_)
* Code style only: wrap after open parenthesis if not in one line (`#195 <https://github.com/ros2/rmw/issues/195>`_)
* Update development version after merging `#186 <https://github.com/ros2/rmw/issues/186>`_ (`#194 <https://github.com/ros2/rmw/issues/194>`_)
* Adding required structs and methods to get a list  of publishers or subscribers with their respective qos (`#186 <https://github.com/ros2/rmw/issues/186>`_)
* Contributors: Alejandro Hernández Cordero, Dirk Thomas, Ingo Lütkebohle, Ivan Santiago Paunovic, Jaison Titus, Karsten Knese, Miaofei Mei, Michael Carroll, Mikael Arguedas, Shane Loretz, William Woodall, Stephen Brawner, Tomoya Fujita

0.8.1 (2019-10-23)
------------------
* Use return_loaned_message_from (`#192 <https://github.com/ros2/rmw/issues/192>`_)
* Add function to enable localhost communication only from env var (`#190 <https://github.com/ros2/rmw/issues/190>`_)
* Zero copy api (`#185 <https://github.com/ros2/rmw/issues/185>`_)
* Add call to bump dev version to the upcoming version 0.8.1 (`#191 <https://github.com/ros2/rmw/issues/191>`_)
* Add pub/sub option structures to support rmw specific payload feature (`#187 <https://github.com/ros2/rmw/issues/187>`_)
* Contributors: Brian Marchi, Dirk Thomas, Karsten Knese, William Woodall

0.8.0 (2019-09-24)
------------------
* Added specific return type for non existent node (`#182 <https://github.com/ros2/rmw/issues/182>`_)
* Added function for getting clients by node (`#179 <https://github.com/ros2/rmw/issues/179>`_)
* Added get_actual_qos() feature to subscriptions (`#177 <https://github.com/ros2/rmw/issues/177>`_)
* Added ``RMW_QOS_POLICY_LIVELINESS_UNKNOWN`` enum (`#175 <https://github.com/ros2/rmw/issues/175>`_)
* Contributors: Jacob Perron, M. M, ivanpauno

0.7.1 (2019-05-08)
------------------

* Implement QoS: liveliness, deadline, lifespan (`#171 <https://github.com/ros2/rmw/issues/171>`_)
* Rmw preallocate (`#160 <https://github.com/ros2/rmw/issues/160>`_)
* Add new QoS policy data types to rmw (`#173 <https://github.com/ros2/rmw/issues/173>`_)
* Contributors: M. M, Michael Carroll, Ross Desmond

0.7.0 (2019-04-13)
------------------
* Add function to get publisher actual qos settings (`#169 <https://github.com/ros2/rmw/issues/169>`_)
* fix checking boolean variable which might contain a string (`#165 <https://github.com/ros2/rmw/issues/165>`_)
* change parameter events to use KEEP_LAST (`#162 <https://github.com/ros2/rmw/issues/162>`_)
* Fix typo error (`#164 <https://github.com/ros2/rmw/issues/164>`_)
* pass context to wait set, and provide fini function for context (`#163 <https://github.com/ros2/rmw/issues/163>`_)
* minor notes explaining rmw_qos_profile_t (`#156 <https://github.com/ros2/rmw/issues/156>`_)
* Contributors: Dirk Thomas, Lalit Begani, Mike Lautman, William Woodall, ivanpauno

0.6.1 (2018-12-06)
------------------
* Add node graph functions (`#158 <https://github.com/ros2/rmw/issues/158>`_)
* refactor init to allow options to be passed and to not be global (`#154 <https://github.com/ros2/rmw/issues/154>`_)
* Methods to retrieve matched counts on publisher and subscriber (`#155 <https://github.com/ros2/rmw/issues/155>`_)
* use uint8_t instead of char for serialized message (`#161 <https://github.com/ros2/rmw/issues/161>`_)
* Contributors: Karsten Knese, Michael Carroll, Ross Desmond, William Woodall

0.6.0 (2018-11-16)
------------------
* use new error handling API (`#153 <https://github.com/ros2/rmw/issues/153>`_)
* Add semicolons to RCLCPP and RCUTILS macros. (`#150 <https://github.com/ros2/rmw/issues/150>`_)
* Include node namespaces in get_node_names() (`#148 <https://github.com/ros2/rmw/issues/148>`_)
* add missing doc for parameter (`#149 <https://github.com/ros2/rmw/issues/149>`_)
* rcutils_serialized_message -> rcutils_char_array (`#146 <https://github.com/ros2/rmw/issues/146>`_)
* rmw serialized to rcutils serialized (`#145 <https://github.com/ros2/rmw/issues/145>`_)
* rcutil -> rcutils (`#147 <https://github.com/ros2/rmw/issues/147>`_)
* get serialization format (`#143 <https://github.com/ros2/rmw/issues/143>`_)
* Contributors: Chris Lalancette, Karsten Knese, Michael Carroll, Mikael Arguedas, William Woodall

0.5.0 (2018-06-23)
------------------
* use rcutils allocator in allocators.c to avoid direct use of malloc/free (`#140 <https://github.com/ros2/rmw/issues/140>`_)
* check the return value of rcutils_snprintf (`#138 <https://github.com/ros2/rmw/issues/138>`_)
* _raw function (`#125 <https://github.com/ros2/rmw/issues/125>`_)
* Merge pull request `#137 <https://github.com/ros2/rmw/issues/137>`_ from ros2/misra_fixup
* Change #if to #ifdef.
* add function to parse key-value user_data (`#132 <https://github.com/ros2/rmw/issues/132>`_)
* Add validation functions accepting string length (`#135 <https://github.com/ros2/rmw/issues/135>`_)
* Clarify that NULL blocks forever in rmw_wait (`#134 <https://github.com/ros2/rmw/issues/134>`_)
* Enable setting log levels in DDS implementation (`#124 <https://github.com/ros2/rmw/issues/124>`_)
* Optimize namespace node and topic validation (`#130 <https://github.com/ros2/rmw/issues/130>`_)
* Contributors: Dirk Thomas, Ethan Gao, Karsten Knese, Michael Carroll, Shane Loretz, Sriram Raghunathan, William Woodall
