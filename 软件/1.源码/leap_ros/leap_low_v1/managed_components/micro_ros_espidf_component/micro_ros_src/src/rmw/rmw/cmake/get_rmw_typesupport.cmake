# Copyright 2015-2016 Open Source Robotics Foundation, Inc.
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
# Get the type support package names for a specific RMW implementation.
#
# :param var: the output variable name containing the package names
# :type var: list of strings
# :param rmw_implementation: the package name of the RMW implementation
# :type rmw_implementation: string
# :param LANGUAGE: Optional flag for the language of the type support to get.
#   If language is omitted, type supports for all languages are returned.
# :type LANGUAGE: string
#
# @public
#
function(get_rmw_typesupport var rmw_implementation)
  cmake_parse_arguments(ARG "" "LANGUAGE" "" ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "get_rmw_typesupport() called with unused "
      "arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()
  set(resource_type "rmw_typesupport")
  if(ARG_LANGUAGE)
    string(TOLOWER "${ARG_LANGUAGE}" ARG_LANGUAGE)
    set(resource_type "${resource_type}_${ARG_LANGUAGE}")
  endif()
  ament_index_has_resource(has_resource "${resource_type}" "${rmw_implementation}")
  if(has_resource)
    ament_index_get_resource(resource "${resource_type}" "${rmw_implementation}")
  else()
    set(resource "")
  endif()
  set(${var} "${resource}" PARENT_SCOPE)
endfunction()
