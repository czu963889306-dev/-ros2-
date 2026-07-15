# Copyright 2018 Open Source Robotics Foundation, Inc.
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
# Get the package name of the default logging implementation.
#
# Either selecting it using the variable RCL_LOGGING_IMPLEMENTATION or
# choosing a default from the available implementations.
#
# :param var: the output variable name containing the package name
# :type var: string
#
macro(get_default_rcl_logging_implementation var)

  # if logging implementation already specified or RCL_LOGGING_IMPLEMENTATION environment variable
  # is set then use that, otherwise default to using rcl_logging_noop
  if(NOT "${RCL_LOGGING_IMPLEMENTATION}" STREQUAL "")
    set(_logging_implementation "${RCL_LOGGING_IMPLEMENTATION}")
  elseif(NOT "$ENV{RCL_LOGGING_IMPLEMENTATION}" STREQUAL "")
    set(_logging_implementation "$ENV{RCL_LOGGING_IMPLEMENTATION}")
  else()
    set(_logging_implementation rcl_logging_spdlog)
  endif()

  # persist implementation decision in cache
  # if it was not determined dynamically
  set(
    RCL_LOGGING_IMPLEMENTATION "${_logging_implementation}"
    CACHE STRING "select rcl logging implementation to use" FORCE
  )

  find_package("${_logging_implementation}" REQUIRED)

  set(${var} ${_logging_implementation})
endmacro()
