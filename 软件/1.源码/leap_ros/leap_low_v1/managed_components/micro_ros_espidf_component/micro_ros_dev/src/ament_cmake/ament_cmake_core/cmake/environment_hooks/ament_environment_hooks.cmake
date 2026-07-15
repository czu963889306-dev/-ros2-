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
# Register environment hooks.
#
# Each file can either be a plain file (ending with a supported extensions)
# or a template which is expanded using configure_file() (ending in
# '.<ext>.in') with @ONLY.
#
# :param ARGN: a list of environment hook files
# :type ARGN: list of strings
#
# @public
#
function(ament_environment_hooks)
  if(_${PROJECT_NAME}_AMENT_GENERATE_PACKAGE_ENVIRONMENT)
    message(FATAL_ERROR "ament_environment_hooks() must be called before "
      "ament_generate_package_environment() (which is invoked by "
      "ament_package())")
  endif()

  foreach(hook ${ARGN})
    assert_file_exists("${hook}"
      "ament_environment_hooks() the passed hook file '${hook}' does not exist")
    stamp("${hook}")

    get_filename_component(hook_filename "${hook}" NAME)

    # check if the file is a template
    if(hook_filename MATCHES "^(.*)\\.in$")
      set(is_template TRUE)
      # cut off .in extension
      set(hook_filename "${CMAKE_MATCH_1}")
    else()
      set(is_template FALSE)
    endif()

    if(hook_filename MATCHES "^.*\\.([^\\.]+)$")
      # extract the extension
      set(hook_extension "${CMAKE_MATCH_1}")
    else()
      message(FATAL_ERROR "ament_environment_hooks() called with the hook "
        "'${hook}' which doesn't have a file extension")
    endif()

    if(is_template)
      # expand template
      configure_file(
        "${hook}"
        "${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_environment_hooks/${hook_filename}"
        @ONLY
      )
      set(hook
        "${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_environment_hooks/${hook_filename}")
    endif()

    # install hook file
    install(
      FILES "${hook}"
      DESTINATION "share/${PROJECT_NAME}/environment"
    )

    # remember all environment hooks
    # for generating the package environment files
    list(APPEND _AMENT_CMAKE_ENVIRONMENT_HOOKS_${hook_extension}
      "share/${PROJECT_NAME}/environment/${hook_filename}")
    set(_AMENT_CMAKE_ENVIRONMENT_HOOKS_${hook_extension}
      "${_AMENT_CMAKE_ENVIRONMENT_HOOKS_${hook_extension}}" PARENT_SCOPE)

    get_filename_component(hook_basename "${hook}" NAME_WE)
    if(DEFINED AMENT_CMAKE_ENVIRONMENT_HOOKS_DESC_${hook_basename})
      # write .dsv file containing the descriptor of the environment hook
      set(dsv_file "${CMAKE_BINARY_DIR}/ament_cmake_environment_hooks/${hook_basename}.dsv")
      file(WRITE "${dsv_file}" "${AMENT_CMAKE_ENVIRONMENT_HOOKS_DESC_${hook_basename}}\n")
      install(
        FILES "${dsv_file}"
        DESTINATION "share/${PROJECT_NAME}/environment"
      )
    endif()
  endforeach()
endfunction()
