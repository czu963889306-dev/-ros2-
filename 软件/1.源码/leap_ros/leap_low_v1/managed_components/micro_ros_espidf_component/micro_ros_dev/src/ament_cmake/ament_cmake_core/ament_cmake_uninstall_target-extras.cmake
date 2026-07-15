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

# copied from ament_cmake_core/ament_cmake_uninstall_target-extras.cmake

option(AMENT_CMAKE_UNINSTALL_TARGET
  "Generate an uninstall target to revert the effects of the install step" ON)

if(AMENT_CMAKE_UNINSTALL_TARGET)
  include(
    "${ament_cmake_core_DIR}/uninstall_target/ament_cmake_uninstall_target_append_uninstall_code.cmake")

  # create the install script from the template
  # ament_cmake_core/uninstall_target/ament_cmake_uninstall_target.cmake.in
  set(AMENT_CMAKE_UNINSTALL_TARGET_UNINSTALL_SCRIPT
    "${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_uninstall_target/ament_cmake_uninstall_target.cmake")
  configure_file(
    "${ament_cmake_core_DIR}/uninstall_target/ament_cmake_uninstall_target.cmake.in"
    "${AMENT_CMAKE_UNINSTALL_TARGET_UNINSTALL_SCRIPT}"
    @ONLY
  )

  if(NOT TARGET uninstall)
    add_custom_target(uninstall)
  endif()

  if(NOT TARGET ${PROJECT_NAME}_uninstall)
    # register uninstall target to run generated CMake script
    add_custom_target(${PROJECT_NAME}_uninstall
      COMMAND ${CMAKE_COMMAND} -P "${AMENT_CMAKE_UNINSTALL_TARGET_UNINSTALL_SCRIPT}")
    add_dependencies(uninstall ${PROJECT_NAME}_uninstall)
  endif()
endif()
