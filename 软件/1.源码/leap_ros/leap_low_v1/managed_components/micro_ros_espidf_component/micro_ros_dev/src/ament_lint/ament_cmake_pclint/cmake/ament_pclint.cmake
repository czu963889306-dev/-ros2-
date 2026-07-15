# Copyright 2014-2015 Open Source Robotics Foundation, Inc.
# Copyright 2017-2018 Apex.AI, Inc.
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

# Add a test to perform static code analysis with PC-lint.
#
# :param TESTNAME: the name of the test, default: "pclint"
# :type TESTNAME: string
# :param LANGUAGE: force analysis to a specific language. Either "c" or "cpp"
# :type LANGUAGE: string
# :param LNT_FILE: Full-path of a custom lnt file.
# :type LNT_FILE: string
# :param ARGN: the files or directories to check
# :type ARGN: list of strings
#
# @public
function(ament_pclint)
  cmake_parse_arguments(ARG
                        ""
                        "LANGUAGE;TESTNAME;LNT_FILE"
                        "INCLUDE_DIRS;COMPILE_DEFS"
                        ${ARGN})
  if(NOT ARG_TESTNAME)
    set(ARG_TESTNAME "pclint")
  endif()

  find_program(ament_pclint_BIN NAMES "ament_pclint")
  if(NOT ament_pclint_BIN)
    message(FATAL_ERROR "ament_pclint() could not find program 'ament_pclint'")
  endif()

  set(cmd "${ament_pclint_BIN}" ${ARG_UNPARSED_ARGUMENTS})

  get_property(INCLUDED_DIRS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

  if(INCLUDED_DIRS)
    list(APPEND cmd "--include-directories" ${INCLUDED_DIRS})
    set(_INCLUDE_DIRS_FLAG_SET "Found")
  endif()

  # Include 'include' folders in AMENT_PREFIX_PATH
  if(ARG_INCLUDE_DIRS)
    if(NOT _INCLUDE_DIRS_FLAG_SET)
      list(APPEND cmd "--include-directories")
      set(_INCLUDED_DIRS_FLAG_SET "Found")
    endif()
    foreach(path ${ARG_INCLUDE_DIRS})
      list(APPEND cmd "${path}/include")
    endforeach()
  endif()

  get_directory_property(COMPILE_DEFS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_DEFINITIONS)

  if(COMPILE_DEFS)
    list(APPEND cmd "--compiler-definitions" "${COMPILE_DEFS}")
    set(_COMPILE_DEFS_FLAG_SET "Found")
  endif()

  # Add user-specificied compiler-definitions
  if(ARG_COMPILE_DEFS)
    if(NOT _COMPILE_DEFS_FLAG_SET)
      list(APPEND cmd "--compiler-definitions")
      set(_COMPILE_DEFS_FLAG_SET "Found")
    endif()
    foreach(def ${ARG_COMPILE_DEFS})
      list(APPEND cmd "${def}")
    endforeach()
  endif()

  if(ARG_LANGUAGE)
    list(APPEND cmd "--language" "${ARG_LANGUAGE}")
  endif()

  if(ARG_LNT_FILE)
    list(APPEND cmd "--pclint-config-file" ${ARG_LNT_FILE})
  endif()

  set(result_file "${AMENT_TEST_RESULTS_DIR}/${PROJECT_NAME}/${ARG_TESTNAME}.xunit.xml")

  list(APPEND cmd "--xunit-file" "${result_file}")

  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/ament_pclint")

  set(pclint_exec "pclp64")  # Windows executable name
  if(APPLE)
      set(pclint_exec "${pclint_exec}_osx")
  elseif(UNIX)  # i.e. UNIX AND NOT APPLE
      set(pclint_exec "${pclint_exec}_linux")
  endif()
  find_program(pclint_BIN NAMES "${pclint_exec}")

  if(pclint_BIN)
    ament_add_test(
      "${ARG_TESTNAME}"
      COMMAND ${cmd}
      OUTPUT_FILE "${CMAKE_BINARY_DIR}/ament_pclint/${ARG_TESTNAME}.txt"
      RESULT_FILE "${result_file}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      TIMEOUT 120)
    set_tests_properties(
      "${ARG_TESTNAME}"
      PROPERTIES
      LABELS "pclint;linter"
    )
  else()
    message(WARNING "WARNING: ${pclint_exec} not found, skipping pclint test creation")
  endif()
endfunction()
