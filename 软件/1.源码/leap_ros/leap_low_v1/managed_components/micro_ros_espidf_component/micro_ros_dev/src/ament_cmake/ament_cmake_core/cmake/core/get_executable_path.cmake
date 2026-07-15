# Copyright 2021 Open Source Robotics Foundation, Inc.
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
# Get the path to an executable at build or configure time.
#
# The argument target_or_path may either be a path to an executable (such as
# PYTHON_EXECUTABLE), or an executable target (such as Python3::Interpreter).
# If the argument is an executable target then its location will be returned.
# otherwise the original argument will be returned unmodified.
#
# Use CONFIGURE when an executable is to be run at configure time, such as when
# using execute_process().
# The returned value will be the path to the process.
# Use BUILD when an executable is to be run at build or test time, such as
# when using add_custom_command() or add_test().
# The returned value will be either a path or a generator expression that
# evaluates to the path of an executable target.
#
# :param var: the output variable name
# :type var: string
# :param target_or_path: imported executable target or a path to an executable
# :param target_or_path: string
#
# @public
#
function(get_executable_path var target_or_path)
  cmake_parse_arguments(ARG "BUILD;CONFIGURE" "" "" ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "get_executable() called with unused arguments: "
      "${ARG_UNPARSED_ARGUMENTS}")
  endif()
  if(NOT ARG_BUILD AND NOT ARG_CONFIGURE)
    message(FATAL_ERROR "get_executable_path() must have BUILD or CONFIGURE set"
      "${ARG_UNPARSED_ARGUMENTS}")
  endif()

  # If it isn't a target, return whatever was given unmodified
  set(output_var "${target_or_path}")

  if(TARGET ${target_or_path})
    # There is a target with this name
    get_target_property(type "${target_or_path}" TYPE)
    get_target_property(imported "${target_or_path}" IMPORTED)
    if("${type}" STREQUAL "EXECUTABLE")
      # The target is an executable, grab its LOCATION property
      if(ARG_BUILD)
        if(imported)
          # Return a generator expression to get the LOCATION property
          set(output_var "$<TARGET_PROPERTY:${target_or_path},LOCATION>")
        else()
          # Return a generator expression to get the output location
          set(output_var "$<TARGET_FILE:${target_or_path}>")
        endif()
      else()
        if(imported)
          # Return the content of the property directly
          get_target_property(output_var "${target_or_path}" LOCATION)
        else()
          message(WARNING
            "There exists a non-imported executable target named"
            " '${target_or_path}', but those cannot be used at configure time."
            " Assuming it's a path instead.")
        endif()
      endif()
    endif()
  endif()

  set("${var}" "${output_var}" PARENT_SCOPE)
endfunction()
