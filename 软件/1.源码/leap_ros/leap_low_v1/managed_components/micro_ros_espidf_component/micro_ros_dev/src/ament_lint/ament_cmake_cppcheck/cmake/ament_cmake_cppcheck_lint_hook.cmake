# Copyright 2015 Open Source Robotics Foundation, Inc.
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

find_package(ament_cmake_core REQUIRED)

file(GLOB_RECURSE _source_files FOLLOW_SYMLINKS
  "*.c"
  "*.cc"
  "*.cpp"
  "*.cxx"
  "*.h"
  "*.hh"
  "*.hpp"
  "*.hxx"
)
if(_source_files)
  message(STATUS "Added test 'cppcheck' to perform static code analysis on C / C++ code")

  # Get include paths for added targets
  set(_all_include_dirs "")
  if(DEFINED ament_cmake_cppcheck_ADDITIONAL_INCLUDE_DIRS)
    list(APPEND _all_include_dirs ${ament_cmake_cppcheck_ADDITIONAL_INCLUDE_DIRS})
  endif()

  # Forces cppcheck to consider ament_cmake_cppcheck_LANGUAGE as the given language if defined
  set(_language "")
  if(DEFINED ament_cmake_cppcheck_LANGUAGE)
    set(_language LANGUAGE ${ament_cmake_cppcheck_LANGUAGE})
    message(STATUS "Configured cppcheck language: ${ament_cmake_cppcheck_LANGUAGE}")
  endif()

  # Get exclude paths for added targets
  set(_all_exclude "")
  if(DEFINED ament_cmake_cppcheck_ADDITIONAL_EXCLUDE)
    list(APPEND _all_exclude ${ament_cmake_cppcheck_ADDITIONAL_EXCLUDE})
  endif()

  if(DEFINED AMENT_LINT_AUTO_FILE_EXCLUDE)
    list(APPEND _all_exclude ${AMENT_LINT_AUTO_FILE_EXCLUDE})
  endif()

  # BUILDSYSTEM_TARGETS only supported in CMake >= 3.7
  if(NOT CMAKE_VERSION VERSION_LESS "3.7.0")
    get_directory_property(_build_targets DIRECTORY ${PROJECT_SOURCE_DIR} BUILDSYSTEM_TARGETS)
    foreach(_target ${_build_targets})
      # Include directories property is different for INTERFACE libraries
      get_target_property(_target_type ${_target} TYPE)
      if(${_target_type} STREQUAL "INTERFACE_LIBRARY")
        get_target_property(_include_dirs ${_target} INTERFACE_INCLUDE_DIRECTORIES)
      else()
        get_target_property(_include_dirs ${_target} INCLUDE_DIRECTORIES)
      endif()

      # Only append include directories that are from the package being tested
      # This accomplishes two things:
      #     1. Reduces execution time (less include directories to search)
      #     2. cppcheck will not check for errors in external packages
      foreach(_include_dir ${_include_dirs})
        # TODO(jacobperron): Escape special regex characters in PROJECT_SOURCE_DIR
        #                    Related CMake feature request: https://gitlab.kitware.com/cmake/cmake/issues/18409
        # Check if include directory is a subdirectory of the source directory
        string(REGEX MATCH "^${PROJECT_SOURCE_DIR}/.*" _is_subdirectory ${_include_dir})
        # Check if include directory is part of a generator expression (e.g. $<BUILD_INTERFACE:...>)
        string(REGEX MATCH "^\\$<.*:${PROJECT_SOURCE_DIR}/.*>$" _is_genexp_subdirectory "${_include_dir}")
        if(_is_subdirectory OR _is_genexp_subdirectory)
          list_append_unique(_all_include_dirs ${_include_dir})
        endif()
      endforeach()
    endforeach()
  endif()

  message(STATUS "Configured cppcheck include dirs: ${_all_include_dirs}")
  message(
    STATUS "Configured cppcheck exclude dirs and/or files: ${_all_exclude}"
  )
  ament_cppcheck(
    ${_language} INCLUDE_DIRS ${_all_include_dirs} EXCLUDE ${_all_exclude}
  )
endif()
