cmake_minimum_required(VERSION 3.12)

project(rcutils)

option(RCUTILS_NO_THREAD_SUPPORT "Disable thread support." OFF)
option(RCUTILS_NO_FILESYSTEM "Disable filesystem usage." OFF)
option(RCUTILS_AVOID_DYNAMIC_ALLOCATION "Disable dynamic allocations." OFF)
option(RCUTILS_NO_64_ATOMIC "Enable alternative support for 64 bits atomic operations in platforms with no native support." OFF)
option(RCUTILS_MICROROS "Flag for building micro-ROS." ON)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()
# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

include(CheckLibraryExists)

find_package(ament_cmake_python REQUIRED)
find_package(ament_cmake_ros REQUIRED)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

ament_python_install_package(${PROJECT_NAME})

if(UNIX AND NOT APPLE)
  include(cmake/check_c_compiler_uses_glibc.cmake)
  check_c_compiler_uses_glibc(USES_GLIBC)
  if(USES_GLIBC)
    # Ensure GNU extended libc API is used, as C++ test code will.
    # See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=2082.
    add_definitions(-D_GNU_SOURCE)
  endif()
endif()

if(NOT RCUTILS_MICROROS AND (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
  # enables building a static library but later link it into a dynamic library
  add_compile_options(-fPIC)
endif()
if(NOT WIN32)
  # About -Wno-sign-conversion: With Clang, -Wconversion implies -Wsign-conversion. There are a number of
  # implicit sign conversions in gtest.cc, see https://ci.ros2.org/job/ci_osx/9381/clang/.
  # Hence disabling -Wsign-conversion for now until all those have eventually been fixed.
  # (from https://github.com/ros2/rcutils/pull/263#issuecomment-663252537)
  add_compile_options(-Wall -Wextra -Wconversion -Wno-sign-conversion -Wpedantic)
endif()

if(WIN32)
  set(time_impl_c src/time_win32.c)
else()
  set(time_impl_c src/time_unix.c)
endif()

set(rcutils_sources
  src/allocator.c
  src/array_list.c
  src/char_array.c
  src/cmdline_parser.c
  src/env.c
  src/error_handling.c
  src/filesystem.c
  src/find.c
  src/format_string.c
  src/hash_map.c
  src/logging.c
  src/process.c
  src/qsort.c
  src/repl_str.c
  src/shared_library.c
  src/snprintf.c
  src/split.c
  src/strcasecmp.c
  src/strdup.c
  src/strerror.c
  src/string_array.c
  src/string_map.c
  src/testing/fault_injection.c
  src/time.c
  ${time_impl_c}
  src/uint8_array.c
  $<$<BOOL:${RCUTILS_NO_64_ATOMIC}>:src/atomic_64bits.c>
)
set_source_files_properties(
  ${rcutils_sources}
  PROPERTIES language "C")

# "watch" template/inputs for changes
configure_file(
  "resource/logging_macros.h.em"
  "logging_macros.h.em.watch"
  COPYONLY)
configure_file(
  "rcutils/logging.py"
  "logging.py.watch"
  COPYONLY)
# generate header with logging macros
set(rcutils_module_path ${CMAKE_CURRENT_SOURCE_DIR})
set(python_code
  "import em"  # implicitly added ; between python statements due to CMake list
  "\
em.invoke( \
  [ \
    '-o', 'include/rcutils/logging_macros.h', \
    '-D', 'rcutils_module_path=\"${rcutils_module_path}\"', \
    '${CMAKE_CURRENT_SOURCE_DIR}/resource/logging_macros.h.em' \
  ] \
)")
string(REPLACE ";" "$<SEMICOLON>" python_code "${python_code}")
add_custom_command(OUTPUT include/rcutils/logging_macros.h
  COMMAND ${CMAKE_COMMAND} -E make_directory "include/rcutils"
  COMMAND Python3::Interpreter ARGS -c "${python_code}"
  DEPENDS
    "${CMAKE_CURRENT_BINARY_DIR}/logging_macros.h.em.watch"
    "${CMAKE_CURRENT_BINARY_DIR}/logging.py.watch"
  COMMENT "Expanding logging_macros.h.em"
  VERBATIM
)
list(APPEND rcutils_sources
  include/rcutils/logging_macros.h)

add_library(
  ${PROJECT_NAME}
  ${rcutils_sources})
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

# Causes the visibility macros to use dllexport rather than dllimport,
# which is appropriate when building the dll but not consuming it.
target_compile_definitions(${PROJECT_NAME} PRIVATE "RCUTILS_BUILDING_DLL")

if(BUILD_TESTING AND NOT RCUTILS_DISABLE_FAULT_INJECTION)
  target_compile_definitions(${PROJECT_NAME} PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
endif()
configure_file(
    "${PROJECT_SOURCE_DIR}/include/rcutils/configuration_flags.h.in"
    "${PROJECT_BINARY_DIR}/include/rcutils/configuration_flags.h"
)

if(NOT RCUTILS_NO_FILESYSTEM)
  target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
endif()

check_library_exists(atomic __atomic_load_8 "" HAVE_LIBATOMICS)

if(HAVE_LIBATOMICS AND NOT WIN32)
  # Exporting link flag since it won't pass ament_export_libraries() existence check
  ament_export_link_flags("-latomic")
  target_link_libraries(${PROJECT_NAME} atomic)
endif()

# Needed if pthread is used for thread local storage.
if(IOS AND IOS_SDK_VERSION LESS 10.0)
  ament_export_libraries(pthread)
endif()

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

if(BUILD_TESTING)
  if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
  endif()

  find_package(performance_test_fixture REQUIRED)

  find_package(ament_cmake_gmock REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_cmake_pytest REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(launch_testing_ament_cmake REQUIRED)

  if(ament_cmake_cppcheck_FOUND)
    ament_cppcheck(
      TESTNAME "cppcheck_logging_macros"
      "${CMAKE_CURRENT_BINARY_DIR}/include/rcutils/logging_macros.h")
  endif()
  if(ament_cmake_cpplint_FOUND)
    ament_cpplint(
      TESTNAME "cpplint_logging_macros"
      # the generated code might contain longer lines for templated types
      MAX_LINE_LENGTH 999
      ROOT "${CMAKE_CURRENT_BINARY_DIR}/include"
      "${CMAKE_CURRENT_BINARY_DIR}/include/rcutils/logging_macros.h")
  endif()
  if(ament_cmake_uncrustify_FOUND)
    ament_uncrustify(
      TESTNAME "uncrustify_logging_macros"
      # the generated code might contain longer lines for templated types
      MAX_LINE_LENGTH 0
      "${CMAKE_CURRENT_BINARY_DIR}/include/rcutils/logging_macros.h")
  endif()

  find_package(mimick_vendor REQUIRED)

  find_package(osrf_testing_tools_cpp REQUIRED)
  get_target_property(memory_tools_test_env_vars
    osrf_testing_tools_cpp::memory_tools LIBRARY_PRELOAD_ENVIRONMENT_VARIABLE)
  get_target_property(memory_tools_is_available
    osrf_testing_tools_cpp::memory_tools LIBRARY_PRELOAD_ENVIRONMENT_IS_AVAILABLE)

  ament_add_gtest(test_logging test/test_logging.cpp)
  target_link_libraries(test_logging ${PROJECT_NAME} mimick osrf_testing_tools_cpp::memory_tools)

  add_executable(test_logging_long_messages test/test_logging_long_messages.cpp)
  target_link_libraries(test_logging_long_messages ${PROJECT_NAME})
  add_launch_test(
    "test/test_logging_long_messages.py"
    TARGET test_logging_long_messages
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:test_logging_long_messages>"
    TIMEOUT 10
  )

  add_launch_test(
    "test/test_logging_output_format.py"
    TARGET test_logging_output_format
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:test_logging_long_messages>"
    TIMEOUT 10
  )

  ament_add_gmock(test_logging_macros test/test_logging_macros.cpp)
  target_link_libraries(test_logging_macros ${PROJECT_NAME})

  add_executable(test_logging_macros_c test/test_logging_macros.c)
  target_link_libraries(test_logging_macros_c ${PROJECT_NAME})
  ament_add_test(test_logging_macros_c
    COMMAND "$<TARGET_FILE:test_logging_macros_c>"
    GENERATE_RESULT_FOR_RETURN_CODE_ZERO)

  set(SKIP_MEMORY_TOOLS_TEST "")
  if(NOT memory_tools_is_available)
    set(SKIP_MEMORY_TOOLS_TEST "SKIP_TEST")
  endif()

  macro(rcutils_custom_add_gtest target)
    ament_add_gtest(${target} ${ARGN})
  endmacro()

  macro(rcutils_custom_add_gmock target)
    ament_add_gmock(${target} ${ARGN})
  endmacro()

  # Gtests
  rcutils_custom_add_gtest(test_allocator test/test_allocator.cpp
    ENV ${memory_tools_test_env_vars}
    ${SKIP_MEMORY_TOOLS_TEST}
  )
  if(TARGET test_allocator)
    target_link_libraries(test_allocator ${PROJECT_NAME} osrf_testing_tools_cpp::memory_tools)
  endif()

  rcutils_custom_add_gtest(test_char_array
    test/test_char_array.cpp
  )
  if(TARGET test_char_array)
    target_link_libraries(test_char_array ${PROJECT_NAME} mimick)
  endif()

  # Can't use C++ with stdatomic_helper.h
  add_executable(test_atomics_executable
    test/test_atomics.c
  )
  set_target_properties(test_atomics_executable
    PROPERTIES
      LANGUAGE C
  )
  target_link_libraries(test_atomics_executable ${PROJECT_NAME})
  if(HAVE_LIBATOMICS)
    target_link_libraries(test_atomics_executable atomic)
  endif()

  add_test(NAME test_atomics COMMAND test_atomics_executable)

  rcutils_custom_add_gmock(test_error_handling test/test_error_handling.cpp
    # Append the directory of librcutils so it is found at test time.
    APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    ENV ${memory_tools_test_env_vars}
  )
  if(TARGET test_error_handling)
    target_link_libraries(test_error_handling ${PROJECT_NAME} osrf_testing_tools_cpp::memory_tools)
  endif()

  rcutils_custom_add_gmock(test_error_handling_helpers test/test_error_handling_helpers.cpp
    # Append the directory of librcutils so it is found at test time.
    APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    ENV ${memory_tools_test_env_vars}
  )
  if(TARGET test_error_handling_helpers)
    target_include_directories(test_error_handling_helpers PUBLIC include)
    target_link_libraries(test_error_handling_helpers ${PROJECT_NAME} osrf_testing_tools_cpp::memory_tools)
  endif()

  rcutils_custom_add_gtest(test_split
    test/test_split.cpp
  )
  if(TARGET test_split)
    target_link_libraries(test_split ${PROJECT_NAME} mimick)
  endif()

  rcutils_custom_add_gtest(test_find
    test/test_find.cpp
  )
  if(TARGET test_find)
    target_link_libraries(test_find ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_strerror
    test/test_strerror.cpp
  )
  if(TARGET test_strerror)
    target_link_libraries(test_strerror ${PROJECT_NAME} mimick)
  endif()

  rcutils_custom_add_gtest(test_string_array
    test/test_string_array.cpp
  )
  if(TARGET test_string_array)
    target_link_libraries(test_string_array ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_env test/test_env.cpp
    ENV
      EMPTY_TEST=
      NORMAL_TEST=foo
    APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
  )
  if(TARGET test_env)
    target_link_libraries(test_env ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_filesystem
    test/test_filesystem.cpp
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
  if(TARGET test_filesystem)
    ament_target_dependencies(test_filesystem "osrf_testing_tools_cpp")
    target_link_libraries(test_filesystem ${PROJECT_NAME} mimick)
    target_compile_definitions(test_filesystem PRIVATE BUILD_DIR="${CMAKE_CURRENT_BINARY_DIR}")
  endif()

  rcutils_custom_add_gtest(test_strdup
    test/test_strdup.cpp
  )
  if(TARGET test_strdup)
    target_link_libraries(test_strdup ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_format_string
    test/test_format_string.cpp
  )
  if(TARGET test_format_string)
    target_link_libraries(test_format_string ${PROJECT_NAME} mimick)
  endif()

  rcutils_custom_add_gtest(test_string_map
    test/test_string_map.cpp
  )
  if(TARGET test_string_map)
    ament_target_dependencies(test_string_map "osrf_testing_tools_cpp")
    target_link_libraries(test_string_map ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_isalnum_no_locale
    test/test_isalnum_no_locale.cpp
  )
  if(TARGET test_isalnum_no_locale)
    target_link_libraries(test_isalnum_no_locale ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_repl_str
    test/test_repl_str.cpp
  )
  if(TARGET test_repl_str)
    target_link_libraries(test_repl_str ${PROJECT_NAME})
  endif()

  macro(add_dummy_shared_library target)
    add_library(${target} test/dummy_shared_library/dummy_shared_library.c)
    if(WIN32)
      # Causes the visibility macros to use dllexport rather than dllimport
      # which is appropriate when building the dll but not consuming it.
      target_compile_definitions(${target} PRIVATE "DUMMY_SHARED_LIBRARY_BUILDING_DLL")
    endif()
  endmacro()

  ament_add_gtest(test_shared_library_in_run_paths test/test_shared_library.cpp)
  if(TARGET test_shared_library_in_run_paths)
    # Rely on CMake setting build tree RUNPATHs by default on Unix systems.
    add_dummy_shared_library(dummy_shared_library_in_run_paths)
    target_compile_definitions(test_shared_library_in_run_paths PRIVATE
      "SHARED_LIBRARY_UNDER_TEST=dummy_shared_library_in_run_paths")
    if(NOT WIN32 AND NOT APPLE)
      # NOTE(hidmic): DT_RUNPATH entries are ignored by dlopen in Linux despite the fact
      # documentation says otherwise, so here we fallback to DT_RPATH entries.
      target_link_libraries(test_shared_library_in_run_paths "-Wl,--disable-new-dtags")
    endif()
    target_link_libraries(test_shared_library_in_run_paths ${PROJECT_NAME} mimick)
  endif()

  set(project_binary_dir "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
  set(test_libraries_dir "$<TARGET_FILE_DIR:${PROJECT_NAME}>/test_libraries")
  ament_add_gtest(test_shared_library_in_load_paths test/test_shared_library.cpp
    APPEND_LIBRARY_DIRS ${project_binary_dir} ${test_libraries_dir}
  )
  if(TARGET test_shared_library_in_load_paths)
    # Make sure CMake adds no RPATHs/RUNPATHs.
    set_target_properties(test_shared_library_in_load_paths
      PROPERTIES SKIP_BUILD_RPATH TRUE)
    add_dummy_shared_library(dummy_shared_library_in_load_paths)
    set_target_properties(dummy_shared_library_in_load_paths PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY ${test_libraries_dir})
    target_compile_definitions(test_shared_library_in_load_paths PRIVATE
      "SHARED_LIBRARY_UNDER_TEST=dummy_shared_library_in_load_paths")
    target_link_libraries(test_shared_library_in_load_paths ${PROJECT_NAME} mimick)
  endif()

  include(CheckCXXCompilerFlag)
  macro(check_cxx_linker_flag flag var)
    set(CMAKE_REQUIRED_FLAGS ${flag})
    check_cxx_compiler_flag("" ${var})
    set(CMAKE_REQUIRED_FLAGS)
  endmacro()

  ament_add_gtest(test_shared_library_preloaded test/test_shared_library.cpp)
  if(TARGET test_shared_library_preloaded)
    add_dummy_shared_library(dummy_shared_library_preloaded)
    set_target_properties(dummy_shared_library_preloaded PROPERTIES
      LIBRARY_OUTPUT_DIRECTORY ${test_libraries_dir})
    target_compile_definitions(test_shared_library_preloaded PRIVATE
      "SHARED_LIBRARY_UNDER_TEST=dummy_shared_library_preloaded")
    if(NOT WIN32)
      # Force (apparently) unused libraries to be linked in.
      check_cxx_linker_flag("-Wl,--no-as-needed" is_no_as_needed_supported)
      if(NOT is_no_as_needed_supported)
        check_cxx_linker_flag("-Wl,-all_load" is_all_load_supported)
        if(NOT is_all_load_supported)
          message(WARNING "No known linker flag to force library linkage")
          message(WARNING "test_shared_library_preloaded might fail in runtime")
        else()
          target_link_libraries(test_shared_library_preloaded "-Wl,-all_load")
        endif()
      else()
        target_link_libraries(test_shared_library_preloaded "-Wl,--no-as-needed")
      endif()
    endif()
    target_link_libraries(test_shared_library_preloaded dummy_shared_library_preloaded)
    target_link_libraries(test_shared_library_preloaded ${PROJECT_NAME} mimick)
  endif()

  rcutils_custom_add_gtest(test_time
    test/test_time.cpp
    ENV ${memory_tools_test_env_vars})
  if(TARGET test_time)
    target_link_libraries(test_time ${PROJECT_NAME} mimick osrf_testing_tools_cpp::memory_tools)
  endif()

  rcutils_custom_add_gtest(test_snprintf
    test/test_snprintf.cpp
  )
  if(TARGET test_snprintf)
    target_link_libraries(test_snprintf ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_strcasecmp
    test/test_strcasecmp.cpp
  )
  if(TARGET test_strcasecmp)
    target_link_libraries(test_strcasecmp ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_uint8_array
    test/test_uint8_array.cpp
  )
  if(TARGET test_uint8_array)
    target_link_libraries(test_uint8_array ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_array_list
    test/test_array_list.cpp
  )
  if(TARGET test_array_list)
    target_link_libraries(test_array_list ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_hash_map
    test/test_hash_map.cpp
  )
  if(TARGET test_hash_map)
    target_link_libraries(test_hash_map ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_cmdline_parser
    test/test_cmdline_parser.cpp
  )
  if(TARGET test_cmdline_parser)
    target_link_libraries(test_cmdline_parser ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_process
    test/test_process.cpp
  )
  if(TARGET test_process)
    target_link_libraries(test_process ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_logging_custom_env test/test_logging_custom_env.cpp
    ENV
      RCUTILS_CONSOLE_OUTPUT_FORMAT=
        "[{severity}] [{time},{time_as_nanoseconds}] [{name},{function_name},{file_name}]: {line_number}-{message}"
      RCUTILS_CONSOLE_STDOUT_LINE_BUFFERED=1
      RCUTILS_LOGGING_BUFFERED_STREAM=1
      RCUTILS_LOGGING_USE_STDOUT=1
      RCUTILS_COLORIZED_OUTPUT=1
  )
  if(TARGET test_logging_custom_env)
    target_link_libraries(test_logging_custom_env ${PROJECT_NAME} osrf_testing_tools_cpp::memory_tools mimick)
  endif()

  # RCUTILS_LOGGING_MAX_OUTPUT_FORMAT_LEN is defined as 2048, truncation should occur
  foreach(i RANGE 0 100)
    set(_output_format
      "${_output_format} [{severity}] [{time},{time_as_nanoseconds}] [{name},{function_name},{file_name}]: {line_number}-{message}")
  endforeach(i)
  rcutils_custom_add_gtest(test_logging_custom_env2 test/test_logging_custom_env.cpp
    ENV
      RCUTILS_CONSOLE_OUTPUT_FORMAT="${_output_format}"
      RCUTILS_CONSOLE_STDOUT_LINE_BUFFERED=0
      RCUTILS_LOGGING_BUFFERED_STREAM=0
      RCUTILS_LOGGING_USE_STDOUT=0
      RCUTILS_COLORIZED_OUTPUT=0
  )
  if(TARGET test_logging_custom_env2)
    target_link_libraries(test_logging_custom_env2 ${PROJECT_NAME} osrf_testing_tools_cpp::memory_tools mimick)
  endif()

  rcutils_custom_add_gtest(test_logging_bad_env test/test_logging_bad_env.cpp
    ENV
      RCUTILS_LOGGING_USE_STDOUT=42
  )
  if(TARGET test_logging_bad_env)
    target_link_libraries(test_logging_bad_env ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_logging_bad_env2 test/test_logging_bad_env.cpp
    ENV
      RCUTILS_COLORIZED_OUTPUT=42
  )
  if(TARGET test_logging_bad_env2)
    target_link_libraries(test_logging_bad_env2 ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_logging_bad_env3 test/test_logging_bad_env.cpp
    ENV
      RCUTILS_LOGGING_BUFFERED_STREAM=42
  )
  if(TARGET test_logging_bad_env3)
    target_link_libraries(test_logging_bad_env3 ${PROJECT_NAME})
  endif()

  rcutils_custom_add_gtest(test_logging_enable_for
    test/test_logging_enable_for.cpp
  )
  if(TARGET test_logging_enable_for)
    target_link_libraries(test_logging_enable_for ${PROJECT_NAME} osrf_testing_tools_cpp::memory_tools)
  endif()

  rcutils_custom_add_gtest(test_logging_console_output_handler
    test/test_logging_console_output_handler.cpp
  )
  if(TARGET test_logging_console_output_handler)
    target_link_libraries(test_logging_console_output_handler ${PROJECT_NAME} osrf_testing_tools_cpp::memory_tools)
  endif()

  rcutils_custom_add_gtest(test_macros
    test/test_macros.cpp
  )

  add_performance_test(benchmark_logging test/benchmark/benchmark_logging.cpp)
  if(TARGET benchmark_logging)
    target_link_libraries(benchmark_logging ${PROJECT_NAME})
  endif()

  add_performance_test(benchmark_err_handle test/benchmark/benchmark_error_handling.cpp)
  if(TARGET benchmark_err_handle)
    target_link_libraries(benchmark_err_handle ${PROJECT_NAME})
  endif()

  if(TARGET test_macros)
    target_link_libraries(test_macros ${PROJECT_NAME})
  endif()
endif()

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")

if(NOT RCUTILS_NO_FILESYSTEM)
  ament_export_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
endif()

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME})

ament_export_dependencies(ament_cmake)

ament_package()

install(
  DIRECTORY include/ ${CMAKE_CURRENT_BINARY_DIR}/include/
  DESTINATION include/${PROJECT_NAME})
