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
# Convert non-idl interface files into `.idl` files.
#
# The input file might be a `.msg` or `.srv` file.
#
# :param idl_var: the variable name to return the list of generated `.idl`
#   files, each item is a tuple separated by a colon with an absolute base path
#   and a path relative to that base path
# :type idl_var: string
# :param arguments_file: the path of the arguments file containing the paths of
#   the non-idl files.
# :type arguments_file: string
# :param TARGET: the name of the generation target
# :type TARGET: string
#
# @public
#
function(rosidl_adapt_interfaces idl_var arguments_file)
  cmake_parse_arguments(ARG "" "TARGET" ""
    ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "rosidl_adapt_interfaces() called with unused "
      "arguments: ${ARG_UNPARSED_ARGUMENTS}")
  endif()

  find_package(ament_cmake_core REQUIRED)  # for get_executable_path
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
  get_executable_path(python_interpreter Python3::Interpreter CONFIGURE)

  set(idl_output "${CMAKE_CURRENT_BINARY_DIR}/rosidl_adapter/${ARG_TARGET}.idls")
  set(cmd
    "${python_interpreter}" -m rosidl_adapter
    --package-name ${PROJECT_NAME}
    --arguments-file "${arguments_file}"
    --output-dir "${CMAKE_CURRENT_BINARY_DIR}/rosidl_adapter/${PROJECT_NAME}"
    --output-file "${idl_output}")
  execute_process(
    COMMAND ${cmd}
    OUTPUT_QUIET
    ERROR_VARIABLE error
    RESULT_VARIABLE result
  )
  if(NOT result EQUAL 0)
    string(REPLACE ";" " " cmd_str "${cmd}")
    message(FATAL_ERROR
      "execute_process(${cmd_str}) returned error code ${result}:\n${error}")
  endif()

  file(STRINGS "${idl_output}" idl_tuples)

  set(${idl_var} ${idl_tuples} PARENT_SCOPE)
endfunction()
