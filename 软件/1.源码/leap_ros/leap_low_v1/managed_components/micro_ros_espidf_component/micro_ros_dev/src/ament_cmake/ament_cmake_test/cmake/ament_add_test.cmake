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
# Add a test.
#
# A test is expected to generate a JUnit result file
# ${AMENT_TEST_RESULTS_DIR}/$PROJECT_NAME}/${testname}.xml.
# Failing to do so is considered a failed test.
#
# :param testname: the name of the test
# :type testname: string
# :param COMMAND: the command including its arguments to invoke
# :type COMMAND: list of strings
# :param OUTPUT_FILE: the path of the file to pipe the output to
# :type OUTPUT_FILE: string
# :param RUNNER: the path to the test runner script (default: run_test.py).
# :type RUNNER: string
# :param TIMEOUT: the test timeout in seconds, default: 60
# :type TIMEOUT: integer
# :param WORKING_DIRECTORY: the working directory for invoking the
#   command in, default: CMAKE_CURRENT_BINARY_DIR
# :type WORKING_DIRECTORY: string
# :param GENERATE_RESULT_FOR_RETURN_CODE_ZERO: generate a test result
#   file when the command invocation returns with code zero
#   command in, default: FALSE
# :type GENERATE_RESULT_FOR_RETURN_CODE_ZERO: option
# :param SKIP_TEST: if set mark the test as being skipped
# :type SKIP_TEST: option
# :param ENV: list of env vars to set; listed as ``VAR=value``
# :type ENV: list of strings
# :param APPEND_ENV: list of env vars to append if already set, otherwise set;
#   listed as ``VAR=value``
# :type APPEND_ENV: list of strings
# :param APPEND_LIBRARY_DIRS: list of library dirs to append to the appropriate
#   OS specific env var, a la LD_LIBRARY_PATH
# :type APPEND_LIBRARY_DIRS: list of strings
# :param SKIP_RETURN_CODE: return code signifying that the test has been
#   skipped and did not fail OR succeed
# :type SKIP_RETURN_CODE: integer
#
# @public
#
function(ament_add_test testname)
  cmake_parse_arguments(ARG
    "GENERATE_RESULT_FOR_RETURN_CODE_ZERO;SKIP_TEST"
    "OUTPUT_FILE;RESULT_FILE;RUNNER;SKIP_RETURN_CODE;TIMEOUT;WORKING_DIRECTORY"
    "APPEND_ENV;APPEND_LIBRARY_DIRS;COMMAND;ENV"
    ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_add_test() called with unused arguments: "
      "${ARG_UNPARSED_ARGUMENTS}")
  endif()
  if(NOT ARG_COMMAND)
    message(FATAL_ERROR
      "ament_add_test() must be invoked with the COMMAND argument")
  endif()
  if(NOT ARG_RESULT_FILE)
    set(ARG_RESULT_FILE "${AMENT_TEST_RESULTS_DIR}/${PROJECT_NAME}/${testname}.xml")
  endif()
  if(NOT ARG_RUNNER)
    set(ARG_RUNNER "${ament_cmake_test_DIR}/run_test.py")
  endif()
  if(NOT ARG_TIMEOUT)
    set(ARG_TIMEOUT 60)
  endif()
  if(NOT ARG_TIMEOUT GREATER 0)
    message(FATAL_ERROR "ament_add_test() the TIMEOUT argument must be a "
      "valid number and greater than zero")
  endif()
  if(NOT ARG_WORKING_DIRECTORY)
    set(ARG_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
  endif()

  get_executable_path(python_interpreter Python3::Interpreter BUILD)
  # wrap command with run_test script to ensure test result generation
  set(cmd_wrapper "${python_interpreter}" "-u" "${ARG_RUNNER}"
    "${ARG_RESULT_FILE}"
    "--package-name" "${PROJECT_NAME}")
  if(ARG_SKIP_TEST)
    list(APPEND cmd_wrapper "--skip-test")
    set(ARG_SKIP_RETURN_CODE 0)
  elseif(ARG_SKIP_RETURN_CODE)
    list(APPEND cmd_wrapper "--skip-return-code" "${ARG_SKIP_RETURN_CODE}")
  endif()
  if(ARG_GENERATE_RESULT_FOR_RETURN_CODE_ZERO)
    list(APPEND cmd_wrapper "--generate-result-on-success")
  endif()
  if(ARG_OUTPUT_FILE)
    list(APPEND cmd_wrapper "--output-file" "${ARG_OUTPUT_FILE}")
  endif()
  if(ARG_ENV)
    list(APPEND cmd_wrapper "--env" ${ARG_ENV})
  endif()
  if(ARG_APPEND_LIBRARY_DIRS)
    if(WIN32)
      set(_library_dirs_env_var "PATH")
    elseif(APPLE)
      set(_library_dirs_env_var "DYLD_LIBRARY_PATH")
    elseif(UNIX)
      set(_library_dirs_env_var "LD_LIBRARY_PATH")
    else()
      message(FATAL_ERROR "Unknown platform for environment variable to find libraries")
    endif()
    foreach(_dir ${ARG_APPEND_LIBRARY_DIRS})
      list(APPEND ARG_APPEND_ENV "${_library_dirs_env_var}=${_dir}")
    endforeach()
  endif()
  if(ARG_APPEND_ENV)
    list(APPEND cmd_wrapper "--append-env" ${ARG_APPEND_ENV})
  endif()
  list(APPEND cmd_wrapper "--command" ${ARG_COMMAND})

  add_test(
    NAME "${testname}"
    COMMAND ${cmd_wrapper}
    WORKING_DIRECTORY "${ARG_WORKING_DIRECTORY}"
  )
  set_tests_properties(
    "${testname}"
    PROPERTIES TIMEOUT ${ARG_TIMEOUT}
  )
  if(DEFINED ARG_SKIP_RETURN_CODE)
    set_tests_properties(
      "${testname}"
      PROPERTIES SKIP_RETURN_CODE ${ARG_SKIP_RETURN_CODE}
    )
  endif()
endfunction()
