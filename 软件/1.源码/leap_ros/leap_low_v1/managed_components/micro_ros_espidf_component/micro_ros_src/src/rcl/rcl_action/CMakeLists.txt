cmake_minimum_required(VERSION 3.5)

project(rcl_action)

find_package(ament_cmake_ros REQUIRED)
find_package(ament_cmake_gen_version_h REQUIRED)

find_package(action_msgs REQUIRED)
find_package(rcl REQUIRED)
find_package(rcutils REQUIRED)
find_package(rmw REQUIRED)
find_package(rosidl_runtime_c REQUIRED)

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

set(rcl_action_sources
  src/${PROJECT_NAME}/action_client.c
  src/${PROJECT_NAME}/action_server.c
  src/${PROJECT_NAME}/goal_handle.c
  src/${PROJECT_NAME}/goal_state_machine.c
  src/${PROJECT_NAME}/graph.c
  src/${PROJECT_NAME}/names.c
  src/${PROJECT_NAME}/types.c
)

set_source_files_properties(
  ${rcl_action_sources}
  PROPERTIES language "C"
)

add_library(${PROJECT_NAME}
  ${rcl_action_sources}
)

target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

ament_target_dependencies(${PROJECT_NAME}
  "action_msgs"
  "rcl"
  "rcutils"
  "rmw"
  "rosidl_runtime_c"
)

rcl_set_symbol_visibility_hidden(${PROJECT_NAME} LANGUAGE "C")
# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME} PRIVATE "RCL_ACTION_BUILDING_DLL")

install(
  DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  find_package(osrf_testing_tools_cpp REQUIRED)
  find_package(test_msgs REQUIRED)
  ament_lint_auto_find_test_dependencies()
  ament_find_gtest()
  # Gtests
  ament_add_gtest(test_action_client
    test/rcl_action/test_action_client.cpp
    TIMEOUT 360
  )
  if(TARGET test_action_client)
    target_include_directories(test_action_client PUBLIC
      src
    )
    target_link_libraries(test_action_client
      ${PROJECT_NAME}
    )
    ament_target_dependencies(test_action_client
      "osrf_testing_tools_cpp"
      "rcl"
      "test_msgs"
    )
    target_compile_definitions(test_action_client PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
  endif()

  # get the rmw implementations ahead of time
  find_package(rmw_implementation_cmake REQUIRED)
  get_available_rmw_implementations(rmw_implementations)
  foreach(rmw_implementation ${rmw_implementations})
    find_package("${rmw_implementation}" REQUIRED)
  endforeach()

  function(custom_test_c target)
    ament_add_gtest(
      "${target}${target_suffix}" ${ARGN}
      TIMEOUT 180
      ENV
      RCL_ASSERT_RMW_ID_MATCHES=${rmw_implementation}
      RMW_IMPLEMENTATION=${rmw_implementation}
    )
    if(TARGET ${target}${target_suffix})
      target_compile_definitions(${target}${target_suffix}
        PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
        target_compile_definitions(${target}${target_suffix}
          PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
      target_link_libraries(${target}${target_suffix}
        ${PROJECT_NAME}
      )
      ament_target_dependencies(${target}${target_suffix}
        "osrf_testing_tools_cpp"
        "rcl"
        "test_msgs"
      )
    endif()
  endfunction()

  macro(targets)
    custom_test_c(test_action_communication
      "test/rcl_action/test_action_communication.cpp")
    custom_test_c(test_action_interaction
      "test/rcl_action/test_action_interaction.cpp")

    custom_test_c(test_graph
      "test/rcl_action/test_graph.cpp")
  endmacro()

  call_for_each_rmw_implementation(targets)

  ament_add_gtest(test_action_server
    test/rcl_action/test_action_server.cpp
    TIMEOUT 120
  )
  if(TARGET test_action_server)
    target_include_directories(test_action_server PUBLIC
      src
    )
    target_link_libraries(test_action_server
      ${PROJECT_NAME}
    )
    ament_target_dependencies(test_action_server
      "osrf_testing_tools_cpp"
      "rcl"
      "test_msgs"
    )
    target_compile_definitions(test_action_server PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
  endif()
  ament_add_gtest(test_goal_handle
    test/rcl_action/test_goal_handle.cpp
  )
  if(TARGET test_goal_handle)
    target_link_libraries(test_goal_handle
      ${PROJECT_NAME}
    )
    ament_target_dependencies(test_goal_handle "rcl")
  endif()
  ament_add_gtest(test_goal_state_machine
    test/rcl_action/test_goal_state_machine.cpp
  )
  if(TARGET test_goal_state_machine)
    target_link_libraries(test_goal_state_machine
      ${PROJECT_NAME}
    )
    ament_target_dependencies(test_goal_state_machine  "osrf_testing_tools_cpp" "rcl")
  endif()
  ament_add_gtest(test_types
    test/rcl_action/test_types.cpp
  )
  if(TARGET test_types)
    target_link_libraries(test_types
      ${PROJECT_NAME}
    )
    ament_target_dependencies(test_types "rcl")
  endif()
  ament_add_gtest(test_names
    test/rcl_action/test_names.cpp
  )
  if(TARGET test_names)
    target_link_libraries(test_names
      ${PROJECT_NAME}
    )
    ament_target_dependencies(test_names "rcl")
  endif()
  ament_add_gtest(test_wait
    test/rcl_action/test_wait.cpp
  )
  if(TARGET test_wait)
    target_link_libraries(test_wait
      ${PROJECT_NAME}
    )
    target_include_directories(test_wait PUBLIC
      src
    )
    ament_target_dependencies(test_wait
      "osrf_testing_tools_cpp"
      "rcl"
      "test_msgs"
    )
  endif()
endif()

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_libraries(${PROJECT_NAME})

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME})

# specific order: dependents before dependencies
ament_export_dependencies(action_msgs)
ament_export_dependencies(ament_cmake)
ament_export_dependencies(rcl)
ament_export_dependencies(rcutils)
ament_export_dependencies(rmw)
ament_export_dependencies(rosidl_runtime_c)

ament_generate_version_header(${PROJECT_NAME})

ament_package()
