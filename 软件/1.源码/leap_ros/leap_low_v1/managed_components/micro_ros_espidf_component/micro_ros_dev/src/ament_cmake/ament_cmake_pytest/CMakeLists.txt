cmake_minimum_required(VERSION 3.12)

project(ament_cmake_pytest NONE)

find_package(ament_cmake_core REQUIRED)

# Even though we theoretically only need this as a test dependency, we
# have to find it *before* the if(BUILD_TESTING) check since ament_cmake_test
# is the package that defines that option.
find_package(ament_cmake_test REQUIRED)

if(BUILD_TESTING)
  # The test we are enabling below is to test that the code in ament_cmake_test can handle
  # non-unicode prints on the console. Ideally we would put the test in that package,
  # but we can't actually use ament_add_test() from within its own package so
  # we put it here instead. For similar reasons, we don't use ament_add_pytest_test() below since
  # it is defined in *this* package.

  get_executable_path(python_interpreter Python3::Interpreter CONFIGURE)

  # Check if pytest is available
  set(check_pytest_cmd "${python_interpreter}" "-m" "pytest" "--version")
  set(SKIP_TEST_ARG "")
  execute_process(
    COMMAND ${check_pytest_cmd}
    RESULT_VARIABLE res
    OUTPUT_VARIABLE output
    ERROR_VARIABLE error)
  if(NOT res EQUAL 0)
    message(WARNING
      "The Python module 'pytest' was not found, pytests cannot be run. "
      "On Linux, install the 'python3-pytest' package. "
      "On other platforms, install 'pytest' using pip.")
    set(SKIP_TEST_ARG SKIP_TEST)
  endif()

  set(result_file "${AMENT_TEST_RESULTS_DIR}/${PROJECT_NAME}/pytest.xunit.xml")
  set(cmd
    "${python_interpreter}"
    "-u"  # unbuffered stdout and stderr
    "-m" "pytest"
    "${CMAKE_CURRENT_SOURCE_DIR}/test"
    # store last failed tests
    "-o" "cache_dir=${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_pytest/pytest/.cache"
    "-s"
    # junit arguments
    "--junit-xml=${result_file}"
    "--junit-prefix=${PROJECT_NAME}"
  )
  ament_add_test(
    pytest
    COMMAND ${cmd}
    OUTPUT_FILE "${CMAKE_BINARY_DIR}/ament_cmake_pytest/pytest.txt"
    RESULT_FILE "${result_file}"
    ${SKIP_TEST_ARG}
  )
  set_tests_properties(
    pytest
    PROPERTIES
    LABELS "pytest"
  )
endif()

ament_package(
  CONFIG_EXTRAS "ament_cmake_pytest-extras.cmake"
)

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)
