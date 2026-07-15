# Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

find_package(ament_cmake_ros REQUIRED)
find_package(microcdr REQUIRED)
find_package(rmw REQUIRED)

set(_output_path "${CMAKE_CURRENT_BINARY_DIR}/rosidl_typesupport_microxrcedds_cpp/${PROJECT_NAME}")

set(_generated_files "")
foreach(_abs_idl_file ${rosidl_generate_interfaces_ABS_IDL_FILES})
  get_filename_component(_parent_folder "${_abs_idl_file}" DIRECTORY)
  get_filename_component(_parent_folder "${_parent_folder}" NAME)
  get_filename_component(_idl_name "${_abs_idl_file}" NAME_WE)
  string_camel_case_to_lower_case_underscore("${_idl_name}" _header_name)
  list(APPEND _generated_files
    "${_output_path}/${_parent_folder}/detail/microxrcedds/${_header_name}__type_support_cpp.cpp"
    "${_output_path}/${_parent_folder}/detail/${_header_name}__rosidl_typesupport_microxrcedds_cpp.hpp"
    )
endforeach()

set(_dependency_files "")
set(_dependencies "")
foreach(_pck_name ${rosidl_generated_interfaces_DEPENDENCY_PACKAGE_NAMES})
  foreach(_idl_file ${${_pkg_name}_IDL_FILES})
    set(_abs_idl_file "${${_pkg_name}_DIR}/../${_idl_file}")
    normalize_path(_abs_idl_file "${_abs_idl_file}")
    list(APPEND _dependency_files "${_abs_idl_file}")
    list(APPEND _dependencies "${_pkg_name}:${_abs_idl_file}")
  endforeach()
endforeach()

# Check if all templates exits
set(target_dependencies
  "${rosidl_typesupport_microxrcedds_cpp_BIN}"
  ${rosidl_typesupport_microxrcedds_cpp_GENERATOR_FILES}
  "${rosidl_typesupport_microxrcedds_cpp_TEMPLATE_DIR}/idl__rosidl_typesupport_microxrcedds_cpp.hpp.em"
  "${rosidl_typesupport_microxrcedds_cpp_TEMPLATE_DIR}/idl__type_support_cpp.cpp.em"
  "${rosidl_typesupport_microxrcedds_cpp_TEMPLATE_DIR}/msg__rosidl_typesupport_microxrcedds_cpp.hpp.em"
  "${rosidl_typesupport_microxrcedds_cpp_TEMPLATE_DIR}/msg__type_support_cpp.cpp.em"
  "${rosidl_typesupport_microxrcedds_cpp_TEMPLATE_DIR}/srv__rosidl_typesupport_microxrcedds_cpp.hpp.em"
  "${rosidl_typesupport_microxrcedds_cpp_TEMPLATE_DIR}/srv__type_support_cpp.cpp.em"
  ${rosidl_generate_interfaces_ABS_IDL_FILES}
  ${_dependency_files})
foreach(dep ${target_dependencies})
  if(NOT EXISTS "${dep}")
    message(FATAL_ERROR "Target dependency '${dep}' does not exist")
  endif()
endforeach()


# Generate script argument file
set(generator_arguments_file "${CMAKE_CURRENT_BINARY_DIR}/rosidl_typesupport_microxrcedds_cpp__arguments.json")
rosidl_write_generator_arguments(
  "${generator_arguments_file}"
  PACKAGE_NAME "${PROJECT_NAME}"
  IDL_TUPLES "${rosidl_generate_interfaces_IDL_TUPLES}"
  ROS_INTERFACE_DEPENDENCIES "${_dependencies}"
  OUTPUT_DIR "${_output_path}"
  TEMPLATE_DIR "${rosidl_typesupport_microxrcedds_cpp_TEMPLATE_DIR}"
  TARGET_DEPENDENCIES ${target_dependencies}
  )

find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Execute python script
add_custom_command(
  OUTPUT
    ${_generated_files}
  COMMAND
    Python3::Interpreter
  ARGS
    ${rosidl_typesupport_microxrcedds_cpp_BIN}
    --generator-arguments-file "${generator_arguments_file}"
  DEPENDS
    ${target_dependencies} ${_dds_idl_files}
  COMMENT
    "Generating C++ type support for eProsima Micro XRCE-DDS"
  VERBATIM
  )

# Generate header to switch between export and import for a specific package
set(_visibility_control_file
"${_output_path}/msg/rosidl_typesupport_microxrcedds_cpp__visibility_control.h")
string(TOUPPER "${PROJECT_NAME}" PROJECT_NAME_UPPER)
configure_file(
  "${rosidl_typesupport_microxrcedds_cpp_TEMPLATE_DIR}/rosidl_typesupport_microxrcedds_cpp__visibility_control.h.in"
  "${_visibility_control_file}"
  @ONLY
  )

set(_target_suffix "__rosidl_typesupport_microxrcedds_cpp")

# Generate Micro XRCE-DDS typesupport shared library
add_library(${rosidl_generate_interfaces_TARGET}${_target_suffix}
  ${rosidl_typesupport_microxrcedds_c_LIBRARY_TYPE}
  ${_generated_files}
  )

# Change output library name if asked to
if(rosidl_generate_interfaces_LIBRARY_NAME)
  set_target_properties(${rosidl_generate_interfaces_TARGET}${_target_suffix} PROPERTIES
    OUTPUT_NAME
      "${rosidl_generate_interfaces_LIBRARY_NAME}${_target_suffix}"
    )
endif()

# Set C++ standard
set_target_properties(${rosidl_generate_interfaces_TARGET}${_target_suffix} PROPERTIES
  CXX_STANDARD
    14
  CXX_STANDARD_REQUIRED
    YES
  POSITION_INDEPENDENT_CODE
    YES
  )

# Set flag for visibility macro
target_compile_definitions(${rosidl_generate_interfaces_TARGET}${_target_suffix}
  PRIVATE
      $<$<PLATFORM_ID:Windows>:"ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP_BUILDING_DLL_${PROJECT_NAME}">
  )

# Set compiler flags
target_compile_options(${rosidl_generate_interfaces_TARGET}${_target_suffix}
  PRIVATE
    $<$<PLATFORM_ID:Linux>:-Wall>
    $<$<PLATFORM_ID:Linux>:-Wextra>
    $<$<PLATFORM_ID:Linux>:-Wpedantic>
    $<$<PLATFORM_ID:Windows>:/W4>
  )

# Include headers from other generators
target_include_directories(${rosidl_generate_interfaces_TARGET}${_target_suffix}
  PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/rosidl_generator_cpp
    ${CMAKE_CURRENT_BINARY_DIR}/rosidl_typesupport_microxrcedds_cpp
  )

ament_target_dependencies(${rosidl_generate_interfaces_TARGET}${_target_suffix}
  "microcdr"
  "rmw"
  "rosidl_runtime_cpp"
  "rosidl_typesupport_microxrcedds_cpp"
  "rosidl_typesupport_interface"
  )

foreach(_pkg_name ${rosidl_generate_interfaces_DEPENDENCY_PACKAGE_NAMES})
  ament_target_dependencies(${rosidl_generate_interfaces_TARGET}${_target_suffix}
    ${_pkg_name}
    )
  target_link_libraries(${rosidl_generate_interfaces_TARGET}${_target_suffix}
    ${${_pkg_name}_LIBRARIES${_target_suffix}}
    )
endforeach()

target_link_libraries(
  ${rosidl_generate_interfaces_TARGET}${_target_suffix}
  ${rosidl_generate_interfaces_TARGET}__rosidl_generator_cpp
  microcdr
  )

# Make top level generation target depend on this library
add_dependencies(
  ${rosidl_generate_interfaces_TARGET}
  ${rosidl_generate_interfaces_TARGET}${_target_suffix}
  )

# Make this library depend on target created by rosidl_generator_cpp
add_dependencies(
  ${rosidl_generate_interfaces_TARGET}${_target_suffix}
  ${rosidl_generate_interfaces_TARGET}__cpp
  )

if(NOT rosidl_generate_interfaces_SKIP_INSTALL)
  install(
    DIRECTORY
      "${_output_path}/"
    DESTINATION
      "include/${PROJECT_NAME}/${PROJECT_NAME}"
    PATTERN
      "*.cpp" EXCLUDE
    )

  if(NOT _generated_files STREQUAL "")
    ament_export_include_directories(include)
  endif()

  install(
    TARGETS
      ${rosidl_generate_interfaces_TARGET}${_target_suffix}
    ARCHIVE DESTINATION
      lib
    LIBRARY DESTINATION
      lib
    RUNTIME DESTINATION
      bin
  )

  rosidl_export_typesupport_libraries(${_target_suffix}
    ${rosidl_generate_interfaces_TARGET}${_target_suffix}
    )
endif()


# test
if(BUILD_TESTING AND rosidl_generate_interfaces_ADD_LINTER_TESTS)
  if(NOT _generated_files STREQUAL "")
    find_package(ament_cmake_cppcheck REQUIRED)
    ament_cppcheck(
      TESTNAME
        "cppcheck_rosidl_typesupport_microxrcedds_cpp"
      ${_generated_files}
      )

    find_package(ament_cmake_cpplint REQUIRED)
    get_filename_component(_cpplint_root "${_output_path}" DIRECTORY)
    ament_cpplint(
      TESTNAME
        "cpplint_rosidl_typesupport_microxrcedds_cpp"
      MAX_LINE_LENGTH
        999
      ROOT
        "${_cpplint_root}"
      ${_output_path}
      )

    find_package(ament_cmake_uncrustify REQUIRED)
    ament_uncrustify(
      TESTNAME
        "uncrustify_rosidl_typesupport_microxrcedds_cpp"
      MAX_LINE_LENGTH
        0
      ${_output_path}
      )
  endif()
endif()
