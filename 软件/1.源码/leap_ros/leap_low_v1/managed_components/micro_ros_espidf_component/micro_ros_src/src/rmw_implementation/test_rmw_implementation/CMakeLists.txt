cmake_minimum_required(VERSION 3.5)

project(test_rmw_implementation)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(osrf_testing_tools_cpp REQUIRED)

  find_package(rcutils REQUIRED)
  find_package(rmw REQUIRED)
  find_package(rmw_implementation REQUIRED)
  find_package(rmw_implementation_cmake REQUIRED)
  find_package(test_msgs REQUIRED)
  find_package(rmw_dds_common REQUIRED)

  macro(test_api)
    find_package(${rmw_implementation} REQUIRED)
    message(STATUS "Creating API tests for '${rmw_implementation}'")
    set(rmw_implementation_env_var RMW_IMPLEMENTATION=${rmw_implementation})

    ament_add_gtest(test_init_shutdown${target_suffix}
      test/test_init_shutdown.cpp
      ENV ${rmw_implementation_env_var}
    )
    target_compile_definitions(test_init_shutdown${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_init_shutdown${target_suffix}
      osrf_testing_tools_cpp rcutils rmw rmw_implementation
    )

    ament_add_gtest(test_init_options${target_suffix}
      test/test_init_options.cpp
      ENV ${rmw_implementation_env_var}
    )
    target_compile_definitions(test_init_options${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_init_options${target_suffix}
      osrf_testing_tools_cpp rcutils rmw rmw_implementation
    )

    ament_add_gtest(test_create_destroy_node${target_suffix}
      test/test_create_destroy_node.cpp
      ENV ${rmw_implementation_env_var}
    )
    target_compile_definitions(test_create_destroy_node${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_create_destroy_node${target_suffix}
      osrf_testing_tools_cpp rcutils rmw rmw_implementation
    )

    ament_add_gtest(test_publisher${target_suffix}
      test/test_publisher.cpp
      ENV ${rmw_implementation_env_var}
    )
    target_compile_definitions(test_publisher${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_publisher${target_suffix}
      osrf_testing_tools_cpp rcutils rmw rmw_implementation test_msgs
    )

    ament_add_gtest(test_subscription${target_suffix}
      test/test_subscription.cpp
      ENV ${rmw_implementation_env_var}
      TIMEOUT 120
    )
    target_compile_definitions(test_subscription${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_subscription${target_suffix}
      osrf_testing_tools_cpp rcutils rmw rmw_implementation test_msgs rmw_dds_common
    )

    ament_add_gtest(test_serialize_deserialize${target_suffix}
      test/test_serialize_deserialize.cpp
      ENV ${rmw_implementation_env_var}
    )
    target_compile_definitions(test_serialize_deserialize${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_serialize_deserialize${target_suffix}
      osrf_testing_tools_cpp rcutils rmw rmw_implementation test_msgs
    )

    ament_add_gtest(test_publisher_allocator${target_suffix}
      test/test_publisher_allocator.cpp
      ENV ${rmw_implementation_env_var}
    )
    target_compile_definitions(test_publisher_allocator${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_publisher_allocator${target_suffix}
      rmw rmw_implementation
    )
    ament_add_gtest(test_subscription_allocator${target_suffix}
      test/test_subscription_allocator.cpp
      ENV ${rmw_implementation_env_var}
    )
    target_compile_definitions(test_subscription_allocator${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_subscription_allocator${target_suffix}
      rmw rmw_implementation
    )

    ament_add_gtest(test_wait_set${target_suffix}
      test/test_wait_set.cpp
      ENV ${rmw_implementation_env_var}
    )
    target_compile_definitions(test_wait_set${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_wait_set${target_suffix}
      rmw rmw_implementation rcutils osrf_testing_tools_cpp test_msgs
    )

    ament_add_gtest(test_graph_api${target_suffix}
      test/test_graph_api.cpp
      ENV ${rmw_implementation_env_var}
      TIMEOUT 120
    )
    target_compile_definitions(test_graph_api${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_graph_api${target_suffix}
      osrf_testing_tools_cpp rcutils rmw rmw_implementation
    )

    ament_add_gtest(test_unique_identifiers${target_suffix}
      test/test_unique_identifiers.cpp
      ENV ${rmw_implementation_env_var}
    )
    target_compile_definitions(test_unique_identifiers${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_unique_identifiers${target_suffix}
      osrf_testing_tools_cpp rcutils rmw rmw_implementation test_msgs
    )

    ament_add_gtest(test_service${target_suffix}
      test/test_service.cpp
      ENV ${rmw_implementation_env_var}
    )
    target_compile_definitions(test_service${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_service${target_suffix}
      osrf_testing_tools_cpp rcutils rmw rmw_implementation test_msgs
    )

    ament_add_gtest(test_client${target_suffix}
      test/test_client.cpp
      ENV ${rmw_implementation_env_var}
      TIMEOUT 120
    )
    target_compile_definitions(test_client${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_client${target_suffix}
      osrf_testing_tools_cpp rcutils rmw rmw_implementation test_msgs
    )

    ament_add_gtest(test_qos_profile_check_compatible${target_suffix}
      test/test_qos_profile_check_compatible.cpp
      ENV ${rmw_implementation_env_var}
    )
    target_compile_definitions(test_qos_profile_check_compatible${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_qos_profile_check_compatible${target_suffix}
      rmw rmw_implementation
    )

    ament_add_gtest(test_duration_infinite${target_suffix}
      test/test_duration_infinite.cpp
      ENV ${rmw_implementation_env_var}
    )
    target_compile_definitions(test_duration_infinite${target_suffix}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
    ament_target_dependencies(test_duration_infinite${target_suffix}
      osrf_testing_tools_cpp rcutils rmw rmw_implementation test_msgs
    )
  endmacro()

  call_for_each_rmw_implementation(test_api)

  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
