# Copyright 2019 Canonical, Ltd.
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
# Add a test to statically check Python types using mypy.
#
# :param CONFIG_FILE: the name of the config file to use, if any
# :type CONFIG_FILE: string
# :param TESTNAME: the name of the test, default: "mypy"
# :type TESTNAME: string
# :param ARGN: the files or directories to check
# :type ARGN: list of strings
#
# @public
#
function(ament_mypy)
  cmake_parse_arguments(ARG "" "CONFIG_FILE;TESTNAME" "" ${ARGN})
  if(NOT ARG_TESTNAME)
    set(ARG_TESTNAME "mypy")
  endif()

  find_program(ament_mypy_BIN NAMES "ament_mypy")
  if(NOT ament_mypy_BIN)
    message(FATAL_ERROR "ament_mypy() could not find program 'ament_mypy'")
  endif()

  set(result_file "${AMENT_TEST_RESULTS_DIR}/${PROJECT_NAME}/${ARG_TESTNAME}.xunit.xml")
  set(cmd "${ament_mypy_BIN}" "--xunit-file" "${result_file}")
  if(ARG_CONFIG_FILE)
    list(APPEND cmd "--config" "${ARG_CONFIG_FILE}")
  endif()
  list(APPEND cmd ${ARG_UNPARSED_ARGUMENTS})

  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/ament_mypy")
  ament_add_test(
    "${ARG_TESTNAME}"
    COMMAND ${cmd}
    OUTPUT_FILE "${CMAKE_BINARY_DIR}/ament_mypy/${ARG_TESTNAME}.txt"
    RESULT_FILE "${result_file}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  set_tests_properties(
    "${ARG_TESTNAME}"
    PROPERTIES
    LABELS "mypy;linter"
  )
endfunction()
