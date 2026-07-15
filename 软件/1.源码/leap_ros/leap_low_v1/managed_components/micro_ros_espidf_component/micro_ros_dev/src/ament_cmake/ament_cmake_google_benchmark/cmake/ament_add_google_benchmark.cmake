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
# Add a google benchmark test.
#
# Call add_executable(target ARGN), link it against the google benchmark
# libraries and register the executable as a test.
#
# If google benchmark is not available the specified target is not being created
# and therefore the target existence should be checked before being used.
#
# :param target: the target name which will also be used as the test name
# :type target: string
# :param ARGN: the list of source files
# :type ARGN: list of strings
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
# :param SKIP_LINKING_MAIN_LIBRARIES: if set skip linking against the google
#   benchmark main libraries
# :type SKIP_LINKING_MAIN_LIBRARIES: option
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
macro(ament_add_google_benchmark target)
  cmake_parse_arguments(_ARG
    "RUN_PARALLEL;SKIP_LINKING_MAIN_LIBRARIES;SKIP_TEST"
    "RUNNER;TIMEOUT;WORKING_DIRECTORY"
    "APPEND_ENV;APPEND_LIBRARY_DIRS;ENV"
    ${ARGN})
  if(NOT _ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "ament_add_google_benchmark() must be invoked with at least one "
      "source file")
  endif()

  # add executable
  set(_argn_executable ${_ARG_UNPARSED_ARGUMENTS})
  if(_ARG_SKIP_LINKING_MAIN_LIBRARIES)
    list(APPEND _argn_executable "SKIP_LINKING_MAIN_LIBRARIES")
  endif()
  ament_add_google_benchmark_executable("${target}" ${_argn_executable})

  # add test
  set(_argn_test "")
  if(_ARG_RUNNER)
    list(APPEND _argn_test "RUNNER" "${_ARG_RUNNER}")
  endif()
  if(_ARG_TIMEOUT)
    list(APPEND _argn_test "TIMEOUT" "${_ARG_TIMEOUT}")
  endif()
  if(_ARG_WORKING_DIRECTORY)
    list(APPEND _argn_test "WORKING_DIRECTORY" "${_ARG_WORKING_DIRECTORY}")
  endif()
  if(_ARG_RUN_PARALLEL)
    list(APPEND _argn_test "RUN_PARALLEL")
  endif()
  if(_ARG_SKIP_TEST)
    list(APPEND _argn_test "SKIP_TEST")
  endif()
  if(_ARG_ENV)
    list(APPEND _argn_test "ENV" ${_ARG_ENV})
  endif()
  if(_ARG_APPEND_ENV)
    list(APPEND _argn_test "APPEND_ENV" ${_ARG_APPEND_ENV})
  endif()
  if(_ARG_APPEND_LIBRARY_DIRS)
    list(APPEND _argn_test "APPEND_LIBRARY_DIRS" ${_ARG_APPEND_LIBRARY_DIRS})
  endif()
  ament_add_google_benchmark_test("${target}" ${_argn_test})
endmacro()
