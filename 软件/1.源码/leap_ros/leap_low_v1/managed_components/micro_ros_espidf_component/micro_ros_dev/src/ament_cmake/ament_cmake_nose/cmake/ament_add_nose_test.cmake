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
# Add a nose test.
#
# :param testname: the name of the test
# :type testname: string
# :param path: the path to a file or folder where ``nosetests``
#   should be invoked on
# :type path: string
# :param SKIP_TEST: if set mark the test as being skipped
# :type SKIP_TEST: option
# :param PYTHON_EXECUTABLE: Python executable used to run the test.
#   It defaults to the CMake executable target Python3::Interpreter.
# :type PYTHON_EXECUTABLE: string
# :param RUNNER: the path to the test runner script (default: see ament_add_test).
# :type RUNNER: string
# :param TIMEOUT: the test timeout in seconds,
#   default defined by ``ament_add_test()``
# :type TIMEOUT: integer
# :param WORKING_DIRECTORY: the working directory for invoking the
#   command in, default defined by ``ament_add_test()``
# :type WORKING_DIRECTORY: string
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
macro(ament_add_nose_test testname path)
  _ament_cmake_nose_find_nosetests()
  if(NOSETESTS)
    _ament_add_nose_test("${testname}" "${path}" ${ARGN})
  endif()
endmacro()

function(_ament_add_nose_test testname path)
  cmake_parse_arguments(ARG
    "SKIP_TEST"
    "PYTHON_EXECUTABLE;RUNNER;TIMEOUT;WORKING_DIRECTORY"
    "APPEND_ENV;APPEND_LIBRARY_DIRS;ENV"
    ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_add_nose_test() called with unused arguments: "
      "${ARG_UNPARSED_ARGUMENTS}")
  endif()

  if(NOT IS_ABSOLUTE "${path}")
    set(path "${CMAKE_CURRENT_SOURCE_DIR}/${path}")
  endif()
  # only check existence of path if it doesn't contain generator expressions
  string(FIND "${path}" "$<" index)
  if(index EQUAL -1 AND NOT EXISTS "${path}")
    message(FATAL_ERROR
      "ament_add_nose_test() the path '${path}' does not exist")
  endif()
  if(NOT ARG_PYTHON_EXECUTABLE)
    set(ARG_PYTHON_EXECUTABLE Python3::Interpreter)
  endif()

  get_executable_path(python_interpreter "${ARG_PYTHON_EXECUTABLE}" BUILD)

  set(result_file "${AMENT_TEST_RESULTS_DIR}/${PROJECT_NAME}/${testname}.xunit.xml")
  # Invoke ${NOSETESTS} explicitly with the ${PYTHON_EXECUTABLE} because on
  # some systems, like OS X, the ${NOSETESTS} binary may have a #! which points
  # to the Python2 on the system, rather than the Python3 which is what we want
  # most of the time.
  # This "misalignment" can occur when you do `pip install -U nose` after doing
  # `pip3 install -U nose` because both install the `nose` binary.
  # Basically the last Python system to install nose will determine what the
  # ${NOSETESTS} executable references.
  # See: https://github.com/ament/ament_cmake/pull/70
  set(cmd
    "${python_interpreter}"
    "-u"  # unbuffered stdout and stderr
    "${NOSETESTS}" "${path}"
    "--nocapture"  # stdout will be printed immediately
    "--with-xunit" "--xunit-file=${result_file}")
  if(NOT "${NOSETESTS_VERSION}" VERSION_LESS "1.3.5")
    list(APPEND cmd "--xunit-testsuite-name=${PROJECT_NAME}.nosetests")
    if(NOT "${NOSETESTS_VERSION}" VERSION_LESS "1.3.8")
      list(APPEND cmd "--xunit-prefix-with-testsuite-name")
    endif()
  endif()

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
    set(ARG_TIMEOUT "TIMEOUT" "${ARG_TIMEOUT}")
  endif()
  if(ARG_WORKING_DIRECTORY)
    set(ARG_WORKING_DIRECTORY "WORKING_DIRECTORY" "${ARG_WORKING_DIRECTORY}")
  endif()
  if(ARG_SKIP_TEST)
    set(ARG_SKIP_TEST "SKIP_TEST")
  endif()

  ament_add_test(
    "${testname}"
    COMMAND ${cmd}
    OUTPUT_FILE "${CMAKE_BINARY_DIR}/ament_cmake_nose/${testname}.txt"
    RESULT_FILE "${result_file}"
    ${ARG_RUNNER}
    ${ARG_SKIP_TEST}
    ${ARG_ENV}
    ${ARG_APPEND_ENV}
    ${ARG_APPEND_LIBRARY_DIRS}
    ${ARG_TIMEOUT}
    ${ARG_WORKING_DIRECTORY}
  )
  set_tests_properties(
    "${testname}"
    PROPERTIES
    LABELS "nose"
  )
endfunction()
