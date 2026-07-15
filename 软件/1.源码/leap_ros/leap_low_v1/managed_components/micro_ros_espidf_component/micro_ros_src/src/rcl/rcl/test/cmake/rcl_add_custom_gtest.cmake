# Copyright 2016 Open Source Robotics Foundation, Inc.
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

if(rcl_add_custom_gtest_INCLUDED)
  return()
endif()
set(rcl_add_custom_gtest_INCLUDED TRUE)

#
# Custom macro for adding a gtest in rcl.
#
# It also takes some of the arguments of ament_add_gtest as well as
# INCLUDE_DIRS, LIBRARIES, and AMENT_DEPENDENCIES which are passed to
# target_include_directories(), target_link_libraries(), and
# ament_target_dependencies() respectively.
#
# :param target: the target name which will also be used as the test name
# :type target: string
# :param SRCS: list of source files used to create the gtest
# :type SRCS: list of strings
# :param ENV: list of env vars to set; listed as ``VAR=value``
# :type ENV: list of strings
# :param APPEND_ENV: list of env vars to append if already set, otherwise set;
#   listed as ``VAR=value``
# :type APPEND_ENV: list of strings
# :param APPEND_LIBRARY_DIRS: list of library dirs to append to the appropriate
#   OS specific env var, a la LD_LIBRARY_PATH
# :type APPEND_LIBRARY_DIRS: list of strings
# :param INCLUDE_DIRS: list of include directories to add to the target
# :type INCLUDE_DIRS: list of strings
# :param LIBRARIES: list of libraries to link to the target
# :type LIBRARIES: list of strings
# :param AMENT_DEPENDENCIES: list of depends to pass ament_target_dependencies
# :type AMENT_DEPENDENCIES: list of strings
#
# @public
#
macro(rcl_add_custom_gtest target)
  cmake_parse_arguments(_ARG
    "SKIP_TEST;TRACE"
    "TIMEOUT"
    "SRCS;ENV;APPEND_ENV;APPEND_LIBRARY_DIRS;INCLUDE_DIRS;LIBRARIES;AMENT_DEPENDENCIES"
    ${ARGN})
  if(_ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "rcl_add_custom_gtest() called with unused arguments: ${_ARG_UNPARSED_ARGUMENTS}")
  endif()
  if(_ARG_ENV)
    set(_ARG_ENV "ENV" ${_ARG_ENV})
  endif()
  if(_ARG_APPEND_ENV)
    set(_ARG_APPEND_ENV "APPEND_ENV" ${_ARG_APPEND_ENV})
  endif()
  if(_ARG_APPEND_LIBRARY_DIRS)
    set(_ARG_APPEND_LIBRARY_DIRS "APPEND_LIBRARY_DIRS" ${_ARG_APPEND_LIBRARY_DIRS})
  endif()
  if(_ARG_SKIP_TEST)
    set(_ARG_SKIP_TEST "SKIP_TEST")
  else()
    set(_ARG_SKIP_TEST "")
  endif()
  if(_ARG_TIMEOUT)
    set(_ARG_TIMEOUT "TIMEOUT" ${_ARG_TIMEOUT})
  endif()

  # Pass args along to ament_add_gtest().
  ament_add_gtest(${target} ${_ARG_SRCS} ${_ARG_ENV} ${_ARG_APPEND_ENV} ${_ARG_APPEND_LIBRARY_DIRS}
                  ${_ARG_SKIP_TEST} ${_ARG_TIMEOUT})
  # Check if the target was actually created.
  if(TARGET ${target})
    if(_ARG_TRACE)
      message(STATUS "rcl_add_custom_gtest() Target '${target}':")
    endif()
    # Add extra include directories, if any.
    if(_ARG_INCLUDE_DIRS)
      if(_ARG_TRACE)
        message(STATUS "  rcl_add_custom_gtest() INCLUDE_DIRS: ${_ARG_INCLUDE_DIRS}")
      endif()
      target_include_directories(${target} PUBLIC ${_ARG_INCLUDE_DIRS})
    endif()
    # Add extra link libraries, if any.
    if(_ARG_LIBRARIES)
      if(_ARG_TRACE)
        message(STATUS "  rcl_add_custom_gtest() LIBRARIES: ${_ARG_LIBRARIES}")
      endif()
      target_link_libraries(${target} ${_ARG_LIBRARIES})
    endif()
    # Add extra ament dependencies, if any.
    if(_ARG_AMENT_DEPENDENCIES)
      if(_ARG_TRACE)
        message(STATUS "  rcl_add_custom_gtest() AMENT_DEPENDENCIES: ${_ARG_AMENT_DEPENDENCIES}")
      endif()
      ament_target_dependencies(${target} ${_ARG_AMENT_DEPENDENCIES})
    endif()
    target_compile_definitions(${target}
      PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
  endif()
endmacro()
