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

#
# Generate code for ROS IDL files using all available generators.
#
# Execute the extension point ``rosidl_generate_interfaces``.
#
# :param target: the _name of the generation target,
#   specific generators might use the _name as a prefix for their own
#   generation step
# :type target: string
# :param ARGN: the interface file containing message, service, and action
#   definitions where each value might be either a path relative to the
#   CMAKE_CURRENT_SOURCE_DIR or a tuple separated by a colon with an absolute
#   base path and a path relative to that base path.
#   If the interface file's parent directory is 'action', it is assumed to be
#   an action definition.
#   If an action interface is passed then you must add a depend tag for
#   'action_msgs' to your package.xml, otherwise this macro will error.
#   For backward compatibility if an interface file doesn't end in ``.idl`` it
#   is being passed to ``rosidl_adapter`` (if available) to be transformed into
#   an ``.idl`` file.
# :type ARGN: list of strings
# :param DEPENDENCIES: the packages from which message types are
#   being used
# :type DEPENDENCIES: list of strings
# :param LIBRARY_NAME: the base name of the library, specific generators might
#   append their own suffix
# :type LIBRARY_NAME: string
# :param SKIP_INSTALL: if set skip installing the interface files
# :type SKIP_INSTALL: option
# :param SKIP_GROUP_MEMBERSHIP_CHECK: if set, skip enforcing the appartenance
#   to the rosidl_interface_packages group
# :type SKIP_GROUP_MEMBERSHIP_CHECK: option
# :param ADD_LINTER_TESTS: if set lint the interface files using
#   the ``ament_lint`` package
# :type ADD_LINTER_TESTS: option
#
# @public
#
macro(rosidl_generate_interfaces target)
  cmake_parse_arguments(_ARG
    "ADD_LINTER_TESTS;SKIP_INSTALL;SKIP_GROUP_MEMBERSHIP_CHECK"
    "LIBRARY_NAME" "DEPENDENCIES"
    ${ARGN})
  if(NOT _ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "rosidl_generate_interfaces() called without any "
      "interface files")
  endif()

  if(_${PROJECT_NAME}_AMENT_PACKAGE)
    message(FATAL_ERROR
      "rosidl_generate_interfaces() must be called before ament_package()")
  endif()

  _rosidl_cmake_register_package_hook()
  ament_export_dependencies(${_ARG_DEPENDENCIES})

  # check that passed interface files exist
  # a tuple with an absolute base and a relative path is returned as is
  # a relative path is returned as colon separated tuple
  # of the base path and the relative path
  set(_interface_tuples "")
  foreach(_file ${_ARG_UNPARSED_ARGUMENTS})
    if(IS_ABSOLUTE "${_file}")
      string(FIND "${_file}" ":" _index)
      if(_index EQUAL -1)
        message(FATAL_ERROR "rosidl_generate_interfaces() the passed absolute "
          "file '${_file}' must be represented as an absolute base path "
          "separated by a colon from the relative path to the interface file")
      endif()
      string(REGEX REPLACE ":([^:]*)$" "/\\1" _abs_file "${_file}")
      if(NOT EXISTS "${_abs_file}")
        message(FATAL_ERROR "rosidl_generate_interfaces() the passed file "
          "'${_abs_file}' doesn't exist")
      endif()
      list(APPEND _interface_tuples "${_file}")
    else()
      set(_abs_file "${CMAKE_CURRENT_SOURCE_DIR}/${_file}")
      if(NOT EXISTS "${_abs_file}")
        message(FATAL_ERROR "rosidl_generate_interfaces() the passed file "
            "'${_file}' doesn't exist relative to the "
            "CMAKE_CURRENT_SOURCE_DIR '${CMAKE_CURRENT_SOURCE_DIR}'")
      endif()
      list(APPEND _interface_tuples "${CMAKE_CURRENT_SOURCE_DIR}:${_file}")
    endif()
  endforeach()

  # stamp all interface files
  foreach(_tuple ${_interface_tuples})
    string(REGEX REPLACE ":([^:]*)$" "/\\1" _abs_interface "${_tuple}")
    stamp("${_abs_interface}")
  endforeach()

  # separate idl files from non-idl files
  set(_idl_tuples "")
  set(_non_idl_tuples "")
  foreach(_tuple ${_interface_tuples})
    get_filename_component(_extension "${_tuple}" EXT)
    if("${_extension}" STREQUAL ".idl")
      list(APPEND _idl_tuples "${_tuple}")
    else()
      list(APPEND _non_idl_tuples "${_tuple}")
    endif()
  endforeach()

  # adapt all non-idl files
  if(NOT "${_non_idl_tuples}" STREQUAL "")
    if(rosidl_adapter_FOUND)
      # wrap the non-idl tuples in a file
      # since they might exceed the maximum command line length
      set(_adapter_arguments_file "${CMAKE_CURRENT_BINARY_DIR}/rosidl_adapter__arguments__${target}.json")
      rosidl_write_generator_arguments(
        "${_adapter_arguments_file}"
        PACKAGE_NAME "${PROJECT_NAME}"
        NON_IDL_TUPLES "${_non_idl_tuples}"
      )
      rosidl_adapt_interfaces(
        _idl_adapter_tuples
        "${_adapter_arguments_file}"
        TARGET ${target}
      )
    endif()
  endif()
  # afterwards all remaining interface files are .idl files
  list(APPEND _idl_tuples ${_idl_adapter_tuples})

  # to generate action interfaces, we need to depend on "action_msgs"
  foreach(_tuple ${_interface_tuples})
    string(REGEX REPLACE ".*:([^:]*)$" "\\1" _tuple_file "${_tuple}")
    get_filename_component(_parent_dir "${_tuple_file}" DIRECTORY)
    if("${_parent_dir}" STREQUAL "action")
      find_package(action_msgs QUIET)
      if(NOT ${action_msgs_FOUND})
        message(FATAL_ERROR
          "Unable to generate action interface for '${_tuple_file}'. "
          "In order to generate action interfaces you must add a depend tag "
          "for 'action_msgs' in your package.xml.")
      endif()
      list_append_unique(_ARG_DEPENDENCIES "action_msgs")
      ament_export_dependencies(action_msgs)
      break()
    endif()
  endforeach()

  # collect all interface files from dependencies
  set(_dep_files)
  foreach(_dep ${_ARG_DEPENDENCIES})
    if(NOT ${_dep}_FOUND)
      message(FATAL_ERROR "rosidl_generate_interfaces() the passed dependency "
        "'${_dep}' has not been found before using find_package()")
    endif()
    foreach(_idl_file ${${_dep}_IDL_FILES})
      set(_abs_idl_file "${${_dep}_DIR}/../${_idl_file}")
      normalize_path(_abs_idl_file "${_abs_idl_file}")
      list(APPEND _dep_files "${_abs_idl_file}")
    endforeach()
  endforeach()

  set(_non_idl_files "")
  foreach(_tuple ${_non_idl_tuples})
    string(REGEX REPLACE ":([^:]*)$" "/\\1" _non_idl_file "${_tuple}")
    list(APPEND _non_idl_files "${_non_idl_file}")

    # Split .srv into two .msg files
    get_filename_component(_extension "${_tuple}" EXT)
    # generate request and response messages for services
    if(_extension STREQUAL ".srv")
      string(REGEX REPLACE ":([^:]*)$" ";\\1" _list "${_tuple}")
      list(GET _list 1 _relpath)
      get_filename_component(_name "${_relpath}" NAME_WE)
      get_filename_component(_parent_folder "${_relpath}" DIRECTORY)
      file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/rosidl_cmake/${_parent_folder}")
      set(_request_file "${CMAKE_CURRENT_BINARY_DIR}/rosidl_cmake/${_parent_folder}/${_name}_Request.msg")
      set(_response_file "${CMAKE_CURRENT_BINARY_DIR}/rosidl_cmake/${_parent_folder}/${_name}_Response.msg")
      file(READ "${_non_idl_file}" _service_content)
      string(REGEX REPLACE "^((.*\r?\n)|)---(\r?\n.*)?$" "\\1" _request_content "${_service_content}")
      string(REGEX REPLACE "^((.*\r?\n)|)---(\r?\n(.*)|())$" "\\3" _response_content "${_service_content}")
      # only re-write the request/response messages if the content has changed
      # to avoid the last modified timestamp to be updated
      if(NOT EXISTS "${_request_file}")
        file(WRITE "${_request_file}" "${_request_content}")
      else()
        file(READ "${_request_file}" _existing_request_content)
        if(NOT "${_request_content}" STREQUAL "${_existing_request_content}")
          file(WRITE "${_request_file}" "${_request_content}")
        endif()
      endif()
      if(NOT EXISTS "${_response_file}")
        file(WRITE "${_response_file}" "${_response_content}")
      else()
        file(READ "${_response_file}" _existing_response_content)
        if(NOT "${_response_content}" STREQUAL "${_existing_response_content}")
          file(WRITE "${_response_file}" "${_response_content}")
        endif()
      endif()
      list(APPEND _non_idl_files "${_request_file}" "${_response_file}")
    endif()
  endforeach()

  add_custom_target(
    ${target} ALL
    DEPENDS
    ${_non_idl_files}
    ${_dep_files}
    SOURCES
    ${_non_idl_files}
  )

  if(NOT _ARG_SKIP_INSTALL)
    if(NOT _ARG_SKIP_GROUP_MEMBERSHIP_CHECK)
      set(_group_name "rosidl_interface_packages")
      if(NOT _AMENT_PACKAGE_NAME)
        ament_package_xml()
      endif()
      if(NOT _group_name IN_LIST ${_AMENT_PACKAGE_NAME}_MEMBER_OF_GROUPS)
        message(FATAL_ERROR
          "Packages installing interfaces must include \
          '<member_of_group>${_group_name}</member_of_group>' \
          in their package.xml"
        )
      endif()
    endif()
    # register interfaces with the ament index
    set(_idl_files_lines)
    foreach(_idl_tuple ${_idl_tuples})
      string(REGEX REPLACE ":([^:]*)$" ";\\1" _idl_list "${_idl_tuple}")
      list(GET _idl_list 1 _idl_relpath)
      file(TO_CMAKE_PATH "${_idl_relpath}" _idl_relpath)
      list(APPEND _idl_files_lines "${_idl_relpath}")
      endforeach()
    foreach(_idl_file ${_non_idl_files})
      get_filename_component(_interface_ns "${_idl_file}" DIRECTORY)
      get_filename_component(_interface_ns "${_interface_ns}" NAME)
      get_filename_component(_interface_name "${_idl_file}" NAME)
      list(APPEND _idl_files_lines "${_interface_ns}/${_interface_name}")
    endforeach()
    list(SORT _idl_files_lines)
    string(REPLACE ";" "\n" _idl_files_lines "${_idl_files_lines}")
    ament_index_register_resource("rosidl_interfaces" CONTENT "${_idl_files_lines}")
  endif()

  # collect package names of recursive dependencies which contain interface files
  set(_recursive_dependencies)
  foreach(_dep ${_ARG_DEPENDENCIES})
    if(DEFINED ${_dep}_IDL_FILES)
      list_append_unique(_recursive_dependencies "${_dep}")
    endif()
    foreach(_dep2 ${${_dep}_RECURSIVE_DEPENDENCIES})
      if(DEFINED ${_dep2}_IDL_FILES)
        list_append_unique(_recursive_dependencies "${_dep2}")
      endif()
    endforeach()
  endforeach()

  # generators must be executed in topological order
  # which is ensured by every generator finding its dependencies first
  # and then registering itself as an extension
  set(rosidl_generate_interfaces_TARGET ${target})
  set(rosidl_generate_interfaces_DEPENDENCY_PACKAGE_NAMES ${_recursive_dependencies})
  set(rosidl_generate_interfaces_LIBRARY_NAME ${_ARG_LIBRARY_NAME})
  set(rosidl_generate_interfaces_SKIP_INSTALL ${_ARG_SKIP_INSTALL})
  set(rosidl_generate_interfaces_ADD_LINTER_TESTS ${_ARG_ADD_LINTER_TESTS})

  set(rosidl_generate_interfaces_IDL_TUPLES ${_idl_tuples})
  unset(rosidl_generate_interfaces_IDL_FILES)

  set(rosidl_generate_interfaces_ABS_IDL_FILES)
  foreach(_idl_tuple ${rosidl_generate_interfaces_IDL_TUPLES})
    string(REGEX REPLACE ":([^:]*)$" "/\\1" _abs_idl_file "${_idl_tuple}")
    list(APPEND rosidl_generate_interfaces_ABS_IDL_FILES "${_abs_idl_file}")
  endforeach()

  ament_execute_extensions("rosidl_generate_idl_interfaces")

  # check for extensions registered with the previous extension point
  set(obsolete_extension_point "rosidl_generate_interfaces")
  if(AMENT_EXTENSIONS_${obsolete_extension_point})
    foreach(_extension ${AMENT_EXTENSIONS_${obsolete_extension_point}})
      string(REPLACE ":" ";" _extension_list "${_extension}")
      list(GET _extension_list 0 _pkg_name)
      message(WARNING "Package '${_pkg_name}' registered an extension for the "
        "obsolete extension point '${obsolete_extension_point}'. "
        "It is being skipped and needs to be updated to the new extension "
        "point 'rosidl_generate_idl_interfaces'."
        "Please refer to the migration steps on the Dashing release page for "
        "more details.")
    endforeach()
  endif()

  if(NOT _ARG_SKIP_INSTALL)
    foreach(_idl_tuple ${_idl_tuples})
      string(REGEX REPLACE ":([^:]*)$" ";\\1" _idl_list "${_idl_tuple}")
      list(GET _idl_list 1 _idl_relpath)
      string(REGEX REPLACE ":([^:]*)$" "/\\1" _idl_file "${_idl_tuple}")
      get_filename_component(_parent_folders "${_idl_relpath}" DIRECTORY)
      install(
        FILES ${_idl_file}
        DESTINATION "share/${PROJECT_NAME}/${_parent_folders}"
      )
      file(TO_CMAKE_PATH "${_idl_relpath}" _idl_relpath)
      list(APPEND _rosidl_cmake_IDL_FILES "${_idl_relpath}")
    endforeach()
    # install interface files to subfolders based on their extension
    foreach(_idl_file ${_non_idl_files})
      get_filename_component(_parent_folder "${_idl_file}" DIRECTORY)
      get_filename_component(_parent_folder "${_parent_folder}" NAME)
      install(
        FILES ${_idl_file}
        DESTINATION "share/${PROJECT_NAME}/${_parent_folder}"
      )
      get_filename_component(_name "${_idl_file}" NAME)
      list(APPEND _rosidl_cmake_INTERFACE_FILES "${_parent_folder}/${_name}")
    endforeach()
  endif()
endmacro()
