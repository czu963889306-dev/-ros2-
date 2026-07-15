cmake_minimum_required(VERSION 3.5)

project(ament_cmake_xmllint NONE)

find_package(ament_cmake_core REQUIRED)
find_package(ament_cmake_test REQUIRED)

ament_package(
  CONFIG_EXTRAS "ament_cmake_xmllint-extras.cmake"
)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_cmake_copyright REQUIRED)
  ament_copyright()

  find_package(ament_cmake_lint_cmake REQUIRED)
  ament_lint_cmake()
endif()
