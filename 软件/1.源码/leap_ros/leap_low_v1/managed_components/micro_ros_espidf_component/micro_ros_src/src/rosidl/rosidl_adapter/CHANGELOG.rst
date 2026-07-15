^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rosidl_adapter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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

3.1.3 (2022-04-08)
------------------

3.1.2 (2022-04-05)
------------------

3.1.1 (2022-03-28)
------------------

3.1.0 (2022-03-01)
------------------
* rename nested loop index (`#643 <https://github.com/ros2/rosidl/issues/643>`_)
* Contributors: ibnHatab

3.0.1 (2022-01-13)
------------------
* Fix how comments in action interfaces are processed (`#632 <https://github.com/ros2/rosidl/issues/632>`_)
* Pass comments in ros interface constants to the .idl generated files (`#630 <https://github.com/ros2/rosidl/issues/630>`_)
* Contributors: Ivan Santiago Paunovic

3.0.0 (2021-11-05)
------------------
* Update package maintainers (`#624 <https://github.com/ros2/rosidl/issues/624>`_)
* Make rosidl packages use FindPython3 instead of FindPythonInterp (`#612 <https://github.com/ros2/rosidl/issues/612>`_)
* Contributors: Michel Hidalgo, Shane Loretz

2.5.0 (2021-08-10)
------------------

2.4.0 (2021-07-12)
------------------

2.3.0 (2021-06-11)
------------------
* Fix escaping in string literals (`#595 <https://github.com/ros2/rosidl/issues/595>`_)
* Ignore multiple ``#`` characters and dedent comments (`#594 <https://github.com/ros2/rosidl/issues/594>`_)
* Contributors: Ivan Santiago Paunovic

2.2.1 (2021-04-06)
------------------

2.2.0 (2021-03-18)
------------------
* Expose .msg/.srv/.action to .idl conversion via rosidl translate CLI (`#576 <https://github.com/ros2/rosidl/issues/576>`_)
* Contributors: Michel Hidalgo

2.1.0 (2021-03-09)
------------------
* Support hex constants in msg files (`#559 <https://github.com/ros2/rosidl/issues/559>`_)
* Contributors: Dereck Wonnacott

2.0.3 (2021-01-25)
------------------
* Treat \t as whitespace (`#557 <https://github.com/ros2/rosidl/issues/557>`_)
* Contributors: Dereck Wonnacott

2.0.2 (2020-12-08)
------------------
* Update the maintainers of this repository. (`#536 <https://github.com/ros2/rosidl/issues/536>`_)
* Contributors: Chris Lalancette

2.0.1 (2020-09-28)
------------------

2.0.0 (2020-09-24)
------------------

1.1.0 (2020-08-17)
------------------
* Refactor regex for valid package/field names (`#508 <https://github.com/ros2/rosidl/issues/508>`_)
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

0.8.1 (2019-10-23)
------------------

0.8.0 (2019-09-24)
------------------
* Improve parser error messages (`#415 <https://github.com/ros2/rosidl/issues/415>`_)
* support adjacent string literals, use them for multi-line comments (`#410 <https://github.com/ros2/rosidl/issues/410>`_)
* avoid zero length comment when the comment only contains a unit (`#411 <https://github.com/ros2/rosidl/issues/411>`_)
* use latin-1 encoding when reading/writing .idl files, prepend BOM to generated C/C++ files when necessary (`#391 <https://github.com/ros2/rosidl/issues/391>`_)
* fix error msg asserts due to change in pytest (`#393 <https://github.com/ros2/rosidl/issues/393>`_)
* open interface files with utf-8 encoding (`#390 <https://github.com/ros2/rosidl/issues/390>`_)
* Contributors: Dirk Thomas, Jacob Perron, William Woodall
