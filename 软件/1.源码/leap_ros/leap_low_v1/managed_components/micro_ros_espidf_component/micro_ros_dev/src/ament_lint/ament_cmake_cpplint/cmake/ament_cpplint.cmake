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
# Add a test to check the code for compliance with cpplint.
#
# :param TESTNAME: the name of the test, default: "cpplint"
# :type TESTNAME: string
# :param FILTERS: list of category filters to apply
# :type FILTERS: list of strings
# :param MAX_LINE_LENGTH: override the maximum line length,
#   the default is defined in ament_cpplint
# :type MAX_LINE_LENGTH: integer
# :param ROOT: override the --root option of cpplint
# :type ROOT: string
# :param TIMEOUT: the test timeout in seconds, default: 120
# :type TIMEOUT: integer
# :param EXCLUDE: an optional list of exclude directories or files for cpplint
# :type EXCLUDE: list
# :param ARGN: the files or directories to check
# :type ARGN: list of strings
#
# @public
#
function(ament_cpplint)
  cmake_parse_arguments(ARG "" "EXCLUDE;MAX_LINE_LENGTH;ROOT;TESTNAME;TIMEOUT" "FILTERS" ${ARGN})
  if(NOT ARG_TESTNAME)
    set(ARG_TESTNAME "cpplint")
  endif()

  find_program(ament_cpplint_BIN NAMES "ament_cpplint")
  if(NOT ament_cpplint_BIN)
    message(FATAL_ERROR "ament_cpplint() could not find program 'ament_cpplint'")
  endif()

  set(result_file "${AMENT_TEST_RESULTS_DIR}/${PROJECT_NAME}/${ARG_TESTNAME}.xunit.xml")
  set(cmd "${ament_cpplint_BIN}" "--xunit-file" "${result_file}")
  if(ARG_EXCLUDE)
    list(APPEND cmd "--exclude" "${ARG_EXCLUDE}")
  endif()
  if(ARG_FILTERS)
    string(REPLACE ";" "," filters "${ARG_FILTERS}")
    list(APPEND cmd "--filters=${filters}")
  endif()
  if(ARG_MAX_LINE_LENGTH)
    list(APPEND cmd "--linelength" "${ARG_MAX_LINE_LENGTH}")
  endif()
  if(ARG_ROOT)
    list(APPEND cmd "--root" "${ARG_ROOT}")
  endif()
  list(APPEND cmd ${ARG_UNPARSED_ARGUMENTS})
  if(NOT ARG_TIMEOUT)
    set(ARG_TIMEOUT 120)
  endif()

  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/ament_cpplint")
  ament_add_test(
    "${ARG_TESTNAME}"
    COMMAND ${cmd}
    OUTPUT_FILE "${CMAKE_BINARY_DIR}/ament_cpplint/${ARG_TESTNAME}.txt"
    RESULT_FILE "${result_file}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    TIMEOUT "${ARG_TIMEOUT}"
  )
  set_tests_properties(
    "${ARG_TESTNAME}"
    PROPERTIES
    LABELS "cpplint;linter"
  )
endfunction()
