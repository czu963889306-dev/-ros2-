# rosidl

```rosidl``` is one of the ros_core packages.
See [documentation](https://docs.ros.org/en/rolling/Concepts/About-Internal-Interfaces.html#the-rosidl-repository) for details of this package.

## Packages

* [rosidl_adapter](./rosidl_adapter)
  * API and scripts to parse `.msg`/`.srv`/`.action` files and convert them to `.idl`
* [rosidl_cmake](./rosidl_cmake)
  * CMake functionality to invoke code generation for ROS interface files
* [rosidl_generator_c](./rosidl_generator_c)
  * Generate the ROS interfaces in C
* [rosidl_generator_cpp](./rosidl_generator_cpp)
  * Generate the ROS interfaces in C++
* [rosidl_parser](./rosidl_parser)
  * Parser for `.idl` ROS interface files
* [rosidl_runtime_c](./rosidl_runtime_c)
  * Provides definitions, initialization and finalization functions, and macros for getting and working with rosidl typesupport types in C
* [rosidl_runtime_cpp](./rosidl_runtime_cpp)
  * Provides definitions and templated functions for getting and working with rosidl typesupport types in C++
* [rosidl_typesupport_interface](./rosidl_typesupport_interface)
  * Interface for rosidl typesupport packages
* [rosidl_typesupport_introspection_c](./rosidl_typesupport_introspection_c)
  * Generate the message type support for dynamic message construction in C
* [rosidl_typesupport_introspection_cpp](./rosidl_typesupport_introspection_cpp)
  * Generate the message type support for dynamic message construction in C++
