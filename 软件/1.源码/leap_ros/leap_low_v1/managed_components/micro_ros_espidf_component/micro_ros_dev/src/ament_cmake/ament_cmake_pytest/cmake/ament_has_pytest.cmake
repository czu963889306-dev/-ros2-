# Copyright 2017 Open Source Robotics Foundation, Inc.
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
# Check if the Python module `pytest` was found.
#
# :param var: the output variable name
# :type var: string
# :param QUIET: suppress the CMake warning if pytest is not found, if not set
#   and pytest was not found a CMake warning is printed
# :type QUIET: option
# :param PYTHON_EXECUTABLE: Python executable used to check for pytest
#   It defaults to the CMake executable target Python3::Interpreter.
# :type PYTHON_EXECUTABLE: string
#
# @public
#
function(ament_has_pytest var)
  cmake_parse_arguments(ARG "QUIET" "PYTHON_EXECUTABLE" "" ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_has_pytest() called with unused arguments: "
      "${ARG_UNPARSED_ARGUMENTS}")
  endif()

  if(NOT ARG_PYTHON_EXECUTABLE)
    set(ARG_PYTHON_EXECUTABLE Python3::Interpreter)
  endif()

  get_executable_path(python_interpreter "${ARG_PYTHON_EXECUTABLE}" CONFIGURE)

  set(cmd "${python_interpreter}" "-m" "pytest" "--version")
  execute_process(
    COMMAND ${cmd}
    RESULT_VARIABLE res
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error)
  if(res EQUAL 0)
    set(${var} TRUE PARENT_SCOPE)
  else()
    if(NOT ARG_QUIET)
      string(REPLACE ";" " " cmd_str "${cmd}")
      message(WARNING
        "Failed to find Python module `pytest`: "
        "'${cmd_str}' returned error code ${res}")
    endif()
    set(${var} FALSE PARENT_SCOPE)
  endif()
endfunction()
