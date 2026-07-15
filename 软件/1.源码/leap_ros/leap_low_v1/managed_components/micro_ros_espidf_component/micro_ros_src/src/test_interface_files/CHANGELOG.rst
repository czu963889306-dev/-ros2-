^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package test_interface_files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.9.1 (2022-04-05)
------------------
* Revert "Update package.xml (`#18 <https://github.com/ros2/test_interface_files/issues/18>`_)" (`#19 <https://github.com/ros2/test_interface_files/issues/19>`_)
* Update package.xml (`#18 <https://github.com/ros2/test_interface_files/issues/18>`_)
* Update maintainers to Audrow Nash (`#17 <https://github.com/ros2/test_interface_files/issues/17>`_)
* Contributors: Audrow Nash, Chris Lalancette, Nikolai Morin

0.9.0 (2021-08-06)
------------------
* Added BoundedPlainSequences messages (`#14 <https://github.com/ros2/test_interface_files/issues/14>`_)
* Contributors: Miguel Company

0.8.1 (2021-03-18)
------------------
* Update maintainer (`#13 <https://github.com/ros2/test_interface_files/issues/13>`_)
* Contributors: Jacob Perron

0.8.0 (2019-09-24)
------------------
* remove absolute paths from installed CMake code (`#9 <https://github.com/ros2/test_interface_files/issues/9>`_)
* add wstring default values with non-ASCII characters (`#8 <https://github.com/ros2/test_interface_files/issues/8>`_)
* Add Arrays.srv (`#7 <https://github.com/ros2/test_interface_files/issues/7>`_)
* add IdlOnlyTypes - even though they are commented out atm (`#6 <https://github.com/ros2/test_interface_files/issues/6>`_)
* Contributors: Dirk Thomas, Jacob Perron

0.7.0 (2019-05-08)
------------------
* add WStrings message (`#4 <https://github.com/ros2/test_interface_files/issues/4>`_)
* Refactor interface messages (`#3 <https://github.com/ros2/test_interface_files/pull/3>`_)
    * Rename messages towards consolidation
    * Refactor message fields
        * Add fields to cover test cases
        * Move string fields to Strings.msg
        * Comment out fields that have issues to be resolved
    * Remove old interface files
* Rename messages to align with IDL terminology
* rename package, remove all dependencies, and only install the interface files
* move package content into the root of the repo
* Merge: test_msgs package from ros2/rcl_interfaces
  The commit messages have been rewritten to reference the tickets in the original repository
