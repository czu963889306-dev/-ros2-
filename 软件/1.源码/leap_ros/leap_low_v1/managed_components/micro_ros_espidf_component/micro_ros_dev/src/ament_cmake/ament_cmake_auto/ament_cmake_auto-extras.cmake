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

# copied from ament_cmake_auto/ament_cmake_auto-extras.cmake

find_package(ament_cmake QUIET REQUIRED)

include("${ament_cmake_auto_DIR}/ament_auto_add_executable.cmake")
include("${ament_cmake_auto_DIR}/ament_auto_add_gmock.cmake")
include("${ament_cmake_auto_DIR}/ament_auto_add_gtest.cmake")
include("${ament_cmake_auto_DIR}/ament_auto_add_library.cmake")
include("${ament_cmake_auto_DIR}/ament_auto_generate_code.cmake")
include("${ament_cmake_auto_DIR}/ament_auto_find_build_dependencies.cmake")
include("${ament_cmake_auto_DIR}/ament_auto_find_test_dependencies.cmake")
include("${ament_cmake_auto_DIR}/ament_auto_package.cmake")
