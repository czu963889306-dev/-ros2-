cmake_minimum_required(VERSION 3.5)

project(rosidl_typesupport_c)

if(BUILD_SHARED_LIBS)
  set(${PROJECT_NAME}_LIBRARY_TYPE "SHARED")
else()
  set(${PROJECT_NAME}_LIBRARY_TYPE "STATIC")
endif()

if(rosidl_typesupport_c_LIBRARY_TYPE STREQUAL "STATIC"
  AND NOT BUILD_TESTING
)
  set(ROSIDL_TYPESUPPORT_STATIC_TYPESUPPORT ON)
else()
  set(ROSIDL_TYPESUPPORT_STATIC_TYPESUPPORT OFF)
endif()

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

find_package(ament_cmake_ros REQUIRED)
find_package(rcutils REQUIRED)
find_package(rosidl_runtime_c REQUIRED)
if(NOT ROSIDL_TYPESUPPORT_STATIC_TYPESUPPORT)
  find_package(rcpputils REQUIRED)
endif()

ament_export_dependencies(rcutils)
ament_export_dependencies(rosidl_runtime_c)
ament_export_dependencies(rosidl_typesupport_interface)
if(NOT ROSIDL_TYPESUPPORT_STATIC_TYPESUPPORT)
  ament_export_dependencies(rcpputils)
endif()

ament_python_install_package(${PROJECT_NAME})

add_library(${PROJECT_NAME}
  src/identifier.c
  src/message_type_support_dispatch.cpp
  src/service_type_support_dispatch.cpp)
if(WIN32)
  target_compile_definitions(${PROJECT_NAME}
    PRIVATE "ROSIDL_TYPESUPPORT_C_BUILDING_DLL")
endif()
target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    $<$<BOOL:${ROSIDL_TYPESUPPORT_STATIC_TYPESUPPORT}>:ROSIDL_TYPESUPPORT_STATIC_TYPESUPPORT>
)
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

target_link_libraries(${PROJECT_NAME} PUBLIC
  rosidl_runtime_c::rosidl_runtime_c)
target_link_libraries(${PROJECT_NAME} PRIVATE
  rcutils::rcutils)

if(NOT ROSIDL_TYPESUPPORT_STATIC_TYPESUPPORT)
  target_link_libraries(${PROJECT_NAME} PRIVATE
    rcpputils::rcpputils)
endif()
# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")
ament_export_libraries(${PROJECT_NAME})

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME})

ament_index_register_resource("rosidl_runtime_packages")

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(performance_test_fixture REQUIRED)
  # Give cppcheck hints about macro definitions coming from outside this package
  get_target_property(ament_cmake_cppcheck_ADDITIONAL_INCLUDE_DIRS
    performance_test_fixture::performance_test_fixture INTERFACE_INCLUDE_DIRECTORIES)

  set(TEST_LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/test_libs")

  add_library(rosidl_typesupport_c__test_type_support1
    test/test_type_support.c)
  target_link_libraries(rosidl_typesupport_c__test_type_support1 ${PROJECT_NAME})
  add_custom_command(TARGET rosidl_typesupport_c__test_type_support1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:rosidl_typesupport_c__test_type_support1>
    ${TEST_LIB_DIR})

  # This same library is added a second type with a new name for additional tests
  add_library(rosidl_typesupport_c__test_type_support2
    test/test_type_support.c)
  target_link_libraries(rosidl_typesupport_c__test_type_support2 ${PROJECT_NAME})
  add_custom_command(TARGET rosidl_typesupport_c__test_type_support2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:rosidl_typesupport_c__test_type_support2>
    ${TEST_LIB_DIR})

  ament_add_gtest(test_message_type_support test/test_message_type_support_dispatch.cpp
    APPEND_ENV
    "DYLD_LIBRARY_PATH=${TEST_LIB_DIR}"
    "LD_LIBRARY_PATH=${TEST_LIB_DIR}"
    "PATH=${TEST_LIB_DIR}")
  if(TARGET test_message_type_support)
    target_link_libraries(test_message_type_support
      ${PROJECT_NAME}
      mimick
      rcpputils::rcpputils
    )
    target_compile_definitions(test_message_type_support PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
  endif()

  ament_add_gtest(test_service_type_support test/test_service_type_support_dispatch.cpp
    APPEND_ENV
    "DYLD_LIBRARY_PATH=${TEST_LIB_DIR}"
    "LD_LIBRARY_PATH=${TEST_LIB_DIR}"
    "PATH=${TEST_LIB_DIR}")
  if(TARGET test_service_type_support)
    target_link_libraries(test_service_type_support
      ${PROJECT_NAME}
      mimick
      rcpputils::rcpputils
    )
    target_compile_definitions(test_service_type_support PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
  endif()

  # Test type_support_dispatch throws runtime error when trying to load this "library"
  file(GENERATE
    OUTPUT
    "${TEST_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}rosidl_typesupport_c__test_type_support3${CMAKE_SHARED_LIBRARY_SUFFIX}"
    CONTENT "I'm not a shared library, why would you treat me like one?")

  add_performance_test(benchmark_type_support_dispatch test/benchmark/benchmark_type_support_dispatch.cpp
    APPEND_ENV
    "DYLD_LIBRARY_PATH=${TEST_LIB_DIR}"
    "LD_LIBRARY_PATH=${TEST_LIB_DIR}"
    "PATH=${TEST_LIB_DIR}")
  if(TARGET benchmark_type_support_dispatch)
    target_link_libraries(benchmark_type_support_dispatch
      ${PROJECT_NAME}
      rcpputils::rcpputils)
  endif()

  find_package(ament_cmake_pytest REQUIRED)
  ament_add_pytest_test(test_cli_extension test/test_cli_extension.py)
endif()

ament_package(
  CONFIG_EXTRAS "rosidl_typesupport_c-extras.cmake.in"
)

install(
  PROGRAMS bin/rosidl_typesupport_c
  DESTINATION lib/rosidl_typesupport_c
)
install(
  DIRECTORY cmake resource
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
