cmake_minimum_required(VERSION 3.12)

project(ament_cmake_auto NONE)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_gmock REQUIRED)
find_package(ament_cmake_gtest REQUIRED)

ament_package(
  CONFIG_EXTRAS "ament_cmake_auto-extras.cmake"
)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)
