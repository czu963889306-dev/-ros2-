cmake_minimum_required(VERSION 3.5)

project(rcl_lifecycle)

find_package(ament_cmake_ros REQUIRED)
find_package(ament_cmake_gen_version_h REQUIRED)

find_package(lifecycle_msgs REQUIRED)
find_package(rcl REQUIRED)
find_package(rcutils REQUIRED)
find_package(rmw REQUIRED)
find_package(rosidl_runtime_c REQUIRED)
find_package(tracetools REQUIRED)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(
    -Wall -Wextra -Wpedantic
    -Wformat=2 -Wconversion -Wshadow -Wsign-conversion
  )
endif()

set(rcl_lifecycle_sources
  src/com_interface.c
  src/default_state_machine.c
  src/rcl_lifecycle.c
  src/transition_map.c
)
set_source_files_properties(
  ${rcl_lifecycle_sources}
  PROPERTIES language "C")

### C-Library depending only on RCL
add_library(
  ${PROJECT_NAME}
  ${rcl_lifecycle_sources})

target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

# specific order: dependents before dependencies
ament_target_dependencies(rcl_lifecycle
  "lifecycle_msgs"
  "rcl"
  "rcutils"
  "rosidl_runtime_c"
  "tracetools"
)

rcl_set_symbol_visibility_hidden(${PROJECT_NAME} LANGUAGE "C")
# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(rcl_lifecycle PRIVATE "RCL_LIFECYCLE_BUILDING_DLL")

if(BUILD_TESTING AND NOT RCUTILS_DISABLE_FAULT_INJECTION)
  target_compile_definitions(${PROJECT_NAME} PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
endif()

install(TARGETS rcl_lifecycle EXPORT rcl_lifecycle
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  find_package(rcl REQUIRED)
  find_package(osrf_testing_tools_cpp REQUIRED)
  ament_lint_auto_find_test_dependencies()

  # Gtests
  ament_add_gtest(test_default_state_machine
    test/test_default_state_machine.cpp
  )
  if(TARGET test_default_state_machine)
    ament_target_dependencies(test_default_state_machine
      "rcl"
      "osrf_testing_tools_cpp"
    )
    target_link_libraries(test_default_state_machine ${PROJECT_NAME})
    target_compile_definitions(test_default_state_machine
      PUBLIC RCUTILS_ENABLE_FAULT_INJECTION
    )
  endif()
  ament_add_gtest(test_multiple_instances
    test/test_multiple_instances.cpp
  )
  if(TARGET test_multiple_instances)
    ament_target_dependencies(test_multiple_instances
      "rcl"
      "osrf_testing_tools_cpp"
    )
    target_link_libraries(test_multiple_instances ${PROJECT_NAME})
  endif()
  ament_add_gtest(test_rcl_lifecycle
    test/test_rcl_lifecycle.cpp
  )
  if(TARGET test_rcl_lifecycle)
    ament_target_dependencies(test_rcl_lifecycle
      "rcl"
      "osrf_testing_tools_cpp"
    )
    target_link_libraries(test_rcl_lifecycle ${PROJECT_NAME})
    target_compile_definitions(test_rcl_lifecycle
      PUBLIC RCUTILS_ENABLE_FAULT_INJECTION
    )
  endif()
  ament_add_gtest(test_transition_map
    test/test_transition_map.cpp
  )
  if(TARGET test_transition_map)
    ament_target_dependencies(test_transition_map
      "rcl"
    )
    target_link_libraries(test_transition_map ${PROJECT_NAME})
  endif()
endif()

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_libraries(${PROJECT_NAME})

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME})

# specific order: dependents before dependencies
ament_export_dependencies(ament_cmake)
ament_export_dependencies(lifecycle_msgs)
ament_export_dependencies(rcl)
ament_export_dependencies(rcutils)
ament_export_dependencies(rosidl_runtime_c)
ament_export_dependencies(tracetools)

ament_generate_version_header(${PROJECT_NAME})

ament_package()

install(
  DIRECTORY include/
  DESTINATION include/${PROJECT_NAME})
