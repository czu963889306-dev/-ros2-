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
# Add a test to check the Python code for syntax and style compliance
# using flake8.
#
# The default configuration file for flake8 is located at
# configuration/ament_flake8.ini within the ament_flake8 directory
# The default configuration file can be either overridden by the
# argument 'CONFIG_FILE' or by a global variable named
# 'ament_cmake_flake8_CONFIG_FILE'
# The 'CONFIG_FILE' argument takes priority over
# 'ament_cmake_flake8_CONFIG_FILE' if both are defined
#
# :param TESTNAME: the name of the test, default: "flake8"
# :type TESTNAME: string
# :param CONFIG_FILE: the path of the configuration file for flake8 to consider
# :type CONFIG_FILE: string
# :param MAX_LINE_LENGTH: override the maximum line length,
#   the default is defined in ament_flake8
# :type MAX_LINE_LENGTH: integer
# :param ARGN: the files or directories to check
# :type ARGN: list of strings
#
# @public
#
function(ament_flake8)
  cmake_parse_arguments(ARG "" "MAX_LINE_LENGTH;TESTNAME;CONFIG_FILE" "" ${ARGN})
  if(NOT ARG_TESTNAME)
    set(ARG_TESTNAME "flake8")
  endif()

  find_program(ament_flake8_BIN NAMES "ament_flake8")
  if(NOT ament_flake8_BIN)
    message(FATAL_ERROR "ament_flake8() could not find program 'ament_flake8'")
  endif()

  set(result_file "${AMENT_TEST_RESULTS_DIR}/${PROJECT_NAME}/${ARG_TESTNAME}.xunit.xml")
  set(cmd "${ament_flake8_BIN}" "--xunit-file" "${result_file}")
  if(ARG_CONFIG_FILE)
    list(APPEND cmd "--config" "${ARG_CONFIG_FILE}")
  elseif(DEFINED ament_cmake_flake8_CONFIG_FILE)
    list(APPEND cmd "--config" "${ament_cmake_flake8_CONFIG_FILE}")
  endif()
  if(ARG_MAX_LINE_LENGTH)
    list(APPEND cmd "--linelength" "${ARG_MAX_LINE_LENGTH}")
  endif()
  list(APPEND cmd ${ARG_UNPARSED_ARGUMENTS})

  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/ament_flake8")
  ament_add_test(
    "${ARG_TESTNAME}"
    COMMAND ${cmd}
    OUTPUT_FILE "${CMAKE_BINARY_DIR}/ament_flake8/${ARG_TESTNAME}.txt"
    RESULT_FILE "${result_file}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  set_tests_properties(
    "${ARG_TESTNAME}"
    PROPERTIES
    LABELS "flake8;linter"
  )
endfunction()
