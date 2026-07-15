cmake_minimum_required(VERSION 3.5)

project(rosidl_default_generators NONE)

find_package(ament_cmake REQUIRED)

ament_export_dependencies(ament_cmake_core)
ament_export_dependencies(rosidl_cmake)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package(
  CONFIG_EXTRAS "rosidl_default_generators-extras.cmake.in"
)
