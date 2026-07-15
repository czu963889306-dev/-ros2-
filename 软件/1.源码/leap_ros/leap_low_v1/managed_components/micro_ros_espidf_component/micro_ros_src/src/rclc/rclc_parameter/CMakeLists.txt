cmake_minimum_required(VERSION 3.5)
project(rclc_parameter)

#################################################
# package options
#################################################
set(RCLC_PARAMETER_MAX_STRING_LENGTH 50 CACHE STRING "Set the maximum length for strings.")
add_definitions(-DRCLC_PARAMETER_MAX_STRING_LENGTH=${RCLC_PARAMETER_MAX_STRING_LENGTH})

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
find_package(ament_cmake REQUIRED)
find_package(rcl REQUIRED)
find_package(rclc REQUIRED)
find_package(rcutils REQUIRED)
find_package(builtin_interfaces REQUIRED)
find_package(rcl_interfaces REQUIRED)
find_package(rosidl_runtime_c REQUIRED)

#################################################
# create library
#################################################
add_library(${PROJECT_NAME}
  src/rclc_parameter/parameter_server.c
  src/rclc_parameter/parameter_utils.c
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
)

target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

ament_target_dependencies(${PROJECT_NAME}
  rcl
  rclc
  rcutils
  builtin_interfaces
  rcl_interfaces
  rosidl_runtime_c
)



#################################################
# install
#################################################
# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME}
  PUBLIC "RCLC_PARAMETER_BUILDING_LIBRARY")
# this alternative would also work
#add_definitions(-DRCLC_PARAMETER_BUILDING_LIBRARY)

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
ament_export_targets(${PROJECT_NAME})

#################################################
# gtest
#################################################
if(BUILD_TESTING)
  find_package(rclcpp REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE) # No copyright header check since link to NOTICE file is not recognized properly.
  find_package(osrf_testing_tools_cpp REQUIRED)
  find_package(std_msgs REQUIRED)
  find_package(example_interfaces REQUIRED)

  ament_lint_auto_find_test_dependencies()

  ament_add_gtest(${PROJECT_NAME}_test
    test/rclc_parameter/test_parameter.cpp
  )

  target_include_directories(${PROJECT_NAME}_test PRIVATE include src)
  target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME})
  ament_target_dependencies(${PROJECT_NAME}_test
    rclcpp
    rclc
    rcl
    rcutils
    rosidl_generator_c
    osrf_testing_tools_cpp
    std_msgs
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
ament_export_dependencies(rclc)
ament_export_dependencies(rcutils)
ament_export_dependencies(builtin_interfaces)
ament_package()
