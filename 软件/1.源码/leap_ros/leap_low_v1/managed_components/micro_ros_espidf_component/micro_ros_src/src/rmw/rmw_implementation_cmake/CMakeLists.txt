cmake_minimum_required(VERSION 3.5)

project(rmw_implementation_cmake NONE)

find_package(ament_cmake REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_dependencies(ament_cmake)

ament_package(
  CONFIG_EXTRAS "rmw_implementation_cmake-extras.cmake"
)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)
