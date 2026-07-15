^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rmw_implementation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

2.8.5 (2025-07-16)
------------------

2.8.4 (2024-07-26)
------------------

2.8.3 (2024-05-15)
------------------
* Update quality declaration document (`#225 <https://github.com/ros2/rmw_implementation/issues/225>`_) (`#228 <https://github.com/ros2/rmw_implementation/issues/228>`_)
* Contributors: mergify[bot]

2.8.2 (2023-01-10)
------------------
* Build-time RMW selection does not need ament_index_cpp (`#210 <https://github.com/ros2/rmw_implementation/issues/210>`_) (`#211 <https://github.com/ros2/rmw_implementation/issues/211>`_)
* Contributors: mergify[bot]

2.8.1 (2022-03-28)
------------------
* add content-filtered-topic interfaces (`#181 <https://github.com/ros2/rmw_implementation/issues/181>`_)
* Add rmw_feature_supported() (`#204 <https://github.com/ros2/rmw_implementation/issues/204>`_)
* Contributors: Chen Lihui, Ivan Santiago Paunovic

2.8.0 (2022-03-01)
------------------
* Add EventsExecutor (`#161 <https://github.com/ros2/rmw_implementation/issues/161>`_)
* Contributors: iRobot ROS

2.7.1 (2022-01-14)
------------------
* Fix relative path include syntax for cpplint (`#203 <https://github.com/ros2/rmw_implementation/issues/203>`_)
* Support and prefer exported targets from rmw implementations (`#201 <https://github.com/ros2/rmw_implementation/issues/201>`_)
* Contributors: Jacob Perron, Shane Loretz

2.7.0 (2021-11-19)
------------------
* Add client/service QoS getters. (`#196 <https://github.com/ros2/rmw_implementation/issues/196>`_)
* Update maintainers to Audrow Nash and Michael Carroll. (`#199 <https://github.com/ros2/rmw_implementation/issues/199>`_)
* Contributors: Audrow Nash, mauropasse

2.6.1 (2021-11-18)
------------------
* Fix renamed `rcpputils` header (`#198 <https://github.com/ros2/rmw_implementation/issues/198>`_)
* Fix rmw_implementation generated documentation (`#197 <https://github.com/ros2/rmw_implementation/issues/197>`_)
* Contributors: Abrar Rahman Protyasha, Michel Hidalgo

2.6.0 (2021-08-09)
------------------
* Add rmw_publisher_wait_for_all_acked. (`#188 <https://github.com/ros2/rmw_implementation/issues/188>`_)
* Contributors: Barry Xu

2.5.0 (2021-05-05)
------------------
* Attempt to load any available RMW implementation. (`#189 <https://github.com/ros2/rmw_implementation/issues/189>`_)
* Update includes after rcutils/get_env.h deprecation (`#190 <https://github.com/ros2/rmw_implementation/issues/190>`_)
* Contributors: Chris Lalancette, Christophe Bedard

2.4.1 (2021-04-16)
------------------

2.4.0 (2021-04-06)
------------------
* Unique network flows (`#170 <https://github.com/ros2/rmw_implementation/issues/170>`_)
* updating quality declaration links (re: `ros2/docs.ros2.org#52 <https://github.com/ros2/docs.ros2.org/issues/52>`_) (`#185 <https://github.com/ros2/rmw_implementation/issues/185>`_)
* Contributors: Ananya Muddukrishna, shonigmann

2.3.0 (2021-03-25)
------------------
* Remove rmw_connext_cpp. (`#183 <https://github.com/ros2/rmw_implementation/issues/183>`_)
* Add support for rmw_connextdds (`#182 <https://github.com/ros2/rmw_implementation/issues/182>`_)
* Contributors: Andrea Sorbini, Chris Lalancette

2.2.0 (2021-03-08)
------------------
* Add function for checking QoS profile compatibility (`#180 <https://github.com/ros2/rmw_implementation/issues/180>`_)
* Shorten some excessively long lines of CMake (`#179 <https://github.com/ros2/rmw_implementation/issues/179>`_)
* Add rmw_fastrtps_dynamic_cpp to the explicit group deps (`#177 <https://github.com/ros2/rmw_implementation/issues/177>`_)
* Contributors: Jacob Perron, Scott K Logan

2.1.2 (2021-01-29)
------------------
* Accept any RMW implementation, not just the default (`#172 <https://github.com/ros2/rmw_implementation/issues/172>`_)
* Contributors: Scott K Logan

2.1.1 (2021-01-25)
------------------
* Defer path resolution of rmw implementation libraries to dynamic linker. (`#169 <https://github.com/ros2/rmw_implementation/issues/169>`_)
* Contributors: Michel Hidalgo

2.1.0 (2020-12-10)
------------------
* Update QD to QL 1 (`#166 <https://github.com/ros2/rmw_implementation/issues/166>`_)
* Fix up C functions to never throw. (`#149 <https://github.com/ros2/rmw_implementation/issues/149>`_)
* Restored Dirk as author (`#155 <https://github.com/ros2/rmw_implementation/issues/155>`_)
* Update maintainers (`#154 <https://github.com/ros2/rmw_implementation/issues/154>`_)
* Updated performance QD section (`#153 <https://github.com/ros2/rmw_implementation/issues/153>`_)
* Update Quality Declaration to QL2. (`#151 <https://github.com/ros2/rmw_implementation/issues/151>`_)
* Add nominal test for symbol prefetch() and unload. (`#145 <https://github.com/ros2/rmw_implementation/issues/145>`_)
* Added benchmark test to rmw_implementation (`#127 <https://github.com/ros2/rmw_implementation/issues/127>`_)
* Test load and lookup functionality. (`#135 <https://github.com/ros2/rmw_implementation/issues/135>`_)
* Contributors: Alejandro Hernández Cordero, Michel Hidalgo, Stephen Brawner

2.0.0 (2020-07-08)
------------------
* Remove domain_id and localhost_only from node API (`#114 <https://github.com/ros2/rmw_implementation/issues/114>`_)
* Move the quality declaration into the rmw_implementation subdirectory. (`#111 <https://github.com/ros2/rmw_implementation/issues/111>`_)
* Contributors: Chris Lalancette, Ivan Santiago Paunovic

1.0.0 (2020-05-12)
------------------
* Remove MANUAL_BY_NODE liveliness API (`#101 <https://github.com/ros2/rmw_implementation/issues/101>`_)
* Contributors: Ivan Santiago Paunovic

0.9.0 (2020-04-25)
------------------
* Rename rosidl_message_bounds_t (`#98 <https://github.com/ros2/rmw_implementation/issues/98>`_)
* Adapt interfaces for service timestamps (`#96 <https://github.com/ros2/rmw_implementation/issues/96>`_)
* Add take_sequence to RMW API (`#93 <https://github.com/ros2/rmw_implementation/issues/93>`_)
* Export targets in addition to include directories / libraries (`#97 <https://github.com/ros2/rmw_implementation/issues/97>`_)
* Removed ament_cmake_python from package.xml (`#95 <https://github.com/ros2/rmw_implementation/issues/95>`_)
* Using get_env_var from rcpputils (`#94 <https://github.com/ros2/rmw_implementation/issues/94>`_)
* security-context -> enclave (`#91 <https://github.com/ros2/rmw_implementation/issues/91>`_)
* Fix dependency on rmw_implementation_cmake (`#92 <https://github.com/ros2/rmw_implementation/issues/92>`_)
* Removed poco dependency (`#87 <https://github.com/ros2/rmw_implementation/issues/87>`_)
* Use one participant per context API changes (`#77 <https://github.com/ros2/rmw_implementation/issues/77>`_)
* Add rmw\_*_event_init() functions to rmw_implementation (`#88 <https://github.com/ros2/rmw_implementation/issues/88>`_)
* Moved rmw_implementation_cmake from depend to build_depend (`#82 <https://github.com/ros2/rmw_implementation/issues/82>`_)
* Removed python code (`#85 <https://github.com/ros2/rmw_implementation/issues/85>`_)
* Remove OpenSplice dependency (`#79 <https://github.com/ros2/rmw_implementation/issues/79>`_)
* Code style only: wrap after open parenthesis if not in one line (`#78 <https://github.com/ros2/rmw_implementation/issues/78>`_)
* Depend on rcpputils for find_library (`#57 <https://github.com/ros2/rmw_implementation/issues/57>`_)
* Added functions to get qos policies for publishers and subscribers to a topic (`#72 <https://github.com/ros2/rmw_implementation/issues/72>`_)
* Contributors: Alejandro Hernández Cordero, Dirk Thomas, Eric Cousineau, Ingo Lütkebohle, Ivan Santiago Paunovic, Jacob Perron, Jaison Titus, Miaofei Mei, Michael Carroll, Mikael Arguedas

0.8.2 (2019-11-13)
------------------
* Add support for Cyclone DDS. (`#71 <https://github.com/ros2/rmw_implementation/issues/71>`_)
* Contributors: Ruffin

0.8.1 (2019-10-23)
------------------
* use return_loaned_message_from (`#76 <https://github.com/ros2/rmw_implementation/issues/76>`_)
* Add localhost boolean parameter to create node function (`#75 <https://github.com/ros2/rmw_implementation/issues/75>`_)
* Zero copy api (`#69 <https://github.com/ros2/rmw_implementation/issues/69>`_)
* Add Python API for RMW implementation lookups (`#73 <https://github.com/ros2/rmw_implementation/issues/73>`_)
* update signature for added pub/sub options (`#74 <https://github.com/ros2/rmw_implementation/issues/74>`_)
* remove unneeded line from CMakeLists (`#70 <https://github.com/ros2/rmw_implementation/issues/70>`_)
* Make middleware selection more independent of build-time package availability (`#67 <https://github.com/ros2/rmw_implementation/issues/67>`_)
* Contributors: Brian Marchi, Dan Rose, Karsten Knese, Michel Hidalgo, William Woodall

0.8.0 (2019-09-25)
------------------
* Add function for getting clients by node (`#62 <https://github.com/ros2/rmw_implementation/issues/62>`_)
* add get_actual_qos() feature to subscriptions (`#61 <https://github.com/ros2/rmw_implementation/issues/61>`_)
* Contributors: Jacob Perron, M. M

0.7.1 (2019-05-08)
------------------
* add interfaces for rmw_take_event and assert_liveliness (`#60 <https://github.com/ros2/rmw_implementation/issues/60>`_)
* Rmw preallocate (`#51 <https://github.com/ros2/rmw_implementation/issues/51>`_)
* Contributors: Michael Carroll, Nick Burek

0.7.0 (2019-04-13)
------------------
* Add function rmw_get_actual_qos (`#56 <https://github.com/ros2/rmw_implementation/issues/56>`_)
* cmake: Add `RMW_IMPLEMENTATION_FORCE_POCO` (`#59 <https://github.com/ros2/rmw_implementation/issues/59>`_)
* add missing preload of rmw_set_log_severity (`#55 <https://github.com/ros2/rmw_implementation/issues/55>`_)
* Export threading library via extras and not ament_export_libraries to avoid warnings when cross-compiling (`#53 <https://github.com/ros2/rmw_implementation/issues/53>`_)
* pass context to wait set and fini context (`#52 <https://github.com/ros2/rmw_implementation/issues/52>`_)
* Contributors: Dirk Thomas, Eric Cousineau, Esteve Fernandez, William Woodall, ivanpauno

0.6.1 (2018-12-06)
------------------
* Add node graph functions (`#49 <https://github.com/ros2/rmw_implementation/issues/49>`_)
* add new functions (`#50 <https://github.com/ros2/rmw_implementation/issues/50>`_)
* Methods to retrieve matched count on pub/sub. (`#48 <https://github.com/ros2/rmw_implementation/issues/48>`_)
* Contributors: Michael Carroll, Ross Desmond, William Woodall

0.6.0 (2018-11-16)
------------------
* use semicolons after macros (`#47 <https://github.com/ros2/rmw_implementation/issues/47>`_)
* Include node namespaces in get_node_names. (`#46 <https://github.com/ros2/rmw_implementation/issues/46>`_)
* add rmw_get_serialization_format (`#43 <https://github.com/ros2/rmw_implementation/issues/43>`_)
* Contributors: Karsten Knese, Michael Carroll, William Woodall

0.5.1 (2018-07-17)
------------------
* avoid recursive find (`#44 <https://github.com/ros2/rmw_implementation/issues/44>`_)
* Contributors: Dirk Thomas

0.5.0 (2018-06-23)
------------------
* Prepare dependencies for bouncy release. (`#41 <https://github.com/ros2/rmw_implementation/issues/41>`_)
* _raw function (`#31 <https://github.com/ros2/rmw_implementation/issues/31>`_)
* print missing symbol name (`#40 <https://github.com/ros2/rmw_implementation/issues/40>`_)
* Merge pull request `#39 <https://github.com/ros2/rmw_implementation/issues/39>`_ from ros2/misra_fixup
* Change #if to #ifdef
* improve error messages (`#37 <https://github.com/ros2/rmw_implementation/issues/37>`_)
* API to enable log severity setting.  (`#30 <https://github.com/ros2/rmw_implementation/issues/30>`_)
* Contributors: Dirk Thomas, Karsten Knese, Michael Carroll, Sriram Raghunathan, Steven! Ragnarök

0.4.0 (2017-12-08)
------------------
* Merge pull request `#36 <https://github.com/ros2/rmw_implementation/issues/36>`_ from ros2/rename_group
* waitset -> wait_set (`#34 <https://github.com/ros2/rmw_implementation/issues/34>`_)
* Merge pull request `#32 <https://github.com/ros2/rmw_implementation/issues/32>`_ from ros2/rep149
* use format 3
* simplify code relaying all symbols (`#29 <https://github.com/ros2/rmw_implementation/issues/29>`_)
* Merge pull request `#27 <https://github.com/ros2/rmw_implementation/issues/27>`_ from ros2/fix_deadlock
* prefetch all symbols in rmw_init to avoid later race
* make resolved symbol static to significantly reduce the chance of a deadlock
* Merge pull request `#26 <https://github.com/ros2/rmw_implementation/issues/26>`_ from ros2/uncrustify_master
* update style to match latest uncrustify
* Contributors: Dirk Thomas, Karsten Knese, Mikael Arguedas, Morgan Quigley, William Woodall, dhood
