cmake_minimum_required(VERSION 3.5)

project(rcl_yaml_param_parser)

find_package(ament_cmake_ros REQUIRED)
find_package(ament_cmake_gen_version_h REQUIRED)
find_package(rcutils REQUIRED)
find_package(rmw REQUIRED)
find_package(libyaml_vendor REQUIRED)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  add_compile_options(
    -Wall -Wextra -Wpedantic
    -Wformat=2 -Wconversion -Wsign-conversion
  )
endif()

set(rcl_yaml_parser_sources
  src/add_to_arrays.c
  src/namespace.c
  src/node_params.c
  src/parse.c
  src/parser.c
  src/yaml_variant.c
)

add_library(
  ${PROJECT_NAME}
  ${rcl_yaml_parser_sources})
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")
ament_target_dependencies(${PROJECT_NAME} "libyaml_vendor" "rcutils" "rmw")

# Set the visibility to hidden by default if possible
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  # Set the visibility of symbols to hidden by default for gcc and clang
  # (this is already the default on Windows)
  set_target_properties(${PROJECT_NAME}
    PROPERTIES
    COMPILE_FLAGS "-fvisibility=hidden"
  )
endif()

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME} PRIVATE "RCL_YAML_PARAM_PARSER_BUILDING_DLL")

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  find_package(osrf_testing_tools_cpp REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(performance_test_fixture REQUIRED)
  # Give cppcheck hints about macro definitions coming from outside this package
  get_target_property(ament_cmake_cppcheck_ADDITIONAL_INCLUDE_DIRS
    performance_test_fixture::performance_test_fixture INTERFACE_INCLUDE_DIRECTORIES)

  # Gtests
  ament_add_gtest(test_namespace
    test/test_namespace.cpp
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  if(TARGET test_namespace)
    ament_target_dependencies(test_namespace
      "rcutils"
      "osrf_testing_tools_cpp"
    )
    target_compile_definitions(test_namespace PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
    target_link_libraries(test_namespace ${PROJECT_NAME})
  endif()

  ament_add_gtest(test_node_params
    test/test_node_params.cpp
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  if(TARGET test_node_params)
    ament_target_dependencies(test_node_params
      "rcutils"
      "osrf_testing_tools_cpp"
    )
    target_link_libraries(test_node_params ${PROJECT_NAME})
  endif()

  ament_add_gtest(test_parse_yaml
    test/test_parse_yaml.cpp
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  if(TARGET test_parse_yaml)
    target_link_libraries(test_parse_yaml ${PROJECT_NAME})
    target_include_directories(test_parse_yaml
      PRIVATE ${osrf_testing_tools_cpp_INCLUDE_DIRS})
  endif()

  ament_add_gtest(test_parse
    test/test_parse.cpp
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  if(TARGET test_parse)
    ament_target_dependencies(test_parse
      "rcutils"
      "osrf_testing_tools_cpp"
    )
    target_link_libraries(test_parse ${PROJECT_NAME} mimick)
  endif()

  ament_add_gtest(test_parser
    test/test_parser.cpp
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  if(TARGET test_parser)
    ament_target_dependencies(test_parser
      "rcutils"
      "osrf_testing_tools_cpp"
    )
    target_link_libraries(test_parser ${PROJECT_NAME} mimick)
    target_compile_definitions(test_parser PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
  endif()

  ament_add_gtest(test_parser_multiple_nodes
    test/test_parser_multiple_nodes.cpp
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    TIMEOUT 240  # Large timeout to wait for fault injection tests
  )
  if(TARGET test_parser_multiple_nodes)
    ament_target_dependencies(test_parser_multiple_nodes
      "rcutils"
      "osrf_testing_tools_cpp"
    )
    target_link_libraries(test_parser_multiple_nodes ${PROJECT_NAME} mimick)
    target_compile_definitions(test_parser_multiple_nodes PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
  endif()

  ament_add_gtest(test_parser_multiple_params
    test/test_parser_multiple_params.cpp
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  if(TARGET test_parser_multiple_params)
    ament_target_dependencies(test_parser_multiple_params
      "rcutils"
      "osrf_testing_tools_cpp"
    )
    target_link_libraries(test_parser_multiple_params ${PROJECT_NAME} mimick)
    target_compile_definitions(test_parser_multiple_params PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
  endif()

  ament_add_gtest(test_yaml_variant
    test/test_yaml_variant.cpp
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  if(TARGET test_yaml_variant)
    ament_target_dependencies(test_yaml_variant
      "rcutils"
      "osrf_testing_tools_cpp"
    )
    target_link_libraries(test_yaml_variant ${PROJECT_NAME})
  endif()

  add_performance_test(benchmark_parse_yaml test/benchmark/benchmark_parse_yaml.cpp
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
  if(TARGET benchmark_parse_yaml)
    target_link_libraries(benchmark_parse_yaml ${PROJECT_NAME})
    ament_target_dependencies(benchmark_parse_yaml
      "rcpputils"
      "rcutils"
    )
  endif()

  add_performance_test(benchmark_variant test/benchmark/benchmark_variant.cpp)
  if(TARGET benchmark_variant)
    target_link_libraries(benchmark_variant ${PROJECT_NAME})
    ament_target_dependencies(benchmark_variant
      "rcutils"
    )
  endif()
endif()

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_libraries(${PROJECT_NAME})

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME})

ament_export_dependencies(ament_cmake libyaml_vendor)
ament_export_dependencies(rmw)

ament_generate_version_header(${PROJECT_NAME})

install(
  DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)
ament_package()
