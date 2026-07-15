^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rosidl_parser
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

3.1.9 (2026-06-04)
------------------

3.1.8 (2025-12-09)
------------------

3.1.7 (2025-07-16)
------------------

3.1.6 (2024-11-25)
------------------

3.1.5 (2023-07-18)
------------------

3.1.4 (2022-11-07)
------------------
* Always include whitespace in string literals (`#688 <https://github.com/ros2/rosidl/issues/688>`_) (`#689 <https://github.com/ros2/rosidl/issues/689>`_)
* Contributors: mergify[bot]

3.1.3 (2022-04-08)
------------------

3.1.2 (2022-04-05)
------------------

3.1.1 (2022-03-28)
------------------

3.1.0 (2022-03-01)
------------------
* Set maybe_placeholders to False for lark 1.+ compatibility (`#664 <https://github.com/ros2/rosidl/issues/664>`_)
* Generate documentation in generated C header files based on ROS interfaces comments (`#593 <https://github.com/ros2/rosidl/issues/593>`_)
* Contributors: Ivan Santiago Paunovic, Shane Loretz

3.0.1 (2022-01-13)
------------------
* Pass comments in ros interface constants to the .idl generated files (`#630 <https://github.com/ros2/rosidl/issues/630>`_)
* Contributors: Ivan Santiago Paunovic

3.0.0 (2021-11-05)
------------------
* Update package maintainers (`#624 <https://github.com/ros2/rosidl/issues/624>`_)
* Contributors: Michel Hidalgo

2.5.0 (2021-08-10)
------------------

2.4.0 (2021-07-12)
------------------

2.3.0 (2021-06-11)
------------------
* Fix escaping in string literals (`#595 <https://github.com/ros2/rosidl/issues/595>`_)
* Contributors: Ivan Santiago Paunovic

2.2.1 (2021-04-06)
------------------

2.2.0 (2021-03-18)
------------------

2.1.0 (2021-03-09)
------------------

2.0.3 (2021-01-25)
------------------

2.0.2 (2020-12-08)
------------------
* Update and add package.xml descriptions to README (`#553 <https://github.com/ros2/rosidl/issues/553>`_)
* Finish support for fixed-point literals.
* Fix parsing of small floats.
* Update the maintainers of this repository. (`#536 <https://github.com/ros2/rosidl/issues/536>`_)
* Contributors: Chris Lalancette, Shane Loretz

2.0.1 (2020-09-28)
------------------

2.0.0 (2020-09-24)
------------------

1.1.0 (2020-08-17)
------------------
* Allow zero length string constants (`#507 <https://github.com/ros2/rosidl/issues/507>`_)
* Add pytest.ini so tests succeed locally (`#502 <https://github.com/ros2/rosidl/issues/502>`_)
* Contributors: Chris Lalancette, Dirk Thomas

1.0.1 (2020-06-03)
------------------

1.0.0 (2020-05-22)
------------------

0.9.2 (2020-05-19)
------------------

0.9.1 (2020-05-08)
------------------

0.9.0 (2020-04-24)
------------------
* Use f-string (`#436 <https://github.com/ros2/rosidl/issues/436>`_)
* Contributors: Dirk Thomas

0.8.2 (2020-01-17)
------------------
* Allow 'get_const_expr_value' to parse either literals or scoped_names… (`#430 <https://github.com/ros2/rosidl/issues/430>`_)
* Use imperative mood in constructor docstring. (`#425 <https://github.com/ros2/rosidl/issues/425>`_)
* Contributors: Steven! Ragnarök, kylemarcey

0.8.1 (2019-10-23)
------------------

0.8.0 (2019-09-24)
------------------
* support adjacent string literals, use them for multi-line comments (`#410 <https://github.com/ros2/rosidl/issues/410>`_)
* fix parsing empty string literal (`#409 <https://github.com/ros2/rosidl/issues/409>`_)
* add constant for member name in empty structs (`#389 <https://github.com/ros2/rosidl/issues/389>`_)
* Contributors: Dirk Thomas
