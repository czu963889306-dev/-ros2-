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
# Add a test to check the code for compliance with copyright.
#
# :param TESTNAME: the name of the test, default: "copyright"
# :type TESTNAME: string
# :param TIMEOUT: the test timeout in seconds, default: 120
# :type TIMEOUT: integer
# :param EXCLUDE: an optional list of exclude files or directories for copyright check
# :type EXCLUDE: list
# :param ARGN: the files or directories to check
# :type ARGN: list of strings
#
# @public
#
function(ament_copyright)
  cmake_parse_arguments(ARG "" "TESTNAME;TIMEOUT" "EXCLUDE" ${ARGN})
  if(NOT ARG_TESTNAME)
    set(ARG_TESTNAME "copyright")
  endif()
  if(NOT ARG_TIMEOUT)
    set(ARG_TIMEOUT 200)
  endif()

  find_program(ament_copyright_BIN NAMES "ament_copyright")
  if(NOT ament_copyright_BIN)
    message(FATAL_ERROR "ament_copyright() could not find program 'ament_copyright'")
  endif()

  set(result_file "${AMENT_TEST_RESULTS_DIR}/${PROJECT_NAME}/${ARG_TESTNAME}.xunit.xml")
  set(cmd "${ament_copyright_BIN}" "--xunit-file" "${result_file}")
  if(ARG_EXCLUDE)
    list(APPEND cmd "--exclude" "${ARG_EXCLUDE}")
  endif()
  list(APPEND cmd ${ARG_UNPARSED_ARGUMENTS})

  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/ament_copyright")
  ament_add_test(
    "${ARG_TESTNAME}"
    COMMAND ${cmd}
    OUTPUT_FILE "${CMAKE_BINARY_DIR}/ament_copyright/${ARG_TESTNAME}.txt"
    RESULT_FILE "${result_file}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    TIMEOUT "${ARG_TIMEOUT}"
  )
  set_tests_properties(
    "${ARG_TESTNAME}"
    PROPERTIES
    LABELS "copyright;linter"
  )
endfunction()
