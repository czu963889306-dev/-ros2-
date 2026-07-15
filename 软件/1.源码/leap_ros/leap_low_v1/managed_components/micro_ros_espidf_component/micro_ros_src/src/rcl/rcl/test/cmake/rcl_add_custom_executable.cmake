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

if(rcl_add_custom_executable_INCLUDED)
  return()
endif()
set(rcl_add_custom_executable_INCLUDED TRUE)

macro(rcl_add_custom_executable target)
  cmake_parse_arguments(_ARG
    "TRACE"
    ""
    "SRCS;INCLUDE_DIRS;LIBRARIES;AMENT_DEPENDENCIES"
    ${ARGN})
  if(_ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "rcl_add_custom_executable() called with unused arguments:"
      "${_ARG_UNPARSED_ARGUMENTS}")
  endif()

  add_executable(${target} ${_ARG_SRCS})

  if(_ARG_TRACE)
    message(STATUS "rcl_add_custom_executable() Target '${target}':")
  endif()
  # Add extra include directories, if any.
  if(_ARG_INCLUDE_DIRS)
    if(_ARG_TRACE)
      message(STATUS "  rcl_add_custom_executable() INCLUDE_DIRS: ${_ARG_INCLUDE_DIRS}")
    endif()
    target_include_directories(${target} PUBLIC ${_ARG_INCLUDE_DIRS})
  endif()
  # Add extra link libraries, if any.
  if(_ARG_LIBRARIES)
    if(_ARG_TRACE)
      message(STATUS "  rcl_add_custom_executable() LIBRARIES: ${_ARG_LIBRARIES}")
    endif()
    target_link_libraries(${target} ${_ARG_LIBRARIES})
  endif()
  # Add extra ament dependencies, if any.
  if(_ARG_AMENT_DEPENDENCIES)
    if(_ARG_TRACE)
      message(STATUS "  rcl_add_custom_executable() AMENT_DEPENDENCIES: ${_ARG_AMENT_DEPENDENCIES}")
    endif()
    ament_target_dependencies(${target} ${_ARG_AMENT_DEPENDENCIES})
  endif()
  target_compile_definitions(${target}
    PUBLIC "RMW_IMPLEMENTATION=${rmw_implementation}")
endmacro()
