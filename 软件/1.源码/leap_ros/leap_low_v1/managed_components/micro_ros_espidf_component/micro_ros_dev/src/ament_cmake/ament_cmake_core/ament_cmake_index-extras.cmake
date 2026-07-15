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

# copied from ament_core/ament_index-extras.cmake

include("${ament_cmake_core_DIR}/index/ament_index_get_prefix_path.cmake")
include("${ament_cmake_core_DIR}/index/ament_index_get_resource.cmake")
include("${ament_cmake_core_DIR}/index/ament_index_get_resources.cmake")
include("${ament_cmake_core_DIR}/index/ament_index_has_resource.cmake")
include("${ament_cmake_core_DIR}/index/ament_index_register_package.cmake")
include("${ament_cmake_core_DIR}/index/ament_index_register_resource.cmake")

ament_register_extension("ament_package" "ament_cmake_core"
  "index/ament_cmake_index_package_hook.cmake")
