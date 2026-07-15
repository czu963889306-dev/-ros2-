cmake_minimum_required(VERSION 3.5)

project(ament_cmake_lint_cmake NONE)

find_package(ament_cmake_core REQUIRED)
find_package(ament_cmake_test REQUIRED)

# include CMake functions of this package
set(ament_cmake_lint_cmake_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include("ament_cmake_lint_cmake-extras.cmake" NO_POLICY_SCOPE)

ament_package(
  CONFIG_EXTRAS "ament_cmake_lint_cmake-extras.cmake"
)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  ament_lint_cmake()
endif()
