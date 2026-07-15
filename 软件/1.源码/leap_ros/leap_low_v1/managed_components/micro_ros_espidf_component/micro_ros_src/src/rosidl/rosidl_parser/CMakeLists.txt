cmake_minimum_required(VERSION 3.5)

project(rosidl_parser NONE)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

ament_python_install_package(${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()

if(BUILD_TESTING)
  find_package(ament_cmake_pytest REQUIRED)
  ament_add_pytest_test(pytest test)
endif()

install(
  PROGRAMS bin/idl2png
  DESTINATION lib/${PROJECT_NAME})
