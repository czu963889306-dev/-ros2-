# Copyright 2026 Open Source Robotics Foundation, Inc.
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

# Generate and register a post-extra file that creates the aggregate
# interface target ${PROJECT_NAME}::${PROJECT_NAME} for downstream packages.
# This uses CONFIG_EXTRAS_POST so it runs after ament_cmake_export_targets
# has populated ${PROJECT_NAME}_TARGETS.
set(_generated_extra_file
  "${CMAKE_CURRENT_BINARY_DIR}/rosidl_cmake/rosidl_cmake_aggregate_target-extras.cmake")
configure_file(
  "${rosidl_cmake_DIR}/rosidl_cmake_aggregate_target-extras.cmake.in"
  "${_generated_extra_file}"
  @ONLY
)
list(APPEND ${PROJECT_NAME}_CONFIG_EXTRAS_POST "${_generated_extra_file}")
