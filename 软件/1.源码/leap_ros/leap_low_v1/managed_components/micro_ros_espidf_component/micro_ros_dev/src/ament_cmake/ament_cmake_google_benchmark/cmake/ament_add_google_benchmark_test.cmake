# Copyright 2020 Open Source Robotics Foundation, Inc.
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
# Add an existing executable using google benchmark as a test.
#
# Register an executable created with ament_add_google_benchmark_executable() as
# a test.
# If the specified target does not exist the registration is skipped.
#
# :param target: the target name which will also be used as the test name
# :type target: string
# :param RUNNER: the path to the test runner script (default: see
#   ament_add_test).
# :type RUNNER: string
# :param TIMEOUT: the test timeout in seconds,
#   default defined by ``ament_add_test()``
# :type TIMEOUT: integer
# :param WORKING_DIRECTORY: the working directory for invoking the
#   executable in, default defined by ``ament_add_test()``
# :type WORKING_DIRECTORY: string
# :param RUN_PARALLEL: if set allow the test to be run in parallel
#   with other tests
# :type RUN_PARALLEL: option
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
#
# @public
#
function(ament_add_google_benchmark_test target)
  if(NOT TARGET ${target})
    return()
  endif()

  cmake_parse_arguments(ARG
    "RUN_PARALLEL;SKIP_TEST"
    "RUNNER;TIMEOUT;WORKING_DIRECTORY"
    "APPEND_ENV;APPEND_LIBRARY_DIRS;ENV"
    ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "ament_add_google_benchmark_test() called with unused arguments: ${ARGN}")
  endif()

  if(AMENT_CMAKE_GOOGLE_BENCHMARK_OVERLAY AND NOT IS_ABSOLUTE ${AMENT_CMAKE_GOOGLE_BENCHMARK_OVERLAY})
    get_filename_component(AMENT_CMAKE_GOOGLE_BENCHMARK_OVERLAY
      ${CMAKE_CURRENT_SOURCE_DIR}/${AMENT_CMAKE_GOOGLE_BENCHMARK_OVERLAY} ABSOLUTE)
    set(OVERLAY_ARG "--result-file-overlay" "${AMENT_CMAKE_GOOGLE_BENCHMARK_OVERLAY}")
  endif()

  get_executable_path(python_interpreter Python3::Interpreter BUILD)

  set(executable "$<TARGET_FILE:${target}>")
  set(benchmark_out "${AMENT_TEST_RESULTS_DIR}/${PROJECT_NAME}/${target}.google_benchmark.json")
  set(common_out "${AMENT_TEST_RESULTS_DIR}/${PROJECT_NAME}/${target}.benchmark.json")
  set(cmd
    "${python_interpreter}" "-u" "${ament_cmake_google_benchmark_DIR}/run_and_convert.py"
    "${benchmark_out}" "${common_out}" "--package-name" "${PROJECT_NAME}"
    ${OVERLAY_ARG}
    "--command" "${executable}"
    "--benchmark_out_format=json" "--benchmark_out=${benchmark_out}")
  if(ARG_ENV)
    set(ARG_ENV "ENV" ${ARG_ENV})
  endif()
  if(ARG_APPEND_ENV)
    set(ARG_APPEND_ENV "APPEND_ENV" ${ARG_APPEND_ENV})
  endif()
  if(ARG_APPEND_LIBRARY_DIRS)
    set(ARG_APPEND_LIBRARY_DIRS "APPEND_LIBRARY_DIRS" ${ARG_APPEND_LIBRARY_DIRS})
  endif()
  if(ARG_RUNNER)
    set(ARG_RUNNER "RUNNER" ${ARG_RUNNER})
  endif()
  if(ARG_TIMEOUT)
    set(ARG_TIMEOUT "TIMEOUT" ${ARG_TIMEOUT})
  endif()
  if(ARG_WORKING_DIRECTORY)
    set(ARG_WORKING_DIRECTORY "WORKING_DIRECTORY" "${ARG_WORKING_DIRECTORY}")
  endif()
  if(NOT ARG_RUN_PARALLEL)
    set(PROP_RUN_SERIAL "RUN_SERIAL" "TRUE")
  endif()
  if(ARG_SKIP_TEST OR NOT AMENT_RUN_PERFORMANCE_TESTS)
    set(ARG_SKIP_TEST "SKIP_TEST")
  endif()

  ament_add_test(
    "${target}"
    COMMAND ${cmd}
    OUTPUT_FILE "${CMAKE_BINARY_DIR}/ament_cmake_google_benchmark/${target}.txt"
    GENERATE_RESULT_FOR_RETURN_CODE_ZERO
    ${ARG_SKIP_TEST}
    ${ARG_ENV}
    ${ARG_APPEND_ENV}
    ${ARG_APPEND_LIBRARY_DIRS}
    ${ARG_TIMEOUT}
    ${ARG_WORKING_DIRECTORY}
  )
  set_tests_properties(
    "${target}"
    PROPERTIES
    REQUIRED_FILES "${executable}"
    LABELS "google_benchmark;performance"
    ${PROP_RUN_SERIAL}
  )
endfunction()
