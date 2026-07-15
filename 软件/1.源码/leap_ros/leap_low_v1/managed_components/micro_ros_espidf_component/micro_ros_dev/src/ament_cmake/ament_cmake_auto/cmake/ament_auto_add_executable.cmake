# Copyright 2014 Open Source Robotics Foundation, Inc.
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
# Add an executable target.
#
# All arguments of the CMake function ``add_executable()`` can be
# used beside the custom arguments ``DIRECTORY`` and
# ``NO_TARGET_LINK_LIBRARIES``.
#
# :param target: the name of the executable target
# :type target: string
# :param DIRECTORY: the directory to recursively glob for source
#   files with the following extensions: c, cc, cpp, cxx
# :type DIRECTORY: string
# :param NO_TARGET_LINK_LIBRARIES: if set skip linking against
#   ``${PROJECT_NAME}_LIBRARIES``
# :type NO_TARGET_LINK_LIBRARIES: option
#
# Append the target to the ``${PROJECT_NAME}_EXECUTABLES`` variable.
#
# @public
#
macro(ament_auto_add_executable target)
  cmake_parse_arguments(ARG
    "WIN32;MACOSX_BUNDLE;EXCLUDE_FROM_ALL;NO_TARGET_LINK_LIBRARIES"
    "DIRECTORY"
    ""
    ${ARGN})
  if(NOT ARG_DIRECTORY AND NOT ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_auto_add_executable() called without any "
      "source files and without a DIRECTORY argument")
  endif()

  set(_source_files "")
  if(ARG_DIRECTORY)
    # glob all source files
    file(
      GLOB_RECURSE
      _source_files
      RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
      "${ARG_DIRECTORY}/*.c"
      "${ARG_DIRECTORY}/*.cc"
      "${ARG_DIRECTORY}/*.cpp"
      "${ARG_DIRECTORY}/*.cxx"
    )
    if(NOT _source_files)
      message(FATAL_ERROR "ament_auto_add_executable() no source files found "
        "in directory '${CMAKE_CURRENT_SOURCE_DIR}/${ARG_DIRECTORY}'")
    endif()
  endif()

  # parse again to "remove" custom arguments
  cmake_parse_arguments(ARG "NO_TARGET_LINK_LIBRARIES" "DIRECTORY" "" ${ARGN})
  add_executable("${target}" ${ARG_UNPARSED_ARGUMENTS} ${_source_files})

  # add include directory of this package if it exists
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include")
    target_include_directories("${target}" PUBLIC
      "${CMAKE_CURRENT_SOURCE_DIR}/include")
  endif()
  # link against other libraries of this package
  if(NOT ${PROJECT_NAME}_LIBRARIES STREQUAL "" AND
      NOT ARG_NO_TARGET_LINK_LIBRARIES)
    target_link_libraries("${target}" ${${PROJECT_NAME}_LIBRARIES})
  endif()

  # add exported information from found build dependencies
  ament_target_dependencies(${target} SYSTEM ${${PROJECT_NAME}_FOUND_BUILD_DEPENDS})

  list(APPEND ${PROJECT_NAME}_EXECUTABLES "${target}")
endmacro()
