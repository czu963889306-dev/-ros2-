
## Usage

To use this package do the following things.
 - add a test dependency to your packages.xml: `<test_depend>ament_cmake_gtest</test_depend>`
 - Write a gtest executable: Use `#include <gtest/gtest.h>` and standard gtest methods.
 - Find package gtest in your CMakeLists.txt: `find_package(ament_cmake_gtest REQUIRED)`
 - Add a cmake target using the `ament_add_gtest` cmake macro
