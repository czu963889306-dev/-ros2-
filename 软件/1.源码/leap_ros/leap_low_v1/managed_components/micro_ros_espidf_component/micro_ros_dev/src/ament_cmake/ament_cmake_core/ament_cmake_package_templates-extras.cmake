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

# extract information from ament_package.templates
if(NOT TARGET Python3::Interpreter)
  message(FATAL_ERROR
    "ament_cmake_package_templates: target 'Python3::Interpreter' must exist")
endif()

# stamp script to generate CMake code
set(_generator
  "${ament_cmake_core_DIR}/package_templates/templates_2_cmake.py")
stamp("${_generator}")

# invoke generator script
set(_generated_file
  "${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_package_templates/templates.cmake")
get_executable_path(_python_interpreter Python3::Interpreter CONFIGURE)
set(_cmd
  "${_python_interpreter}"
  "${_generator}"
  "${_generated_file}"
)
execute_process(
  COMMAND ${_cmd}
  RESULT_VARIABLE _res
)
if(NOT _res EQUAL 0)
  string(REPLACE ";" " " _cmd_str "${_cmd}")
  message(FATAL_ERROR
    "execute_process(${_cmd_str}) returned error code ${_res}")
endif()

# load extracted variables into cmake
# for each environment hook defined in `ament_package`
# (e.g. `library_path.bat|sh`) a CMake variable is defined starting with
# `ament_cmake_package_templates_ENVIRONMENT_HOOK_`
# (e.g. `ament_cmake_package_templates_ENVIRONMENT_HOOK_LIBRARY_PATH`)
include("${_generated_file}")
