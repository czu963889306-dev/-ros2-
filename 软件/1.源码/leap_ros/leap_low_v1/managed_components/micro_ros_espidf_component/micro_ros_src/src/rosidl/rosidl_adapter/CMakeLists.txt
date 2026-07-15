cmake_minimum_required(VERSION 3.12)

project(rosidl_adapter NONE)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

ament_python_install_package(${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_pytest REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
  ament_add_pytest_test(pytest test)
endif()

ament_package(
  CONFIG_EXTRAS "rosidl_adapter-extras.cmake"
)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)

install(PROGRAMS
  scripts/msg2idl.py
  scripts/srv2idl.py
  DESTINATION lib/${PROJECT_NAME})
