cmake_minimum_required(VERSION 3.15)
project(ament_cmake_vendor_package NONE)

find_package(ament_cmake_core REQUIRED)
find_package(ament_cmake_export_dependencies REQUIRED)

ament_export_dependencies(
  ament_cmake_core
)

ament_package(
  CONFIG_EXTRAS "ament_cmake_vendor_package-extras.cmake"
)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(test)
endif()

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)
