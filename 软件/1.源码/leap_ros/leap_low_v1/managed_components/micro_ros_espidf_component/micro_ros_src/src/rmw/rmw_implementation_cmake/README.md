# rmw_implementation_cmake

This package provides some CMake functions to discover and enumerate available rmw implementations.

## Features
This package provides the following CMake functions:

* [call_for_each_rmw_implementation](cmake/call_for_each_rmw_implementation.cmake): Call a CMake macro for each available RMW implementation.
* [get_available_rmw_implementations](cmake/get_available_rmw_implementations.cmake): Get the package names of the available ROS middleware implementations.
* [get_default_rmw_implementation](cmake/get_default_rmw_implementation.cmake): Get the package name of the default ROS middleware implementation.
