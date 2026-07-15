find_package(ament_cmake_gtest REQUIRED)
find_package(launch_testing_ament_cmake REQUIRED)

find_package(mimick_vendor REQUIRED)
find_package(test_msgs REQUIRED)

find_package(mimick_vendor REQUIRED)

find_package(rcpputils REQUIRED)
find_package(rcutils REQUIRED)
find_package(rmw_implementation_cmake REQUIRED)

find_package(osrf_testing_tools_cpp REQUIRED)

get_target_property(memory_tools_ld_preload_env_var
  osrf_testing_tools_cpp::memory_tools LIBRARY_PRELOAD_ENVIRONMENT_VARIABLE)

include(cmake/rcl_add_custom_executable.cmake)
include(cmake/rcl_add_custom_gtest.cmake)
include(cmake/rcl_add_custom_launch_test.cmake)

set(extra_lib_dirs "${rcl_lib_dir}")
add_definitions(-DTEST_RESOURCES_DIRECTORY="${CMAKE_CURRENT_SOURCE_DIR}/resources")

set(DISTRIBUTION "Unknown")
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  # If we are on Linux, look for /etc/os-release, which has "key=value" items.
  # Parse those items, looking for a key named "id" (case-insensitive), and
  # if we find it, set DISTRIBUTION to that value.  That gives us some idea
  # of which Linux distribution we are on.
  if(EXISTS "/etc/os-release")
    file(STRINGS "/etc/os-release" OS_RELEASE)
    foreach(line ${OS_RELEASE})
      string(REGEX REPLACE "^(.*)=.*" "\\1" key "${line}")
      string(TOLOWER "${key}" key)
      if("${key}" STREQUAL "id")
        string(REGEX REPLACE "^.*=(.*)" "\\1" DISTRIBUTION "${line}")
        string(TOLOWER "${DISTRIBUTION}" DISTRIBUTION)
        break()
      endif()
    endforeach()
  endif()
endif()

# finding gtest once in the highest scope
# prevents finding it repeatedly in each local scope
ament_find_gtest()

macro(test_target)
  find_package(${rmw_implementation} REQUIRED)
  test_target_function()
endmacro()

function(test_target_function)
  message(STATUS "Creating tests for '${rmw_implementation}'")
  set(rmw_implementation_env_var RMW_IMPLEMENTATION=${rmw_implementation})

  # Gtests

  rcl_add_custom_gtest(test_client${target_suffix}
    SRCS rcl/test_client.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME}
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp" "test_msgs"
  )

  rcl_add_custom_gtest(test_time${target_suffix}
    SRCS rcl/test_time.cpp
    ENV ${rmw_implementation_env_var} ${memory_tools_ld_preload_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME} osrf_testing_tools_cpp::memory_tools
    AMENT_DEPENDENCIES ${rmw_implementation}
  )

  rcl_add_custom_gtest(test_timer${target_suffix}
    SRCS rcl/test_timer.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME} osrf_testing_tools_cpp::memory_tools
    AMENT_DEPENDENCIES ${rmw_implementation}
  )

  rcl_add_custom_gtest(test_context${target_suffix}
    SRCS rcl/test_context.cpp
    ENV ${rmw_implementation_env_var} ${memory_tools_ld_preload_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME} mimick osrf_testing_tools_cpp::memory_tools
    AMENT_DEPENDENCIES ${rmw_implementation}
  )

  rcl_add_custom_gtest(test_get_node_names${target_suffix}
    SRCS rcl/test_get_node_names.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME}
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp"
  )

  rcl_add_custom_gtest(test_lexer${target_suffix}
    SRCS rcl/test_lexer.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME}
    AMENT_DEPENDENCIES ${rmw_implementation}
  )

  rcl_add_custom_gtest(test_lexer_lookahead${target_suffix}
    SRCS rcl/test_lexer_lookahead.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME}
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp"
  )

  rcl_add_custom_gtest(test_graph${target_suffix}
    SRCS rcl/test_graph.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME}
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp" "test_msgs"
    TIMEOUT 120
  )

  rcl_add_custom_gtest(test_info_by_topic${target_suffix}
    SRCS rcl/test_info_by_topic.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME} wait_for_entity_helpers
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp" "test_msgs"
  )

  rcl_add_custom_gtest(test_count_matched${target_suffix}
    SRCS rcl/test_count_matched.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME}
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp" "test_msgs"
  )

  rcl_add_custom_gtest(test_get_actual_qos${target_suffix}
    SRCS rcl/test_get_actual_qos.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME}
    AMENT_DEPENDENCIES ${rmw_implementation} "test_msgs" "osrf_testing_tools_cpp"
  )

  rcl_add_custom_gtest(test_init${target_suffix}
    SRCS rcl/test_init.cpp
    ENV ${rmw_implementation_env_var} ${memory_tools_ld_preload_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME} mimick osrf_testing_tools_cpp::memory_tools
    AMENT_DEPENDENCIES ${rmw_implementation}
  )

  # RHEL does not support mimick's inject_on_return functionality, which
  # causes test_node to segfault when it runs.  Since RHEL is not a Tier 1
  # platform, just disable the test since we are covering it elsewhere.
  if(
    "${DISTRIBUTION}" STREQUAL "\"centos\"" OR
    "${DISTRIBUTION}" STREQUAL "\"almalinux\"")
    set(gtest_filter_env_var "GTEST_FILTER=-TestNodeFixture__*.test_rcl_node_init_with_internal_errors")
  else()
    set(gtest_filter_env_var "")
  endif()
  rcl_add_custom_gtest(test_node${target_suffix}
    SRCS rcl/test_node.cpp
    ENV ${rmw_implementation_env_var} ${memory_tools_ld_preload_env_var} ${gtest_filter_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME} mimick osrf_testing_tools_cpp::memory_tools
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp"
    TIMEOUT 240  # Large timeout to wait for fault injection tests
  )

  rcl_add_custom_gtest(test_arguments${target_suffix}
    SRCS rcl/test_arguments.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../src/rcl/
    LIBRARIES ${PROJECT_NAME} osrf_testing_tools_cpp::memory_tools
    AMENT_DEPENDENCIES "osrf_testing_tools_cpp" "rcpputils"
  )

  rcl_add_custom_gtest(test_remap${target_suffix}
    SRCS rcl/test_remap.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../src/rcl/
    LIBRARIES ${PROJECT_NAME}
    AMENT_DEPENDENCIES "osrf_testing_tools_cpp"
  )

  rcl_add_custom_gtest(test_remap_integration${target_suffix}
    SRCS rcl/test_remap_integration.cpp
    TIMEOUT 200
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME}
    AMENT_DEPENDENCIES "osrf_testing_tools_cpp" "test_msgs"
  )

  rcl_add_custom_gtest(test_guard_condition${target_suffix}
    SRCS rcl/test_guard_condition.cpp
    ENV ${rmw_implementation_env_var} ${memory_tools_ld_preload_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME} mimick osrf_testing_tools_cpp::memory_tools
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp"
  )

  rcl_add_custom_gtest(test_publisher${target_suffix}
    SRCS rcl/test_publisher.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../src/rcl/
    LIBRARIES ${PROJECT_NAME} mimick
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp" "test_msgs"
  )

  rcl_add_custom_gtest(test_publisher_wait_all_ack${target_suffix}
    SRCS rcl/test_publisher_wait_all_ack.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../src/rcl/
    LIBRARIES ${PROJECT_NAME} mimick wait_for_entity_helpers
    AMENT_DEPENDENCIES ${rmw_implementation}
      "osrf_testing_tools_cpp"
      "rcpputils"
      "rcutils"
      "rosidl_runtime_c"
      "test_msgs"
  )

  rcl_add_custom_gtest(test_service${target_suffix}
    SRCS rcl/test_service.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME} mimick wait_for_entity_helpers
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp" "test_msgs"
  )

  rcl_add_custom_gtest(test_subscription${target_suffix}
    SRCS rcl/test_subscription.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME} mimick wait_for_entity_helpers
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp" "test_msgs"
    TIMEOUT 120
  )
  # TODO(asorbini) Enable message timestamp tests for rmw_connextdds on Windows
  # once clock incompatibilities are resolved.
  if(rmw_implementation STREQUAL "rmw_fastrtps_cpp" OR
    rmw_implementation STREQUAL "rmw_fastrtps_dynamic_cpp" OR
    (rmw_implementation STREQUAL "rmw_connextdds" AND NOT WIN32))
    message(STATUS "Enabling message timestamp test for ${rmw_implementation}")
    target_compile_definitions(test_subscription${target_suffix}
      PUBLIC "RMW_TIMESTAMPS_SUPPORTED=1" "RMW_RECEIVED_TIMESTAMP_SUPPORTED=1")
    target_compile_definitions(test_service${target_suffix}
      PUBLIC "RMW_TIMESTAMPS_SUPPORTED=1" "RMW_RECEIVED_TIMESTAMP_SUPPORTED=1")
  else()
    if(rmw_implementation STREQUAL "rmw_cyclonedds_cpp")
      message(STATUS "Enabling only source timestamp test for ${rmw_implementation}")
      target_compile_definitions(test_subscription${target_suffix}
        PUBLIC "RMW_TIMESTAMPS_SUPPORTED=1")
      target_compile_definitions(test_service${target_suffix}
        PUBLIC "RMW_TIMESTAMPS_SUPPORTED=1")
    else()
      message(STATUS "Disabling message timestamp test for ${rmw_implementation}")
    endif()
  endif()

  rcl_add_custom_gtest(test_events${target_suffix}
    SRCS rcl/test_events.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../src/rcl/
    LIBRARIES ${PROJECT_NAME}
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp" "test_msgs"
  )

  rcl_add_custom_gtest(test_wait${target_suffix}
    SRCS rcl/test_wait.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME} mimick
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp"
  )

  rcl_add_custom_gtest(test_logging_rosout${target_suffix}
    SRCS rcl/test_logging_rosout.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME}
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp" "rcl_interfaces"
  )

  rcl_add_custom_gtest(test_namespace${target_suffix}
    SRCS test_namespace.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME}
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp" "test_msgs"
  )

  rcl_add_custom_gtest(test_rmw_impl_id_check_func${target_suffix}
    SRCS rcl/test_rmw_impl_id_check_func.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    LIBRARIES ${PROJECT_NAME} mimick
    AMENT_DEPENDENCIES ${rmw_implementation}
  )

  rcl_add_custom_gtest(test_network_flow_endpoints${target_suffix}
    SRCS rcl/test_network_flow_endpoints.cpp
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../src/rcl/
    LIBRARIES ${PROJECT_NAME} mimick
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp" "test_msgs"
  )

  # Launch tests

  rcl_add_custom_executable(service_fixture${target_suffix}
    SRCS rcl/service_fixture.cpp
    LIBRARIES ${PROJECT_NAME} wait_for_entity_helpers
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp" "test_msgs"
  )

  rcl_add_custom_executable(client_fixture${target_suffix}
    SRCS rcl/client_fixture.cpp
    LIBRARIES ${PROJECT_NAME} wait_for_entity_helpers
    AMENT_DEPENDENCIES ${rmw_implementation} "osrf_testing_tools_cpp" "test_msgs"
  )

  rcl_add_custom_launch_test(test_services
    service_fixture
    client_fixture
    ENV ${rmw_implementation_env_var}
    APPEND_LIBRARY_DIRS ${extra_lib_dirs}
    TIMEOUT 15
  )

  set(SKIP_TEST "")
  if(WIN32)
    # TODO(dhood): launch does not set the return code correctly for these tests on Windows.
    # See https://github.com/ros2/launch/issues/66
    set(SKIP_TEST "SKIP_TEST")
  endif()
  set(TEST_RMW_IMPL_ID_CHECK_EXECUTABLE_NAME "$<TARGET_FILE:test_rmw_impl_id_check_exe>")
  configure_file(
    rcl/test_rmw_impl_id_check.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/test_rmw_impl_id_check${target_suffix}.py.configure
    @ONLY
  )
  file(GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test/test_rmw_impl_id_check${target_suffix}_$<CONFIG>.py"
    INPUT "${CMAKE_CURRENT_BINARY_DIR}/test_rmw_impl_id_check${target_suffix}.py.configure"
  )
  add_launch_test(
    "${CMAKE_CURRENT_BINARY_DIR}/test/test_rmw_impl_id_check${target_suffix}_$<CONFIG>.py"
    TARGET test_rmw_impl_id_check${target_suffix}
    APPEND_LIBRARY_DIRS "${extra_lib_dirs}"
    ${SKIP_TEST}
  )
  if(TEST test_rmw_impl_id_check${target_suffix})
    set_tests_properties(
      test_rmw_impl_id_check${target_suffix}
      PROPERTIES DEPENDS test_rmw_impl_id_check_exe
    )
  endif()

endfunction()

# Build simple executable for using in the test_rmw_impl_id_check
add_executable(test_rmw_impl_id_check_exe
  rcl/test_rmw_impl_id_check_exe.cpp)
target_link_libraries(test_rmw_impl_id_check_exe ${PROJECT_NAME})

# This file is used by many tests, so build it just once
add_library(wait_for_entity_helpers STATIC rcl/wait_for_entity_helpers.cpp)
target_include_directories(wait_for_entity_helpers PRIVATE
  ${osrf_testing_tools_cpp_INCLUDE_DIRS})
target_link_libraries(wait_for_entity_helpers PUBLIC ${PROJECT_NAME})
target_link_libraries(wait_for_entity_helpers PRIVATE
  rcutils::rcutils)

call_for_each_rmw_implementation(test_target)

rcl_add_custom_gtest(test_validate_enclave_name
  SRCS rcl/test_validate_enclave_name.cpp
  APPEND_LIBRARY_DIRS ${extra_lib_dirs}
  LIBRARIES ${PROJECT_NAME} mimick
)

rcl_add_custom_gtest(test_domain_id
  SRCS rcl/test_domain_id.cpp
  APPEND_LIBRARY_DIRS ${extra_lib_dirs}
  LIBRARIES ${PROJECT_NAME} mimick
)

rcl_add_custom_gtest(test_localhost
  SRCS rcl/test_localhost.cpp
  APPEND_LIBRARY_DIRS ${extra_lib_dirs}
  LIBRARIES ${PROJECT_NAME}
)

rcl_add_custom_gtest(test_logging
  SRCS rcl/test_logging.cpp
  APPEND_LIBRARY_DIRS ${extra_lib_dirs}
  LIBRARIES ${PROJECT_NAME} mimick
  AMENT_DEPENDENCIES "osrf_testing_tools_cpp"
)

rcl_add_custom_gtest(test_validate_topic_name
  SRCS rcl/test_validate_topic_name.cpp
  APPEND_LIBRARY_DIRS ${extra_lib_dirs}
  LIBRARIES ${PROJECT_NAME}
)

rcl_add_custom_gtest(test_expand_topic_name
  SRCS rcl/test_expand_topic_name.cpp
  APPEND_LIBRARY_DIRS ${extra_lib_dirs}
  LIBRARIES ${PROJECT_NAME} mimick
)

rcl_add_custom_gtest(test_security
  SRCS rcl/test_security.cpp
  APPEND_LIBRARY_DIRS ${extra_lib_dirs}
  LIBRARIES ${PROJECT_NAME} mimick
  AMENT_DEPENDENCIES "osrf_testing_tools_cpp"
)

rcl_add_custom_gtest(test_common
  SRCS rcl/test_common.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../src/rcl/common.c
  APPEND_LIBRARY_DIRS ${extra_lib_dirs}
  INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../src/rcl/
  LIBRARIES ${PROJECT_NAME}
)

rcl_add_custom_gtest(test_log_level
  SRCS rcl/test_log_level.cpp
  APPEND_LIBRARY_DIRS ${extra_lib_dirs}
  LIBRARIES ${PROJECT_NAME} mimick
  AMENT_DEPENDENCIES "osrf_testing_tools_cpp"
)

rcl_add_custom_gtest(test_subscription_content_filter_options
  SRCS rcl/test_subscription_content_filter_options.cpp
  APPEND_LIBRARY_DIRS ${extra_lib_dirs}
  LIBRARIES ${PROJECT_NAME}
  AMENT_DEPENDENCIES "osrf_testing_tools_cpp" "test_msgs"
)
