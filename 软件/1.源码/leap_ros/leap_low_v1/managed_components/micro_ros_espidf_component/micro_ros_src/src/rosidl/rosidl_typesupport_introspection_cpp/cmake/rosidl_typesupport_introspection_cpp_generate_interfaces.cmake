# Copyright 2014-2018 Open Source Robotics Foundation, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

if(NOT TARGET ${rosidl_generate_interfaces_TARGET}__rosidl_generator_cpp)
  message(FATAL_ERROR
    "The 'rosidl_generator_cpp' extension must be executed before the "
    "'rosidl_typesupport_introspection_cpp' extension.")
endif()

find_package(rosidl_runtime_c REQUIRED)
find_package(rosidl_typesupport_interface REQUIRED)
find_package(rosidl_typesupport_introspection_cpp REQUIRED)

set(_output_path
  "${CMAKE_CURRENT_BINARY_DIR}/rosidl_typesupport_introspection_cpp/${PROJECT_NAME}")
set(_generated_header_files "")
set(_generated_source_files "")
foreach(_abs_idl_file ${rosidl_generate_interfaces_ABS_IDL_FILES})
  get_filename_component(_parent_folder "${_abs_idl_file}" DIRECTORY)
  get_filename_component(_parent_folder "${_parent_folder}" NAME)
  get_filename_component(_idl_name "${_abs_idl_file}" NAME_WE)
  string_camel_case_to_lower_case_underscore("${_idl_name}" _header_name)
  list(APPEND _generated_header_files
    "${_output_path}/${_parent_folder}/detail/${_header_name}__rosidl_typesupport_introspection_cpp.hpp")
  list(APPEND _generated_source_files
    "${_output_path}/${_parent_folder}/detail/${_header_name}__type_support.cpp")
endforeach()

set(_dependency_files "")
set(_dependencies "")
foreach(_pkg_name ${rosidl_generate_interfaces_DEPENDENCY_PACKAGE_NAMES})
  foreach(_idl_file ${${_pkg_name}_IDL_FILES})
    set(_abs_idl_file "${${_pkg_name}_DIR}/../${_idl_file}")
    normalize_path(_abs_idl_file "${_abs_idl_file}")
    list(APPEND _dependency_files "${_abs_idl_file}")
    list(APPEND _dependencies "${_pkg_name}:${_abs_idl_file}")
  endforeach()
endforeach()

set(target_dependencies
  "${rosidl_typesupport_introspection_cpp_BIN}"
  ${rosidl_typesupport_introspection_cpp_GENERATOR_FILES}
  "${rosidl_typesupport_introspection_cpp_TEMPLATE_DIR}/idl__rosidl_typesupport_introspection_cpp.hpp.em"
  "${rosidl_typesupport_introspection_cpp_TEMPLATE_DIR}/idl__type_support.cpp.em"
  "${rosidl_typesupport_introspection_cpp_TEMPLATE_DIR}/msg__rosidl_typesupport_introspection_cpp.hpp.em"
  "${rosidl_typesupport_introspection_cpp_TEMPLATE_DIR}/msg__type_support.cpp.em"
  "${rosidl_typesupport_introspection_cpp_TEMPLATE_DIR}/srv__rosidl_typesupport_introspection_cpp.hpp.em"
  "${rosidl_typesupport_introspection_cpp_TEMPLATE_DIR}/srv__type_support.cpp.em"
  ${rosidl_generate_interfaces_ABS_IDL_FILES}
  ${_dependency_files})
foreach(dep ${target_dependencies})
  if(NOT EXISTS "${dep}")
    message(FATAL_ERROR "Target dependency '${dep}' does not exist")
  endif()
endforeach()

set(generator_arguments_file "${CMAKE_CURRENT_BINARY_DIR}/rosidl_typesupport_introspection_cpp__arguments.json")
rosidl_write_generator_arguments(
  "${generator_arguments_file}"
  PACKAGE_NAME "${PROJECT_NAME}"
  IDL_TUPLES "${rosidl_generate_interfaces_IDL_TUPLES}"
  ROS_INTERFACE_DEPENDENCIES "${_dependencies}"
  OUTPUT_DIR "${_output_path}"
  TEMPLATE_DIR "${rosidl_typesupport_introspection_cpp_TEMPLATE_DIR}"
  TARGET_DEPENDENCIES ${target_dependencies}
)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_custom_command(
  OUTPUT ${_generated_header_files} ${_generated_source_files}
  COMMAND Python3::Interpreter
  ARGS ${rosidl_typesupport_introspection_cpp_BIN}
  --generator-arguments-file "${generator_arguments_file}"
  DEPENDS ${target_dependencies}
  COMMENT "Generating C++ introspection for ROS interfaces"
  VERBATIM
)

set(_target_suffix "__rosidl_typesupport_introspection_cpp")

add_library(${rosidl_generate_interfaces_TARGET}${_target_suffix} ${rosidl_typesupport_introspection_cpp_LIBRARY_TYPE}
  ${_generated_header_files} ${_generated_source_files})
add_library(${PROJECT_NAME}::${rosidl_generate_interfaces_TARGET}${_target_suffix} ALIAS
  ${rosidl_generate_interfaces_TARGET}${_target_suffix})
if(rosidl_generate_interfaces_LIBRARY_NAME)
  set_target_properties(${rosidl_generate_interfaces_TARGET}${_target_suffix}
    PROPERTIES OUTPUT_NAME "${rosidl_generate_interfaces_LIBRARY_NAME}${_target_suffix}")
endif()
set_property(TARGET ${rosidl_generate_interfaces_TARGET}${_target_suffix}
  PROPERTY DEFINE_SYMBOL "ROSIDL_TYPESUPPORT_INTROSPECTION_CPP_BUILDING_DLL")
set_property(TARGET ${rosidl_generate_interfaces_TARGET}${_target_suffix}
  PROPERTY CXX_STANDARD 17)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(${rosidl_generate_interfaces_TARGET}${_target_suffix}
    PRIVATE -Wall -Wextra -Wpedantic)
endif()

target_include_directories(${rosidl_generate_interfaces_TARGET}${_target_suffix}
  PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/rosidl_typesupport_introspection_cpp>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

# Depend on the library created by rosidl_generator_cpp
target_link_libraries(${rosidl_generate_interfaces_TARGET}${_target_suffix} PUBLIC
  ${rosidl_generate_interfaces_TARGET}__rosidl_generator_cpp
)

target_link_libraries(${rosidl_generate_interfaces_TARGET}${_target_suffix} PUBLIC
  rosidl_runtime_c::rosidl_runtime_c
  rosidl_typesupport_interface::rosidl_typesupport_interface
  rosidl_typesupport_introspection_cpp::rosidl_typesupport_introspection_cpp)

foreach(_pkg_name ${rosidl_generate_interfaces_DEPENDENCY_PACKAGE_NAMES})
  # Depend on targets generated by this generator in dependency packages
  target_link_libraries(
    ${rosidl_generate_interfaces_TARGET}${_target_suffix} PUBLIC
    ${${_pkg_name}_TARGETS${_target_suffix}})
endforeach()

# Make top level generation target depend on this generated library
add_dependencies(
  ${rosidl_generate_interfaces_TARGET}
  ${rosidl_generate_interfaces_TARGET}${_target_suffix}
)

if(NOT rosidl_generate_interfaces_SKIP_INSTALL)
  install(
    DIRECTORY ${_output_path}/
    DESTINATION "include/${PROJECT_NAME}/${PROJECT_NAME}"
    PATTERN "*.hpp"
  )

  install(
    TARGETS ${rosidl_generate_interfaces_TARGET}${_target_suffix}
    EXPORT ${rosidl_generate_interfaces_TARGET}${_target_suffix}
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
  )

  rosidl_export_typesupport_targets(${_target_suffix}
    ${rosidl_generate_interfaces_TARGET}${_target_suffix})
  ament_export_targets(${rosidl_generate_interfaces_TARGET}${_target_suffix})
endif()

if(BUILD_TESTING AND rosidl_generate_interfaces_ADD_LINTER_TESTS)
  find_package(ament_cmake_cppcheck REQUIRED)
  ament_cppcheck(
    TESTNAME "cppcheck_rosidl_typesupport_introspection_cpp"
    "${_output_path}")

  find_package(ament_cmake_cpplint REQUIRED)
  get_filename_component(_cpplint_root "${_output_path}" DIRECTORY)
  ament_cpplint(
    TESTNAME "cpplint_rosidl_typesupport_introspection_cpp"
    # the generated code might contain longer lines for templated types
    MAX_LINE_LENGTH 999
    ROOT "${_cpplint_root}"
    "${_output_path}")

  find_package(ament_cmake_uncrustify REQUIRED)
  ament_uncrustify(
    TESTNAME "uncrustify_rosidl_typesupport_introspection_cpp"
    # the generated code might contain longer lines for templated types
    # a value of zero tells uncrustify to ignore line length
    MAX_LINE_LENGTH 0
    "${_output_path}")
endif()
