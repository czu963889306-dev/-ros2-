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
# Add a test to lint the CMake code.
#
# :param TESTNAME: the name of the test, default: "lint_cmake"
# :type TESTNAME: string
# :param MAX_LINE_LENGTH: override the maximum line length,
#   the default is defined in ament_lint_cmake
# :type MAX_LINE_LENGTH: integer
# :param ARGN: the files or directories to check
# :type ARGN: list of strings
#
# @public
#
function(ament_lint_cmake)
  cmake_parse_arguments(ARG "" "MAX_LINE_LENGTH;TESTNAME" "" ${ARGN})
  if(NOT ARG_TESTNAME)
    set(ARG_TESTNAME "lint_cmake")
  endif()

  find_program(ament_lint_cmake_BIN NAMES "ament_lint_cmake")
  if(NOT ament_lint_cmake_BIN)
    message(FATAL_ERROR "ament_lint_cmake() could not find program 'ament_lint_cmake'")
  endif()

  set(result_file "${AMENT_TEST_RESULTS_DIR}/${PROJECT_NAME}/${ARG_TESTNAME}.xunit.xml")
  set(cmd "${ament_lint_cmake_BIN}" "--xunit-file" "${result_file}")
  if(DEFINED ARG_MAX_LINE_LENGTH)
    list(APPEND cmd "--linelength" "${ARG_MAX_LINE_LENGTH}")
  endif()
  list(APPEND cmd ${ARG_UNPARSED_ARGUMENTS})

  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/ament_lint_cmake")
  ament_add_test(
    "${ARG_TESTNAME}"
    COMMAND ${cmd}
    OUTPUT_FILE "${CMAKE_BINARY_DIR}/ament_lint_cmake/${ARG_TESTNAME}.txt"
    RESULT_FILE "${result_file}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  set_tests_properties(
    "${ARG_TESTNAME}"
    PROPERTIES
    LABELS "lint_cmake;linter"
  )
endfunction()
