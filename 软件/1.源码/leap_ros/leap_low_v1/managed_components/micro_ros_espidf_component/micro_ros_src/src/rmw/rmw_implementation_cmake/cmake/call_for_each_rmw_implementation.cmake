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
# Call a CMake macro for each available RMW implementation.
#
# The macro will be called for each RMW implementation with a different
# target_suffix.
# It can optionally be called with an empty target_suffix.
#
# The following variables will be set before including the file:
# * rmw_implementation: The package name of the RMW implementation
# * target_suffix: Either a string derived from the RMW implementation or empty
#   if there is only one RMW implementation
#
# :param macro: the name of the CMake macro to be called
# :type macro: string
# :param GENERATE_DEFAULT: if set additionally call the macro with an empty
#   target_suffix independent of how many RMW implementations are available
# :type GENERATE_DEFAULT: option
#
macro(call_for_each_rmw_implementation macro_name)
  if(macro_name STREQUAL "")
    message(FATAL_ERROR
      "call_for_each_rmw_implementation() called without a 'macro_name'")
  endif()
  cmake_parse_arguments(_ARG "GENERATE_DEFAULT" "" "" ${ARGN})
  if(_ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "call_for_each_rmw_implementation() called with unused arguments: "
      "${ARGN}")
  endif()

  set(_filename "${CMAKE_BINARY_DIR}/call_for_each_rmw_implementation.cmake")
  file(WRITE "${_filename}" "${macro_name}()\n")

  # get available rmw implementations
  get_available_rmw_implementations(_rmw_implementations)
  list(LENGTH _rmw_implementations _length)

  if(_ARG_GENERATE_DEFAULT)
    # call macro once without a target suffix and the default rmw impl.
    get_default_rmw_implementation(rmw_implementation)
    find_package("${rmw_implementation}" REQUIRED)
    set(target_suffix)
    include("${_filename}")
  endif()
  # call macro for each RMW implementation with a target suffix
  foreach(rmw_implementation ${_rmw_implementations})
    find_package("${rmw_implementation}" QUIET)
    if(${rmw_implementation}_FOUND)
      set(target_suffix "__${rmw_implementation}")
      include("${_filename}")
    endif()
  endforeach()
endmacro()
