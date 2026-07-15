cmake_minimum_required(VERSION 3.12)

project(ament_cmake_core NONE)

# include some CMake functions of this package
set(ament_cmake_core_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include("ament_cmake_core-extras.cmake" NO_POLICY_SCOPE)
include("ament_cmake_environment-extras.cmake" NO_POLICY_SCOPE)
include("ament_cmake_environment_hooks-extras.cmake" NO_POLICY_SCOPE)
include("ament_cmake_index-extras.cmake" NO_POLICY_SCOPE)
include("ament_cmake_uninstall_target-extras.cmake" NO_POLICY_SCOPE)
# must be after uninstall_target
include("ament_cmake_symlink_install-extras.cmake" NO_POLICY_SCOPE)

ament_package_xml()

message(STATUS "ament_cmake_core ${ament_cmake_core_VERSION}")

include("ament_cmake_package_templates-extras.cmake" NO_POLICY_SCOPE)

ament_package(
  CONFIG_EXTRAS
  "ament_cmake_core-extras.cmake"
  "ament_cmake_environment-extras.cmake"
  "ament_cmake_environment_hooks-extras.cmake"
  "ament_cmake_index-extras.cmake"
  "ament_cmake_package_templates-extras.cmake"
  "ament_cmake_uninstall_target-extras.cmake"
  "ament_cmake_symlink_install-extras.cmake"  # must be after uninstall_target
)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)
