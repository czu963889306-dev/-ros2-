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
# Generate setup files in the root of the install prefix.
#
# @public
#
function(ament_generate_environment)
  if(ARGN)
    message(FATAL_ERROR
      "ament_generate_environment() called with unused arguments: ${ARGN}")
  endif()

  if(NOT TARGET Python3::Interpreter)
    message(FATAL_ERROR
      "ament_generate_environment() target 'Python3::Interpreter' must exist")
  endif()

  # Default python used in local_setup.* scripts
  get_executable_path(ament_package_PYTHON_EXECUTABLE Python3::Interpreter CONFIGURE)

  # configure and install setup files
  foreach(file ${ament_cmake_package_templates_PREFIX_LEVEL})
    # check if the file is a template
    string_ends_with("${file}" ".in" is_template)
    if(is_template)
      # cut of .in extension
      string(LENGTH "${file}" length)
      math(EXPR offset "${length} - 3")
      string(SUBSTRING "${file}" 0 ${offset} name)
      # expand template
      get_filename_component(name "${name}" NAME)
      configure_file(
        "${file}"
        "${CMAKE_BINARY_DIR}/ament_cmake_environment/${name}"
        @ONLY
      )
      set(file "${CMAKE_BINARY_DIR}/ament_cmake_environment/${name}")
    endif()

    install(
      FILES "${file}"
      DESTINATION "${CMAKE_INSTALL_PREFIX}"
    )
  endforeach()
endfunction()
