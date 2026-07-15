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
# Add a test to check the code for compliance with uncrustify.
#
# The default configuration file used for uncrustify is located at
# configuration/ament_code_style.cfg within the ament_uncrustify directory.
# The default configuration file can be overridden by the
# argument 'CONFIG_FILE'.
#
# :param TESTNAME: the name of the test, default: "uncrustify"
# :type TESTNAME: string
# :param CONFIG_FILE: the path of the configuration file for
#   uncrustify to consider
# :type CONFIG_FILE: string
# :param MAX_LINE_LENGTH: override the maximum line length,
#   the default is defined in ament_uncrustify
# :type MAX_LINE_LENGTH: integer
# :param LANGUAGE: a specific language argument for uncrustify instead of
#   deriving the language from the file extension, either 'C' or 'C++'
# :type LANGUAGE: string
# :param EXCLUDE: an optional list of exclude files or directories for uncrustify
# :type EXCLUDE: list
# :param ARGN: the files or directories to check
# :type ARGN: list of strings
#
# @public
#
function(ament_uncrustify)
  cmake_parse_arguments(ARG "" "CONFIG_FILE;LANGUAGE;MAX_LINE_LENGTH;TESTNAME" "EXCLUDE" ${ARGN})
  if(NOT ARG_TESTNAME)
    set(ARG_TESTNAME "uncrustify")
  endif()

  find_program(ament_uncrustify_BIN NAMES "ament_uncrustify")
  if(NOT ament_uncrustify_BIN)
    message(FATAL_ERROR "ament_uncrustify() could not find program 'ament_uncrustify'")
  endif()

  set(result_file "${AMENT_TEST_RESULTS_DIR}/${PROJECT_NAME}/${ARG_TESTNAME}.xunit.xml")
  set(cmd "${ament_uncrustify_BIN}" "--xunit-file" "${result_file}")
  if(DEFINED ARG_MAX_LINE_LENGTH)
    list(APPEND cmd "--linelength" "${ARG_MAX_LINE_LENGTH}")
  endif()
  if(ARG_CONFIG_FILE)
    list(APPEND cmd "-c" "${ARG_CONFIG_FILE}")
  endif()
  if(ARG_LANGUAGE)
    string(TOUPPER ${ARG_LANGUAGE} ARG_LANGUAGE)
    list(APPEND cmd "--language" "${ARG_LANGUAGE}")
  endif()
  if(ARG_EXCLUDE)
    list(APPEND cmd "--exclude" "${ARG_EXCLUDE}")
  endif()
  list(APPEND cmd ${ARG_UNPARSED_ARGUMENTS})

  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/ament_uncrustify")
  ament_add_test(
    "${ARG_TESTNAME}"
    COMMAND ${cmd}
    OUTPUT_FILE "${CMAKE_BINARY_DIR}/ament_uncrustify/${ARG_TESTNAME}.txt"
    RESULT_FILE "${result_file}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  set_tests_properties(
    "${ARG_TESTNAME}"
    PROPERTIES
    LABELS "uncrustify;linter"
  )
endfunction()
