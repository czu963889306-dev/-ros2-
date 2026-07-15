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

#
# Register a CMake script for execution at uninstall time.
#
# :param ARGN: the list of CMake code lines
# :type ARGN: list of strings
# :param COMMENTS: an optional list of comments
# :type COMMENTS: list of strings
#
function(ament_cmake_uninstall_target_append_uninstall_code)
  cmake_parse_arguments(ARG "" "" "COMMENTS" ${ARGN})

  # append code to uninstall script
  if(ARG_COMMENTS)
    file(APPEND "${AMENT_CMAKE_UNINSTALL_TARGET_UNINSTALL_SCRIPT}"
      "\n# ${ARG_COMMENTS}\n")
  endif()
  foreach(code ${ARG_UNPARSED_ARGUMENTS})
    file(APPEND "${AMENT_CMAKE_UNINSTALL_TARGET_UNINSTALL_SCRIPT}" "${code}\n")
  endforeach()
endfunction()
