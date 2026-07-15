^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package rclc
^^^^^^^^^^^^^^^^^^^^^^^^^^

4.0.2 (2023-03-22)
------------------
* Drop build dependency on std_msgs (backport #314) (#315)
* Updated ros-tooling/setup-ros@0.4.2 and ros-tooling/action-ros-ci@0.2.7 (#318) (#320)
* Removed build status for Galactic in README (EOL November 2022) (#321) (#322)
* Update documentation about number_of_handles (#326) (#327)

4.0.1 (2022-07-20)
------------------
* updated documentation bloom build status table (#291) (#292)
* updated os-version to ubuntu-22.04 (#295)
* [rolling] updated ros-tooling versions (backport #289) (#297)
* improved doxygen-generated API documentation (#301) (#302)

4.0.0 (2022-04-28)
------------------
* updated version for Humble release

3.0.8 (2022-04-14)
------------------
* Remove duplicate typedefs. (#249)
* Add rmw dependencies due to EventsExecutor PR in rcl (#255)
* Fix action client & server deallocation (#257)
* updated documentation: build status for Rolling (#266)
* Update action client goal callback signature (#282)
* Upgrade parameters (#274)

3.0.7 (2022-02-17)
------------------
* Fix enum naming for avoid collision (#242)
* Added dependency for package performance-test-fixture (#245)

3.0.6 (2022-01-25)
------------------
* executor ignore canceled timers (#220)
* uddated documentation README.md (#229)
* resolved error in unit test see issue #230 (#231)
* Add thread dependency to examples (Rolling) (#237) (resolves in this package only cpplint errors)

3.0.5 (2021-11-23)
------------------
* Fix data_available reset for timer (backport #215) (#217)

3.0.4 (2021-11-17)
------------------
* Ignoring unsuccessful SERVICE_TAKE (#175)
* Add rclc_parameter Quality Declaration (#144)
* use-ros2-testing (#185)
* Fix: printf in executor spin (#195)
* Fix init options handling (#202) (#205)
* Remove init options from support (#203)
* RCLC Actions Implementation (#170)
* Add rcl_action as build export dependency (#211)


3.0.3 (2021-07-28)
------------------
* Checking for valid ROS context in spin_some
* Refactoring executor (removing callback_type)
* Fixing codecov config

3.0.2 (2021-07-26)
------------------
* Updated codecov to ignore test folders
* Updated bloom release status table

3.0.1 (2021-07-17)
------------------
* Added rclc_parameter package
* Added quality of service entity creation API
* Added executor prepare API
* Added support for removing subscription from executor
* Added support for subscription with context
* Added quality declaration statement
* Updated compatability function for sleep
* Removed duplicate NOTICE files

2.0.0 (2021-04-23)
------------------
* Added codecov support
* New API of rcl_lifecycle in Rolling required major version bump

1.0.1 (2021-03-29)
------------------
* Windows port
* Compatibility sleep function (Windows, POSIX-OS)
* Fixed RCL lifecycle API change for Rolling

1.0.0 (2021-03-04)
------------------
* Service callbacks with context
* Fixed minor issues unit tests
* Upgraded setup_ros action (ci jobs)
* Removed Eloquent from ci jobs

0.1.7 (2021-01-20)
------------------
* Corrected corrupted changelog

0.1.6 (2021-01-20)
------------------
* Fixed issues due to Github Action timing delays on cloud build
* Fixed missing package dependency in Github Action for Eloquent

0.1.5 (2020-12-11)
------------------
* Added support for services,clients and guard_conditions to rclc executor
* Added table for bloom release status in README file

0.1.4 (2020-11-25)
------------------
* Fixed error in bloom release

0.1.3 (2020-11-23)
------------------
* Added rclc_lifecycle package
* Change maintainer information
* Minor fixes, updated unit tests

0.1.2 (2020-05-19)
------------------
* Fixed compiler errors for bloom release

0.1.1 (2020-05-14)
------------------
* Initial release