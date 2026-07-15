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
# Get the package name of the default ROS middleware implementation.
#
# Either selecting it using the variable RMW_IMPLEMENTATION or
# choosing a default from the available implementations.
#
# :param var: the output variable name containing the package name
# :type var: string
#
macro(get_default_rmw_implementation var)
  get_available_rmw_implementations(_middleware_implementations)

  if(_middleware_implementations STREQUAL "")
    message(FATAL_ERROR "Could not find any ROS middleware implementation.")
  endif()

  # option()
  if("${RMW_IMPLEMENTATION}" STREQUAL "" AND
    "$ENV{RMW_IMPLEMENTATION}" STREQUAL ""
  )
    # prefer FastDDS, otherwise first in alphabetical order
    list(FIND _middleware_implementations "rmw_fastrtps_cpp" _index)
    if(NOT _index EQUAL -1)
      list(GET _middleware_implementations ${_index} _middleware_implementation)
    else()
      list(GET _middleware_implementations 0 _middleware_implementation)
    endif()
  else()
    if(NOT "${RMW_IMPLEMENTATION}" STREQUAL "")
      set(_middleware_implementation "${RMW_IMPLEMENTATION}")
    else()
      set(_middleware_implementation "$ENV{RMW_IMPLEMENTATION}")
    endif()
    # persist implementation decision in cache
    # if it was not determined dynamically
    set(
      RMW_IMPLEMENTATION "${_middleware_implementation}"
      CACHE STRING "Select ROS middleware implementation to link against" FORCE
    )
  endif()

  # verify that the selection one is available
  list(FIND _middleware_implementations "${_middleware_implementation}" _index)
  if(_index EQUAL -1)
    string(REPLACE ";" ", " _middleware_implementations_string "${_middleware_implementations}")
    message(FATAL_ERROR
      "Could not find ROS middleware implementation '${_middleware_implementation}'. "
      "Choose one of the following: ${_middleware_implementations_string}")
  endif()
  find_package("${_middleware_implementation}" REQUIRED)

  set(${var} ${_middleware_implementation})
endmacro()
