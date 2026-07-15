cmake_minimum_required(VERSION 3.8)
project(rosidl_typesupport_introspection_tests)

find_package(ament_cmake REQUIRED)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
  endif()

  find_package(rosidl_cmake REQUIRED)
  find_package(rosidl_generator_c REQUIRED)
  find_package(rosidl_generator_cpp REQUIRED)
  find_package(rosidl_typesupport_introspection_c REQUIRED)
  find_package(rosidl_typesupport_introspection_cpp REQUIRED)
  find_package(test_interface_files REQUIRED)

  # Drop BoundedPlainSequences as BoundedSequences
  # already encompasses all the same member types
  list(
    FILTER test_interface_files_MSG_FILES
    EXCLUDE REGEX ".*BoundedPlainSequences.*")
  rosidl_generate_interfaces(${PROJECT_NAME}
    ${test_interface_files_MSG_FILES}
    ${test_interface_files_SRV_FILES}
    # ${test_interface_files_ACTION_FILES}
    SKIP_INSTALL
  )

  find_package(rcutils REQUIRED)
  find_package(rcpputils REQUIRED)
  find_package(rosidl_typesupport_interface REQUIRED)

  add_library(${PROJECT_NAME}_library INTERFACE)
  target_include_directories(${PROJECT_NAME}_library INTERFACE include/)
  target_link_libraries(${PROJECT_NAME}_library INTERFACE
    rcutils::rcutils
    rcpputils::rcpputils
    rosidl_typesupport_interface::rosidl_typesupport_interface
    rosidl_typesupport_introspection_c::rosidl_typesupport_introspection_c
    rosidl_typesupport_introspection_cpp::rosidl_typesupport_introspection_cpp)
  target_compile_features(${PROJECT_NAME}_library INTERFACE cxx_std_17)

  find_package(ament_cmake_gtest REQUIRED)
  file(GLOB test_files test/test_*.cpp)
  foreach(test_file ${test_files})
    get_filename_component(target ${test_file} NAME_WE)

    ament_add_gtest(${target} ${test_file}
      # Ensure typesupport introspection libraries can be found
      APPEND_LIBRARY_DIRS "${CMAKE_CURRENT_BINARY_DIR}"
    )

    if(TARGET ${target})
      target_include_directories(${target} PRIVATE test)
      target_link_libraries(${target}
        ${PROJECT_NAME}_library
        ${PROJECT_NAME}__rosidl_generator_c
        ${PROJECT_NAME}__rosidl_generator_cpp
        ${PROJECT_NAME}__rosidl_typesupport_introspection_c
        ${PROJECT_NAME}__rosidl_typesupport_introspection_cpp)
    endif()
  endforeach()
endif()

ament_package()
