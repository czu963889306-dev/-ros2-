# Copyright 2014-2015 Open Source Robotics Foundation, Inc.
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
# Get all registered package resources of a specific type from the index.
#
# :param var: the output variable name
# :type var: list of resource names
# :param resource_type: the type of the resource
# :type resource_type: string
# :param PREFIX_PATH: the prefix path to search for (default
#   ``ament_index_get_prefix_path()``).
# :type PREFIX_PATH: list of strings
#
# @public
#
function(ament_index_get_resources var resource_type)
  if(resource_type STREQUAL "")
    message(FATAL_ERROR
      "ament_index_get_resources() called without a 'resource_type'")
  endif()

  cmake_parse_arguments(ARG "" "PREFIX_PATH" "" ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_index_get_resources() called with unused "
      "arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  if(ARG_PREFIX_PATH)
    set(prefix_path "${ARG_PREFIX_PATH}")
  else()
    ament_index_get_prefix_path(prefix_path)
  endif()

  # Remove any empty strings and make sure slashes are consistent
  set(paths_to_search)
  foreach(path IN LISTS prefix_path)
    if(NOT path STREQUAL "")
      string(REPLACE "\\" "/" normalized_path "${path}")
      list_append_unique(paths_to_search "${normalized_path}")
    endif()
  endforeach()
  set(all_resources "")
  foreach(path IN LISTS paths_to_search)
    set(resource_index_path "${path}/share/ament_index/resource_index")
    file(GLOB resources
      LIST_DIRECTORIES FALSE
      RELATIVE "${resource_index_path}/${resource_type}"
      "${resource_index_path}/${resource_type}/*")
    foreach(resource IN LISTS resources)
      string(SUBSTRING "${resource}" 0 1 resource_char0)
      # Ignore any files starting with a dot
      if(NOT resource_char0 STREQUAL ".")
        list_append_unique(all_resources ${resource})
      endif()
    endforeach()
  endforeach()
  set(${var} "${all_resources}" PARENT_SCOPE)
endfunction()
