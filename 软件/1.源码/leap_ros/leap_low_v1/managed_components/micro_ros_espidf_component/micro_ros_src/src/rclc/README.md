# The rclc repository
This repository provides the rclc package, which complements the [ROS Client Support Library (rcl)](https://github.com/ros2/rcl/) to make up a complete ROS 2 client library for the C programming language. That is, rclc does not add a new layer of types on top of rcl (like rclcpp and rclpy do) but only provides convenience functions that ease the programming with the rcl types. New types are introduced only for concepts that are missing in rcl, most important an Executor, Lifecycle Node and the Parameter server.

In detail, this repository contains four packages:

- [rclc](rclc/) provides the mentioned convenience functions for creating instances of publishers, subscriptions, nodes, etc. with the corresponding [rcl types](https://github.com/ros2/rcl/tree/master/rcl/include/rcl). Furthermore, it provides the rclc Executor for C, analogously to rclcpp's [Executor class](https://github.com/ros2/rclcpp/blob/master/rclcpp/include/rclcpp/executor.hpp) for C++. A key feature compared to the rclcpp Executor is that it includes features for implementing deterministic timing behavior.
- [rclc_lifecycle](rclc_lifecycle/) introduces an rclc Lifecycle Node, bundling an rcl Node and the [lifecycle state machine](http://design.ros2.org/articles/node_lifecycle.html) from the [rcl_lifecycle package](https://github.com/ros2/rcl/tree/master/rcl_lifecycle).
- [rclc_examples](rclc_examples/) provides small examples for the use of the convenience functions and the rclc Executor, as well as a small example for the use of the rclc Lifecycle Node.
- [rclc_parameter](rclc_parameter/) provides convenience functions for creating parameter server instances with full ROS2 parameters client compatibility.

Technical information on the interfaces and the usage of these packages is given in the README.md files in the corresponding subfolders.

The quality declarations for the packages are avaiable in QUALITY_DECLARATION.md files in the corresponding subfolders.

## Purpose of the project

The software is not ready for production use. It has neither been developed nor tested for a specific use case. However, the license conditions of the applicable Open Source licenses allow you to adapt the software to your needs. Before using it in a safety relevant setting, make sure that the software fulfills your requirements and adjust it according to any applicable safety standards (e.g. ISO 26262).

## Requirements, how to build, test and install

Source your ROS2 `distribution` with `source /opt/ros/distribution/setup.bash`. This will setup the environment variable `$ROS-DISTRO`.
Clone the repository into a ROS2 workspace (e.g. `~/ros2_ws/`) and build the packages using `colcon build` from the [Colcon Command Line Tools](https://colcon.readthedocs.io/en/released/). To test the RCLC package run `colcon test` or if you have multiple repositories in this workspace `colcon test --packages-select rclc`. For correct installation of the `rclc`-package do a `source ~/ros2_ws/install/local_setup.bash`. Then you are ready to run the examples in the `rclc_examples` package.

The following repositories might not be in the default ROS 2 distribution: osrf_testing_tools_cpp and test_msgs. In this case install them manually:

```C
 sudo apt-get install ros-$ROS_DISTRO-osrf-testing-tools-cpp
 sudo apt-get install ros-$ROS_DISTRO-test-msgs
```

## License

rclc is open-sourced under the Apache-2.0 license. See the [LICENSE](LICENSE) file for details.

For a list of other open source components included in rclc, see the file [3rd-party-licenses.txt](3rd-party-licenses.txt).

## Quality assurance

*   Coding style:
    *   The [uncrustify](https://github.com/uncrustify/uncrustify) tool is used to check the coding style.
*   Linters:
    *   The [cpplint](https://github.com/google/styleguide/tree/gh-pages/cpplint) tool is used to detect common flaws and problems in C/C++ code.
    * The [cppcheck](http://cppcheck.sourceforge.net/) tool is used for code analysis.
    *   The CMakeLists.txt is checked with [lint_cmake](https://pypi.org/project/cmakelint/) and the package.xml with [xmllint](http://xmlsoft.org/xmllint.html)
*   Unit tests:
    *   Unit tests based on [gtest](https://github.com/google/googletest) are located in the [rclc/test](rclc/test) folder.

## Known issues/limitations

Please notice the following issues/limitations:

*   The rclc executor is single-threaded. You cannot create nodes in multiple threads and manage the corresponding subscriptions/services/etc. by one executor.

## Bloom Release Status of Code Repository ros2/rclc

Bloom release status of the packages in [github.com/ros2/rclc/](https://github.com/ros2/rclc) for amd64 and arm64 architectures and ROS 2 distributions Foxy, Galactic, Humble and Rolling.

|Package | Release | amd64 | arm64 | 
|:--     |  :--    |  :--  |  :--  | 
| [rclc](https://github.com/ros2/rclc/tree/master/rclc) | Foxy | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Fbin_uF64__rclc__ubuntu_focal_amd64__binary)](https://build.ros2.org/job/Fbin_uF64__rclc__ubuntu_focal_amd64__binary/) | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Fbin_ubv8_uFv8__rclc__ubuntu_focal_arm64__binary)](https://build.ros2.org/job/Fbin_ubv8_uFv8__rclc__ubuntu_focal_arm64__binary/) |
| | Humble | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Hbin_uJ64__rclc__ubuntu_jammy_amd64__binary)](https://build.ros2.org/job/Hbin_uJ64__rclc__ubuntu_jammy_amd64__binary/) | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Hbin_ujv8_uJv8__rclc__ubuntu_jammy_arm64__binary)](https://build.ros2.org/job/Hbin_ujv8_uJv8__rclc__ubuntu_jammy_arm64__binary/) |
| | Iron | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Ipr__rclc__ubuntu_jammy_amd64)](https://build.ros2.org/job/Ipr__rclc__ubuntu_jammy_amd64/) | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Ibin_ujv8_uJv8__rclc__ubuntu_jammy_arm64__binary)](https://build.ros2.org/job/Ibin_ujv8_uJv8__rclc__ubuntu_jammy_arm64__binary/)|
| | Rolling| [![Build Status](https://build.ros2.org/buildStatus/icon?job=Rbin_uJ64__rclc__ubuntu_jammy_amd64__binary)](https://build.ros2.org/job/Rbin_uJ64__rclc__ubuntu_jammy_amd64__binary/) | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Rbin_ujv8_uJv8__rclc__ubuntu_jammy_arm64__binary)](https://build.ros2.org/job/Rbin_ujv8_uJv8__rclc__ubuntu_jammy_arm64__binary/) |
|     |     |   |   |    
| [rclc_examples](https://github.com/ros2/rclc/tree/master/rclc_examples)  
| | Foxy | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Fbin_uF64__rclc_examples__ubuntu_focal_amd64__binary)](https://build.ros2.org/job/Fbin_uF64__rclc_examples__ubuntu_focal_amd64__binary/)  | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Fbin_ubv8_uFv8__rclc_examples__ubuntu_focal_arm64__binary)](https://build.ros2.org/job/Fbin_ubv8_uFv8__rclc_examples__ubuntu_focal_arm64__binary/) | 
| | Humble | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Hbin_uJ64__rclc_examples__ubuntu_jammy_amd64__binary)](https://build.ros2.org/job/Hbin_uJ64__rclc_examples__ubuntu_jammy_amd64__binary/) |  [![Build Status](https://build.ros2.org/buildStatus/icon?job=Hbin_ujv8_uJv8__rclc_examples__ubuntu_jammy_arm64__binary)](https://build.ros2.org/job/Hbin_ujv8_uJv8__rclc_examples__ubuntu_jammy_arm64__binary/) |
| | Iron | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Ibin_uJ64__rclc_examples__ubuntu_jammy_amd64__binary)](https://build.ros2.org/job/Ibin_uJ64__rclc_examples__ubuntu_jammy_amd64__binary/) | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Ibin_ujv8_uJv8__rclc_examples__ubuntu_jammy_arm64__binary)](https://build.ros2.org/job/Ibin_ujv8_uJv8__rclc_examples__ubuntu_jammy_arm64__binary/)  |
| | Rolling| [![Build Status](https://build.ros2.org/buildStatus/icon?job=Rbin_uJ64__rclc_examples__ubuntu_jammy_amd64__binary)](https://build.ros2.org/job/Rbin_uJ64__rclc_examples__ubuntu_jammy_amd64__binary/) |  [![Build Status](https://build.ros2.org/buildStatus/icon?job=Rbin_ujv8_uJv8__rclc_examples__ubuntu_jammy_arm64__binary)](https://build.ros2.org/job/Rbin_ujv8_uJv8__rclc_examples__ubuntu_jammy_arm64__binary/) | 
|     |     |   |   |    
| [rclc_lifecycle](https://github.com/ros2/rclc/tree/master/rclc_lifecycle) 
| | Foxy | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Fbin_uF64__rclc_lifecycle__ubuntu_focal_amd64__binary)](https://build.ros2.org/job/Fbin_uF64__rclc_lifecycle__ubuntu_focal_amd64__binary/) | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Fbin_ubv8_uFv8__rclc_lifecycle__ubuntu_focal_arm64__binary)](https://build.ros2.org/job/Fbin_ubv8_uFv8__rclc_lifecycle__ubuntu_focal_arm64__binary/) | 
| | Humble | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Hbin_uJ64__rclc_lifecycle__ubuntu_jammy_amd64__binary)](https://build.ros2.org/job/Hbin_uJ64__rclc_lifecycle__ubuntu_jammy_amd64__binary/)|  [![Build Status](https://build.ros2.org/buildStatus/icon?job=Hbin_ujv8_uJv8__rclc_lifecycle__ubuntu_jammy_arm64__binary)](https://build.ros2.org/job/Hbin_ujv8_uJv8__rclc_lifecycle__ubuntu_jammy_arm64__binary/) |
| | Iron | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Ibin_uJ64__rclc_lifecycle__ubuntu_jammy_amd64__binary)](https://build.ros2.org/job/Ibin_uJ64__rclc_lifecycle__ubuntu_jammy_amd64__binary/) | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Ibin_ujv8_uJv8__rclc_lifecycle__ubuntu_jammy_arm64__binary)](https://build.ros2.org/job/Ibin_ujv8_uJv8__rclc_lifecycle__ubuntu_jammy_arm64__binary/)  |
| | Rolling | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Rbin_uJ64__rclc_lifecycle__ubuntu_jammy_amd64__binary)](https://build.ros2.org/job/Rbin_uJ64__rclc_lifecycle__ubuntu_jammy_amd64__binary/) | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Rbin_ujv8_uJv8__rclc_lifecycle__ubuntu_jammy_arm64__binary)](https://build.ros2.org/job/Rbin_ujv8_uJv8__rclc_lifecycle__ubuntu_jammy_arm64__binary/) | 
|     |     |   |   |    
| [rclc_parameter](https://github.com/ros2/rclc/tree/master/rclc_parameter) 
| | Humble | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Hbin_uJ64__rclc_parameter__ubuntu_jammy_amd64__binary)](https://build.ros2.org/job/Hbin_uJ64__rclc_parameter__ubuntu_jammy_amd64__binary/) |  [![Build Status](https://build.ros2.org/buildStatus/icon?job=Hbin_ujv8_uJv8__rclc_parameter__ubuntu_jammy_arm64__binary)](https://build.ros2.org/job/Hbin_ujv8_uJv8__rclc_parameter__ubuntu_jammy_arm64__binary/) |
| | Iron | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Ibin_uJ64__rclc_parameter__ubuntu_jammy_amd64__binary)](https://build.ros2.org/job/Ibin_uJ64__rclc_parameter__ubuntu_jammy_amd64__binary/) | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Ibin_ujv8_uJv8__rclc_parameter__ubuntu_jammy_arm64__binary)](https://build.ros2.org/job/Ibin_ujv8_uJv8__rclc_parameter__ubuntu_jammy_arm64__binary/)  |
| | Rolling | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Rbin_uJ64__rclc_parameter__ubuntu_jammy_amd64__binary)](https://build.ros2.org/job/Rbin_uJ64__rclc_parameter__ubuntu_jammy_amd64__binary/) | [![Build Status](https://build.ros2.org/buildStatus/icon?job=Rbin_ujv8_uJv8__rclc_parameter__ubuntu_jammy_arm64__binary)](https://build.ros2.org/job/Rbin_ujv8_uJv8__rclc_parameter__ubuntu_jammy_arm64__binary/)

## Code coverage
The code coverage is tested with every commit, pull request. Results are available at [codecov](https://app.codecov.io/gh/ros2/rclc/branch/master/).

[![codecov](https://codecov.io/gh/ros2/rclc/branch/master/graph/badge.svg?token=QzyykDh4zF)](https://codecov.io/gh/ros2/rclc)
