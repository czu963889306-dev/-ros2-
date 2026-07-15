Change history
==============

2.2.1 (2021-04-06)
------------------
* Change index.ros.org -> docs.ros.org (`#21 <https://github.com/ros2/unique_identifier_msgs/issues/21>`_)
* Contributors: Chris Lalancette

2.2.0 (2020-12-08)
------------------
* Update QD to QL 1 (`#17 <https://github.com/ros2/unique_identifier_msgs/issues/17>`_)
* Update Quality Declaration to QL2. (`#15 <https://github.com/ros2/unique_identifier_msgs/issues/15>`_)
* Update Quality level to level 3 (`#13 <https://github.com/ros2/unique_identifier_msgs/issues/13>`_)
* Add Security Vulnerability Policy pointing to REP-2006. (`#11 <https://github.com/ros2/unique_identifier_msgs/issues/11>`_)
* Contributors: Chris Lalancette, Michel Hidalgo, Stephen Brawner, brawner

2.1.2 (2020-05-26)
------------------
* Change Contributing.md to match linter template (`#10 <https://github.com/ros2/unique_identifier_msgs/issues/10>`_)
* Add README and QUALITY_DECLARATION (`#8 <https://github.com/ros2/unique_identifier_msgs/issues/8>`_)
* Contributors: Jorge Perez, brawner

2.1.1 (2020-04-25)
------------------
* Enable linter tests on unique_identifier_msgs (`#5 <https://github.com/ros2/unique_identifier_msgs/issues/5>`_)
* Contributors: Jorge Perez

2.1.0 (2019-04-14)
------------------
* Added mapping rule for ros1_bridge (`#3 <https://github.com/ros2/unique_identifier_msgs/issues/3>`_)
* Contributors: Paul Bovbel

2.0.0 (2018-11-21)
------------------

* Migrating from its old (language mixed) repository

  Old Repo: https://github.com/ros-geographic-info/unique_identifier

  Name Change: uuid_msgs -> unique_identifier_msgs

  The name change is to line it up with the names of the langauge specific wrappers that will exist around this message artifact.
  As this exactly describes the semantic meaning of the message (a UUID is defined as exactly a 128 bit unique identifier).
* Dropped std_msgs dependency
* Update maintainer

1.1.0 (2018-11-01)
------------------
* Renamed (match c++/python package names): uuid_msgs -> unique_identifier_msgs
* Renamed (exact semantic): UniqueID.msg -> UUID.msg

1.0.4 (2014-04-30)
------------------

 * add architecture independent tag

1.0.1 (2013-03-25)
-------------------

 * Hydro release update.

1.0.0 (2013-03-18)
-------------------

 * Hydro release.
 * Convert to catkin (`#1`_).

0.9.0 (2013-01-03)
------------------

 * Initial release to Groovy.

0.8.0 (2012-07-19)
------------------

 * Initial release to Fuerte.
 * Provides uuid_msgs/UniqueID message.

.. _`#1`: https://github.com/ros-geographic-info/unique_identifier/issues/1
