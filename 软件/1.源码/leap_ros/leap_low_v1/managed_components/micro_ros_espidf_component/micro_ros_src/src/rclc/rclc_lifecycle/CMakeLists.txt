cmake_minimum_required(VERSION 3.5)
project(rclc_lifecycle)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# maximum string length - should cover all lifecycle state and transition names
set(RCLC_LIFECYCLE_MAX_STRING_LENGTH 20 CACHE STRING "Set the maximum length for strings.")
add_definitions(-DRCLC_LIFECYCLE_MAX_STRING_LENGTH=${RCLC_LIFECYCLE_MAX_STRING_LENGTH})

# find dependencies
find_package(ament_cmake_ros REQUIRED)
find_package(std_msgs REQUIRED)
find_package(lifecycle_msgs REQUIRED)
find_package(rcl_lifecycle REQUIRED)
find_package(rclc REQUIRED)


if("${rcl_lifecycle_VERSION}" VERSION_LESS "1.0.0")
  message(STATUS "Found rcl_lifecycle version ${rcl_lifecycle_VERSION}, which belongs to Eloquent or earlier")
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra)
  endif()
else()
  message(STATUS "Found rcl_lifecycle version ${rcl_lifecycle_VERSION}, which belongs to Foxy or later")
  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
  endif()
endif()

# create library
add_library(${PROJECT_NAME}
  src/rclc_lifecycle/rclc_lifecycle.c
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
  rclc
  rcutils
  rcl_lifecycle
  std_msgs
  lifecycle_msgs
)

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME}
  PRIVATE "RCLC_LIFECYCLE_BUILDING_LIBRARY")


# install
#install(DIRECTORY include/ DESTINATION include)
#install(TARGETS ${PROJECT_NAME} DESTINATION lib)

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

ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})

if("${rcl_VERSION}" VERSION_LESS "1.1.10")
# ament_export_targets command is not available for Dashing
else()
ament_export_targets(${PROJECT_NAME})
endif()

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  find_package(osrf_testing_tools_cpp REQUIRED)
  ament_lint_auto_find_test_dependencies()

  ament_add_gtest(${PROJECT_NAME}_test
    test/test_lifecycle.cpp)

  target_include_directories(${PROJECT_NAME}_test PRIVATE include src)
  target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME})
  ament_target_dependencies(${PROJECT_NAME}_test
    rcl
    rcl_lifecycle
    rclc
    lifecycle_msgs
    osrf_testing_tools_cpp
  )
endif()

# export dependencies
# specific order: dependents before dependencies
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(ament_cmake)
ament_export_dependencies(rcl)
ament_export_dependencies(rcl_lifecycle)
ament_export_dependencies(rcutils)
ament_export_dependencies(rosidl_generator_c)
ament_package()
