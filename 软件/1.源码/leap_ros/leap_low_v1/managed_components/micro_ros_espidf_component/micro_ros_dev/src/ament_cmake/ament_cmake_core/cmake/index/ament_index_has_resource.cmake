# Copyright 2015 Open Source Robotics Foundation, Inc.
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
# Check if the index contains a specific resource.
#
# :param var: the prefix path if the resource exists, FALSE otherwise
# :type var: string or FALSE
# :param resource_type: the type of the resource
# :type resource_type: string
# :param resource_name: the name of the resource
# :type resource_name: string
# :param PREFIX_PATH: the prefix path to search for (default
#   ``ament_index_get_prefix_path()``).
# :type PREFIX_PATH: list of strings
#
# @public
#
function(ament_index_has_resource var resource_type resource_name)
  if(resource_type STREQUAL "")
    message(FATAL_ERROR
      "ament_index_has_resource() called without a 'resource_type'")
  endif()
  if(resource_name STREQUAL "")
    message(FATAL_ERROR
      "ament_index_has_resource() called without a 'resource_name'")
  endif()

  cmake_parse_arguments(ARG "" "PREFIX_PATH" "" ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_index_has_resource() called with unused "
      "arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  if(ARG_PREFIX_PATH)
    set(prefix_path "${ARG_PREFIX_PATH}")
  else()
    ament_index_get_prefix_path(prefix_path)
  endif()

  set(retval FALSE)
  foreach(path IN LISTS prefix_path)
    if(EXISTS
        "${path}/share/ament_index/resource_index/${resource_type}/${resource_name}")
      set(retval "${path}")
      break()
    endif()
  endforeach()
  set(${var} "${retval}" PARENT_SCOPE)
endfunction()
