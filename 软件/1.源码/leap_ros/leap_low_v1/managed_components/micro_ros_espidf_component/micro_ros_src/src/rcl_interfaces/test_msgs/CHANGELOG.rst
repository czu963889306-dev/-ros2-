^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package test_msgs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1.2.3 (2026-05-13)
------------------

1.2.2 (2025-07-16)
------------------

1.2.1 (2022-11-07)
------------------

1.2.0 (2022-03-01)
------------------
* Install headers to include/${PROJECT_NAME} and Depend on rosidl_typesupport\_* targets directly (`#133 <https://github.com/ros2/rcl_interfaces/issues/133>`_)
* Update maintainers to Chris Lalancette (`#130 <https://github.com/ros2/rcl_interfaces/issues/130>`_)
* Contributors: Audrow Nash, Shane Loretz

1.1.0 (2021-08-06)
------------------
* Add test fixures for BoundedPlainSequences (`#125 <https://github.com/ros2/rcl_interfaces/issues/125>`_)
* Added BoundedPlainSequences to test_msgs (`#123 <https://github.com/ros2/rcl_interfaces/issues/123>`_)
* Contributors: Miguel Company, Shane Loretz

1.0.3 (2021-04-06)
------------------

1.0.2 (2021-02-22)
------------------
* Update package maintainers. (`#112 <https://github.com/ros2/rcl_interfaces/issues/112>`_)
* Contributors: Chris Lalancette

1.0.1 (2020-06-29)
------------------

1.0.0 (2020-05-26)
------------------

0.9.0 (2020-04-25)
------------------
* Remove unused local variable (`#96 <https://github.com/ros2/rcl_interfaces/issues/96>`_)
* Contributors: Dirk Thomas

0.8.0 (2019-09-26)
------------------
* Add test fixtures for Arrays.srv (`#84 <https://github.com/ros2/rcl_interfaces/issues/84>`_)
* use idl files (if any) (`#82 <https://github.com/ros2/rcl_interfaces/issues/82>`_)
* Contributors: Dirk Thomas, Jacob Perron

0.7.4 (2019-05-29)
------------------
* Remove unnecessary action dependencies from CMakeLists.txt (`#81 <https://github.com/ros2/rcl_interfaces/issues/81>`_)
* Contributors: Jacob Perron

0.7.3 (2019-05-20)
------------------
* add test cases for UTF-8 encoded strings (`#80 <https://github.com/ros2/rcl_interfaces/issues/80>`_)
* Add UTF-8 BOM to message_fixtures.hpp file (`#79 <https://github.com/ros2/rcl_interfaces/issues/79>`_)
* Contributors: Dirk Thomas, Michel Hidalgo

0.7.2 (2019-05-08)
------------------
* add WString tests (`#78 <https://github.com/ros2/rcl_interfaces/issues/78>`_)
* Build interfaces from the package test_interface_files (`#77 <https://github.com/ros2/rcl_interfaces/issues/77>`_)
* Contributors: Dirk Thomas, Jacob Perron

0.7.1 (2019-04-26)
------------------

0.7.0 (2019-04-14)
------------------
* update test fixtures for char type (`#64 <https://github.com/ros2/rcl_interfaces/issues/64>`_)
* Add action with nested messages (`#62 <https://github.com/ros2/rcl_interfaces/issues/62>`_)
* Contributors: Dirk Thomas, Shane Loretz

0.6.2 (2019-01-11)
------------------
* Fix errors from uncrustify v0.68 (`#57 <https://github.com/ros2/rcl_interfaces/issues/57>`_)
* Contributors: Jacob Perron

0.6.1 (2018-12-06)
------------------
* remove trailing slash (`#55 <https://github.com/ros2/rcl_interfaces/issues/55>`_)
* Change UUID type in action_msgs (`#54 <https://github.com/ros2/rcl_interfaces/issues/54>`_)
* add new test messages for arrays of arrays (`#52 <https://github.com/ros2/rcl_interfaces/issues/52>`_)
* Contributors: Alexis Pojomovsky, Dirk Thomas, Karsten Knese

0.6.0 (2018-11-16)
------------------
* Generate action type support / Remove hardcoded action type support (`#48 <https://github.com/ros2/rcl_interfaces/issues/48>`_)
* Hardcoded typesupport for Fibonacci action (`#47 <https://github.com/ros2/rcl_interfaces/issues/47>`_)
* Add message fixture for DynamicArrayStaticArrayPrimitivesNested (`#45 <https://github.com/ros2/rcl_interfaces/issues/45>`_)
* Add test message for dynamic array of static arrays (`#43 <https://github.com/ros2/rcl_interfaces/issues/43>`_)
* use add_compile_options instead of setting only cxx flags
* Only install the Python messages if PYTHON_INSTALL_DIR is defined (`#37 <https://github.com/ros2/rcl_interfaces/issues/37>`_)
* Contributors: Esteve Fernandez, Jacob Perron, Mikael Arguedas, Shane Loretz

0.5.0 (2018-06-24)
------------------

0.4.0 (2017-12-08)
------------------
* member of rosidl_interfaces_packages group (`#28 <https://github.com/ros2/rcl_interfaces/issues/28>`_)
* Merge pull request `#22 <https://github.com/ros2/rcl_interfaces/issues/22>`_ from ros2/remove_indent_off
* Contributors: Dirk Thomas, Mikael Arguedas, Steven! Ragnarök, William Woodall
