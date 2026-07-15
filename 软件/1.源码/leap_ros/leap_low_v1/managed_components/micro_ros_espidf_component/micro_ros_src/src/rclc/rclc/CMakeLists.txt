cmake_minimum_required(VERSION 3.5)

project(rclc)

set(CMAKE_VERBOSE_MAKEFILE ON)

#################################################
# compiler settings
#################################################

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_VERBOSE_MAKEFILE ON)

#################################################
# package dependencies
#################################################
find_package(ament_cmake_ros REQUIRED)
find_package(rcl REQUIRED)
find_package(rcl_action REQUIRED)
find_package(rcutils REQUIRED)
find_package(rosidl_generator_c REQUIRED)

if("${rcl_VERSION}" VERSION_LESS "1.0.0")
  message(STATUS
    "Found rcl version ${rcl_VERSION}, which belongs to Dashing or Eloquent")
  # Later, with CMake 3.12+ use:
  # add_compile_definitions(USE_RCL_WAIT_SET_IS_VALID_BACKPORT)
  add_definitions(-DUSE_RCL_WAIT_SET_IS_VALID_BACKPORT)
else()
  message(STATUS
    "Found rcl version ${rcl_VERSION}, which belongs to Foxy or later")
  find_package(rosidl_runtime_c REQUIRED)
endif()

#################################################
# create library
#################################################

add_library(${PROJECT_NAME}
  src/rclc/init.c
  src/rclc/publisher.c
  src/rclc/subscription.c
  src/rclc/client.c
  src/rclc/service.c
  src/rclc/timer.c
  src/rclc/action_client.c
  src/rclc/action_server.c
  src/rclc/action_goal_handle.c
  src/rclc/node.c
  src/rclc/executor_handle.c
  src/rclc/executor.c
  src/rclc/sleep.c
)
if("${rcl_VERSION}" VERSION_LESS "1.0.0")
  target_sources(${PROJECT_NAME}
    PRIVATE src/rclc/rcl_wait_set_is_valid_backport.c)
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
)

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
# specific order: dependents before dependencies
ament_target_dependencies(${PROJECT_NAME}
  rcl
  rcl_action
  rcutils
  rosidl_generator_c
)

#################################################
# install
#################################################
#install(DIRECTORY include/ DESTINATION include)
#install(TARGETS ${PROJECT_NAME} DESTINATION lib)

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME}
  PRIVATE "RCLC_BUILDING_LIBRARY")

install(
  TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(
  DIRECTORY include/
  DESTINATION include
)

# specific order: dependents before dependencies
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})

if("${rcl_VERSION}" VERSION_LESS "1.1.10")
# ament_export_targets command is not available for Dashing
else()
ament_export_targets(${PROJECT_NAME})
endif()


#################################################
# gtest
#################################################
if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  find_package(osrf_testing_tools_cpp REQUIRED)
  find_package(std_msgs REQUIRED)
  find_package(example_interfaces REQUIRED)
  find_package(rclcpp REQUIRED)
  find_package(rclcpp_action REQUIRED)

  # No copyright header check since link to NOTICE file is not recognized properly.
  set(ament_cmake_copyright_FOUND TRUE)

  ament_lint_auto_find_test_dependencies()

  ament_add_gtest(${PROJECT_NAME}_test
    test/rclc/test_init.cpp
    test/rclc/test_node.cpp
    test/rclc/test_publisher.cpp
    test/rclc/test_subscription.cpp
    test/rclc/test_client.cpp
    test/rclc/test_service.cpp
    test/rclc/test_timer.cpp
    test/rclc/test_executor_handle.cpp
    test/rclc/test_executor.cpp
    test/rclc/test_action_server.cpp
    test/rclc/test_action_client.cpp
  )

  target_include_directories(${PROJECT_NAME}_test PRIVATE include src)
  target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME})
  ament_target_dependencies(${PROJECT_NAME}_test
    rclcpp
    rclcpp_action
    rcl
    rcutils
    rosidl_generator_c
    osrf_testing_tools_cpp
    std_msgs
    test_msgs
    example_interfaces
  )


endif()

#################################################
# export dependencies
#################################################
# specific order: dependents before dependencies
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(ament_cmake)
ament_export_dependencies(rcl)
ament_export_dependencies(rcl_action)
ament_export_dependencies(rcutils)
ament_export_dependencies(rosidl_generator_c)
ament_package()
