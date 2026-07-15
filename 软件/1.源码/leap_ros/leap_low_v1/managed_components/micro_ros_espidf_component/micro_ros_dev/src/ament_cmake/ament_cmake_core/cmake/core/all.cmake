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

# prevent multiple inclusion
if(DEFINED _AMENT_CMAKE_CORE_INCLUDED)
  message(FATAL_ERROR "ament_cmake_core/cmake/core/all.cmake included "
    "multiple times")
endif()
set(_AMENT_CMAKE_CORE_INCLUDED TRUE)

if(NOT DEFINED ament_cmake_core_DIR)
  message(FATAL_ERROR "ament_cmake_core_DIR is not set")
endif()

# set RPATH to ON for OS X
if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
endif()

# the following operations must be performed inside a project context
if(NOT PROJECT_NAME)
  project(ament_cmake_internal NONE)
endif()

# use BUILD_TESTING to avoid warnings about not using it
if(DEFINED BUILD_TESTING AND BUILD_TESTING)
endif()

# various functions / macros
foreach(filename
  "ament_execute_extensions"
  "ament_package"
  "ament_package_xml"
  "ament_register_extension"
  "assert_file_exists"
  "get_executable_path"
  "list_append_unique"
  "normalize_path"
  "python"
  "stamp"
  "string_ends_with"
)
  include(${ament_cmake_core_DIR}/core/${filename}.cmake)
endforeach()

# ensure that no current package name is set
unset(_AMENT_PACKAGE_NAME)
