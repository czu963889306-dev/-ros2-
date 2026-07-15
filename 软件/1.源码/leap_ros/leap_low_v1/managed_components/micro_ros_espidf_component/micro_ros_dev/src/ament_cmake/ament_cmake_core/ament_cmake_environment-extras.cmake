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

# copied from ament_cmake_core/ament_cmake_environment-extras.cmake

option(AMENT_CMAKE_ENVIRONMENT_GENERATION
  "Generate environment files in the CMAKE_INSTALL_PREFIX" OFF)
option(AMENT_CMAKE_ENVIRONMENT_PARENT_PREFIX_PATH_GENERATION
  "Generate marker file containing the parent prefix path" ON)

include("${ament_cmake_core_DIR}/environment/ament_generate_environment.cmake")

ament_register_extension("ament_package" "ament_cmake_core"
  "environment/ament_cmake_environment_package_hook.cmake")
