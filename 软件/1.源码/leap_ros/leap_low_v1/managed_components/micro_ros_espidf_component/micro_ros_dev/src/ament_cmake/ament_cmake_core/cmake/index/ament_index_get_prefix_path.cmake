# Copyright 2016 Open Source Robotics Foundation, Inc.
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
# Get the prefix path including the folder from the binary dir.
#
# :param var: the output variable name for the prefix path
# :type var: string
# :param SKIP_AMENT_PREFIX_PATH: if set skip adding the paths from the
#   environment variable ``AMENT_PREFIX_PATH``
# :type SKIP_AMENT_PREFIX_PATH: option
# :param SKIP_BINARY_DIR: if set skip adding the folder within the binary dir
# :type SKIP_BINARY_DIR: option
#
# @public
#
function(ament_index_get_prefix_path var)
  cmake_parse_arguments(ARG
    "SKIP_AMENT_PREFIX_PATH;SKIP_BINARY_DIR" "" "" ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_index_get_prefix_path() called with unused "
      "arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  if(NOT ARG_SKIP_AMENT_PREFIX_PATH)
    set(prefix_path $ENV{AMENT_PREFIX_PATH})
    if(NOT WIN32)
      # Convert it to a list if not on Windows.
      # On Windows, it is already separated by ; and is a list in CMake.
      string(REPLACE ":" ";" prefix_path "${prefix_path}")
    endif()
  else()
    set(prefix_path "")
  endif()

  # Remove CMAKE_INSTALL_PREFIX if it is in the list of paths to search,
  # and add it to the list at the front
  list(REMOVE_ITEM prefix_path "${CMAKE_INSTALL_PREFIX}")
  list(INSERT prefix_path 0 "${CMAKE_INSTALL_PREFIX}")

  if(NOT ARG_SKIP_BINARY_DIR)
    # prepend path from binary dir
    list(INSERT prefix_path 0 "${CMAKE_BINARY_DIR}/ament_cmake_index")
  endif()

  set(${var} "${prefix_path}" PARENT_SCOPE)
endfunction()
