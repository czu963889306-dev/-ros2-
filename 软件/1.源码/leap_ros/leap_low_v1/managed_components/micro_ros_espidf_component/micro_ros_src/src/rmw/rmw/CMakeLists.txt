cmake_minimum_required(VERSION 3.5)

project(rmw)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake_ros REQUIRED)

find_package(rcutils REQUIRED)
find_package(rosidl_runtime_c REQUIRED)

include(cmake/configure_rmw_library.cmake)

set(rmw_sources
  "src/allocators.c"
  "src/convert_rcutils_ret_to_rmw_ret.c"
  "src/event.c"
  "src/init.c"
  "src/init_options.c"
  "src/message_sequence.c"
  "src/names_and_types.c"
  "src/network_flow_endpoint_array.c"
  "src/network_flow_endpoint.c"
  "src/publisher_options.c"
  "src/qos_string_conversions.c"
  "src/sanity_checks.c"
  "src/security_options.c"
  "src/subscription_content_filter_options.c"
  "src/subscription_options.c"
  "src/time.c"
  "src/topic_endpoint_info_array.c"
  "src/topic_endpoint_info.c"
  "src/types.c"
  "src/validate_full_topic_name.c"
  "src/validate_namespace.c"
  "src/validate_node_name.c"
)
set_source_files_properties(${rmw_sources} PROPERTIES LANGUAGE "C")
add_library(${PROJECT_NAME} ${rmw_sources})
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

if(BUILD_TESTING AND NOT RCUTILS_DISABLE_FAULT_INJECTION)
  target_compile_definitions(${PROJECT_NAME} PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
endif()

ament_target_dependencies(${PROJECT_NAME}
  "rcutils"
  "rosidl_runtime_c"
)
configure_rmw_library(${PROJECT_NAME} LANGUAGE "C")

ament_export_dependencies(
  rcutils
  rosidl_runtime_c
)

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_libraries(${PROJECT_NAME})

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
  # pass extra include dirs to cppcheck explicitly
  list(APPEND AMENT_LINT_AUTO_EXCLUDE ament_cmake_cppcheck)
  ament_cppcheck(INCLUDE_DIRS ${rcutils_INCLUDE_DIRS})

  add_subdirectory(test)
endif()

ament_export_development_version_if_higher_than_manifest("1.1.1-dev")
ament_package(CONFIG_EXTRAS "rmw-extras.cmake")

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)
install(
  DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)
install(
  TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
