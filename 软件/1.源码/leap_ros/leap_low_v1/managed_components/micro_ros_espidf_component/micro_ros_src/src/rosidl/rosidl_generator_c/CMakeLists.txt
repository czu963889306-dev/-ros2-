cmake_minimum_required(VERSION 3.12)

project(rosidl_generator_c C)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake_python REQUIRED)
find_package(ament_cmake_ros REQUIRED)

ament_export_dependencies(rosidl_cmake)
ament_export_dependencies(rosidl_typesupport_interface)
ament_index_register_resource("rosidl_generator_packages")

ament_python_install_package(${PROJECT_NAME})

if(BUILD_SHARED_LIBS)
  set(${PROJECT_NAME}_LIBRARY_TYPE "SHARED")
else()
  set(${PROJECT_NAME}_LIBRARY_TYPE "STATIC")
endif()

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(test_interface_files REQUIRED)
  find_package(rosidl_runtime_c REQUIRED)
  ament_lint_auto_find_test_dependencies()

  include(cmake/register_c.cmake)
  # Trick ament_target_dependencies() into thinking this package has been found
  set(rosidl_generator_c_FOUND "1")
  set(rosidl_generator_c_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

  rosidl_generator_c_extras(
    "${CMAKE_CURRENT_SOURCE_DIR}/bin/rosidl_generator_c"
    "${CMAKE_CURRENT_SOURCE_DIR}/rosidl_generator_c/__init__.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/resource"
  )

  rosidl_generate_interfaces(${PROJECT_NAME}_interfaces
    ${test_interface_files_MSG_FILES}
    ADD_LINTER_TESTS
    SKIP_INSTALL
  )

  add_executable(test_compilation_c test/test_compilation.c test/separate_compilation.c)
  add_executable(test_interfaces_c test/test_interfaces.c)
  add_executable(test_invalid_initialization_c test/test_invalid_initialization.c)

  ament_add_test(
    test_compilation_c
    COMMAND "$<TARGET_FILE:test_compilation_c>"
    GENERATE_RESULT_FOR_RETURN_CODE_ZERO
  )
  ament_add_test(
    test_interfaces_c
    COMMAND "$<TARGET_FILE:test_interfaces_c>"
    GENERATE_RESULT_FOR_RETURN_CODE_ZERO
  )
  ament_add_test(
    test_invalid_initialization_c
    COMMAND "$<TARGET_FILE:test_invalid_initialization_c>"
    GENERATE_RESULT_FOR_RETURN_CODE_ZERO
  )

  # include the built files directly, instead of their install location
  target_link_libraries(test_compilation_c
    ${PROJECT_NAME}_interfaces__${PROJECT_NAME}
    ${rosidl_runtime_c_LIBRARIES}
  )
  target_include_directories(test_compilation_c PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    ${rosidl_runtime_c_INCLUDE_DIRS}
  )
  target_link_libraries(test_interfaces_c
    ${PROJECT_NAME}_interfaces__${PROJECT_NAME}
    ${rosidl_runtime_c_LIBRARIES}
  )
  target_include_directories(test_interfaces_c PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    ${rosidl_runtime_c_INCLUDE_DIRS}
  )
  target_link_libraries(test_invalid_initialization_c
    ${PROJECT_NAME}_interfaces__${PROJECT_NAME}
    ${rosidl_runtime_c_LIBRARIES}
  )
  target_include_directories(test_invalid_initialization_c PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    ${rosidl_runtime_c_INCLUDE_DIRS}
  )
endif()

ament_package(
  CONFIG_EXTRAS "rosidl_generator_c-extras.cmake.in"
)

install(
  PROGRAMS bin/rosidl_generator_c
  DESTINATION lib/rosidl_generator_c
)
install(
  DIRECTORY cmake resource
  DESTINATION share/${PROJECT_NAME}
)
