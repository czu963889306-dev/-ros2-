# Copyright 2017 Open Source Robotics Foundation, Inc.
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
# Add a pytest test.
#
# :param testname: the name of the test
# :type testname: string
# :param path: the path to a file or folder where ``pytest`` should be invoked
#   on
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
# :param WERROR: If ON, then treat warnings as errors. Default: OFF.
# :type WERROR: bool
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
function(ament_add_pytest_test testname path)
  cmake_parse_arguments(ARG
    "SKIP_TEST"
    "PYTHON_EXECUTABLE;RUNNER;TIMEOUT;WERROR;WORKING_DIRECTORY"
    "APPEND_ENV;APPEND_LIBRARY_DIRS;ENV"
    ${ARGN})
  if(ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_add_pytest_test() called with unused arguments: "
      "${ARG_UNPARSED_ARGUMENTS}")
  endif()

  # check arguments
  if(NOT IS_ABSOLUTE "${path}")
    set(path "${CMAKE_CURRENT_SOURCE_DIR}/${path}")
  endif()
  # only check existence of path if it doesn't contain generator expressions
  string(FIND "${path}" "$<" index)
  if(index EQUAL -1 AND NOT EXISTS "${path}")
    message(FATAL_ERROR
      "ament_add_pytest_test() the path '${path}' does not exist")
  endif()
  if(NOT ARG_PYTHON_EXECUTABLE)
    set(ARG_PYTHON_EXECUTABLE Python3::Interpreter)
  endif()

  get_executable_path(python_interpreter "${ARG_PYTHON_EXECUTABLE}" BUILD)

  # ensure pytest is available
  ament_has_pytest(has_pytest QUIET PYTHON_EXECUTABLE "${ARG_PYTHON_EXECUTABLE}")
  if(NOT has_pytest)
    message(WARNING
      "The Python module 'pytest' was not found, pytests cannot be run. "
      "On Linux, install the 'python3-pytest' package. "
      "On other platforms, install 'pytest' using pip.")
    return()
  endif()

  set(result_file "${AMENT_TEST_RESULTS_DIR}/${PROJECT_NAME}/${testname}.xunit.xml")
  set(cmd
    "${python_interpreter}"
    "-u"  # unbuffered stdout and stderr
    "-m" "pytest"
    "${path}"
    # store last failed tests
    "-o" "cache_dir=${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_pytest/${testname}/.cache"
    # junit arguments
    "--junit-xml=${result_file}"
    "--junit-prefix=${PROJECT_NAME}"
  )

  if(ARG_WERROR)
    # treat warnings as errors
    list(APPEND cmd "-We")
  endif()

  # enable pytest coverage by default if the package test_depends on python3-pytest-cov
  if("python3-pytest-cov" IN_LIST ${PROJECT_NAME}_TEST_DEPENDS)
    set(coverage_default ON)
  else()
    set(coverage_default OFF)
  endif()
  option(AMENT_CMAKE_PYTEST_WITH_COVERAGE
    "Generate coverage information for Python tests"
    ${coverage_default})

  if(AMENT_CMAKE_PYTEST_WITH_COVERAGE)
    # get pytest-cov version, if available
    ament_get_pytest_cov_version(pytest_cov_version
      PYTHON_EXECUTABLE "${ARG_PYTHON_EXECUTABLE}"
    )
    if(NOT pytest_cov_version)
      message(WARNING
        "The Python module 'pytest-cov' was not found, test coverage will not be produced. "
        "On Linux, install the 'python3-pytest-cov' package. "
        "On other platforms, install 'pytest-cov' using pip.")
    else()
      set(coverage_directory "${CMAKE_CURRENT_BINARY_DIR}/pytest_cov/${testname}")
      file(MAKE_DIRECTORY "${coverage_directory}")

      list(APPEND cmd
        "--cov=${CMAKE_CURRENT_SOURCE_DIR}"
        "--cov-report=html:${coverage_directory}/coverage.html"
        "--cov-report=xml:${coverage_directory}/coverage.xml"
      )

      if(pytest_cov_version VERSION_LESS "2.5.0")
        message(WARNING
          "Test coverage will be produced, but will not contain branch coverage information, "
          "because the pytest extension 'cov' does not support it "
          "(need 2.5.0, found '${pytest_cov_version}').")
      else()
        list(APPEND cmd "--cov-branch")
      endif()

      list(APPEND ARG_ENV "COVERAGE_FILE=${coverage_directory}/.coverage")
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
    OUTPUT_FILE "${CMAKE_BINARY_DIR}/ament_cmake_pytest/${testname}.txt"
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
    LABELS "pytest"
  )
endfunction()
