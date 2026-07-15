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

# copied from ament_lint_auto/ament_lint_auto-extras.cmake

if(_${PROJECT_NAME}_AMENT_PACKAGE)
  message(FATAL_ERROR "find_package(ament_lint_auto) must be called before ament_package()")
endif()

find_package(ament_cmake_core QUIET REQUIRED)
find_package(ament_cmake_test QUIET REQUIRED)

include(
  "${ament_lint_auto_DIR}/ament_lint_auto_find_test_dependencies.cmake")

ament_register_extension("ament_package" "ament_lint_auto"
  "ament_lint_auto_package_hook.cmake")
