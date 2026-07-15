cmake_minimum_required(VERSION 3.12)

project(ament_cmake NONE)

find_package(ament_cmake_core REQUIRED)
find_package(ament_cmake_export_dependencies REQUIRED)

ament_export_dependencies(
  "ament_cmake_core"
  "ament_cmake_export_definitions"
  "ament_cmake_export_dependencies"
  "ament_cmake_export_include_directories"
  "ament_cmake_export_interfaces"
  "ament_cmake_export_libraries"
  "ament_cmake_export_link_flags"
  "ament_cmake_export_targets"
  "ament_cmake_gen_version_h"
  "ament_cmake_libraries"
  "ament_cmake_python"
  "ament_cmake_target_dependencies"
  "ament_cmake_test"
  "ament_cmake_version"
)

ament_package()
