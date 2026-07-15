# Copyright 2023 PAL Robotics S.L.
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
# Add a gmock with all found test dependencies.
#
# Call add_gmock(target ARGN), link it against the gmock libraries
# and all found test dependencies.
#
# If gmock is not available the specified target is not being created and
# therefore the target existence should be checked before being used.
#
# :param target: the target name which will also be used as the test name
# :type target: string
# :param ARGN: the list of source files and parameters
# :type ARGN: list of strings
#
# @public
#
macro(ament_auto_add_gmock target)
  cmake_parse_arguments(_ARG
    "SKIP_LINKING_MAIN_LIBRARIES;SKIP_TEST"
    "RUNNER;TIMEOUT;WORKING_DIRECTORY"
    "APPEND_ENV;APPEND_LIBRARY_DIRS;ENV"
    ${ARGN})
  if(NOT _ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "ament_auto_add_gmock() must be invoked with at least one source file")
  endif()

  find_package(ament_cmake_gmock QUIET REQUIRED)

  # add gmock
  ament_add_gmock("${target}" ${ARGN})

  # add include directory of this package if it exists
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_include_directories("${target}" PUBLIC
      "${CMAKE_CURRENT_SOURCE_DIR}/include")
  endif()

  # link against other libraries of this package
  if(NOT ${PROJECT_NAME}_LIBRARIES STREQUAL "")
    target_link_libraries("${target}" ${${PROJECT_NAME}_LIBRARIES})
  endif()

  # add exported information from found dependencies
  ament_target_dependencies(${target}
    ${${PROJECT_NAME}_FOUND_BUILD_DEPENDS}
    ${${PROJECT_NAME}_FOUND_TEST_DEPENDS}
  )
endmacro()
