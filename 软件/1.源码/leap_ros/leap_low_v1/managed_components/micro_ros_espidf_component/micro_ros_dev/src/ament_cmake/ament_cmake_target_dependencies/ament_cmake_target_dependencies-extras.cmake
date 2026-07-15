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

# copied from
# ament_cmake_target_dependencies/ament_cmake_target_dependencies-extras.cmake

find_package(ament_cmake_core QUIET REQUIRED)
find_package(ament_cmake_include_directories QUIET REQUIRED)
find_package(ament_cmake_libraries QUIET REQUIRED)

include(
  "${ament_cmake_target_dependencies_DIR}/ament_get_recursive_properties.cmake")
include(
  "${ament_cmake_target_dependencies_DIR}/ament_target_dependencies.cmake")
