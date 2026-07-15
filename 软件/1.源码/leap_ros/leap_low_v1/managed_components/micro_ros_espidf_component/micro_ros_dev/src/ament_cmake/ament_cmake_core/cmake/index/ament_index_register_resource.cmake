# Copyright 2014 Open Source Robotics Foundation, Inc.
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
# Register a package resource of a specific type with the index.
#
# For both CONTENT as well as CONTENT_FILE CMake generator expressions are
# supported.
#
# :param resource_type: the type of the resource
# :type resource_type: string
# :param CONTENT: the content of the marker file being installed
#   as a result of the registration (default: empty string)
# :type CONTENT: string
# :param CONTENT_FILE: the path to a file which will be used to fill
#   the marker file being installed as a result of the registration.
#   The file can either be a plain file or a template (ending with
#   '.in') which is expanded using configure_file() with @ONLY.
#   (optional, conflicts with CONTENT)
# :type CONTENT_FILE: string
# :param PACKAGE_NAME: the package name (default: ${PROJECT_NAME})
# :type PACKAGE_NAME: string
# :param AMENT_INDEX_BINARY_DIR: the base path of the generated ament
#   index (default: ${CMAKE_BINARY_DIR}/ament_cmake_index)
# :type AMENT_INDEX_BINARY_DIR: string
# :param SKIP_INSTALL: if set skip installing the marker file
# :type SKIP_INSTALL: option
#
# @public
#
function(ament_index_register_resource resource_type)
  if(resource_type STREQUAL "")
    message(FATAL_ERROR
      "ament_index_register_resource() called without a 'resource_type'")
  endif()

  cmake_parse_arguments(
    ARG
    "SKIP_INSTALL"
    "PACKAGE_NAME;CONTENT_FILE;AMENT_INDEX_BINARY_DIR"
    "CONTENT"
    ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_index_register_resource() called with unused "
      "arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  if(ARG_CONTENT AND ARG_CONTENT_FILE)
    message(FATAL_ERROR "ament_index_register_resource() called with both "
      "'CONTENT' and 'CONTENT_FILE', only one is allowed")
  endif()

  if(NOT ARG_PACKAGE_NAME)
    set(ARG_PACKAGE_NAME "${PROJECT_NAME}")
  endif()

  if(ARG_CONTENT_FILE)
    if(NOT IS_ABSOLUTE "${ARG_CONTENT_FILE}")
      set(ARG_CONTENT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${ARG_CONTENT_FILE}")
    endif()
    if(NOT EXISTS "${ARG_CONTENT_FILE}")
      message(FATAL_ERROR "ament_index_register_resource() the content file "
        "'${ARG_CONTENT_FILE}' does not exist")
    endif()

    string_ends_with("${ARG_CONTENT_FILE}" ".in" is_template)
    if(NOT is_template)
      # read non-template file content
      file(READ "${ARG_CONTENT_FILE}" ARG_CONTENT)
    endif()
  endif()

  if(NOT ARG_AMENT_INDEX_BINARY_DIR)
    set(ARG_AMENT_INDEX_BINARY_DIR "${CMAKE_BINARY_DIR}/ament_cmake_index")
  endif()

  set(destination "share/ament_index/resource_index/${resource_type}")
  set(marker_file
    "${ARG_AMENT_INDEX_BINARY_DIR}/${destination}/${ARG_PACKAGE_NAME}")

  if(ARG_CONTENT OR NOT ARG_CONTENT_FILE)
    file(GENERATE OUTPUT "${marker_file}" CONTENT "${ARG_CONTENT}")
  else()
    # content file is a  .in template
    configure_file(
      "${ARG_CONTENT_FILE}"
      "${marker_file}.genexp"
      @ONLY
    )
    file(GENERATE OUTPUT "${marker_file}" INPUT "${marker_file}.genexp")
  endif()

  if(NOT ARG_SKIP_INSTALL)
    install(
      FILES "${marker_file}"
      DESTINATION "${destination}"
    )
  endif()
endfunction()
