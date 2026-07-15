# Copyright 2014-2015 Open Source Robotics Foundation, Inc.
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

if(WIN32)
    set(_ext ".bat")
else()
    set(_ext ".sh")
endif()
ament_environment_hooks(
  "${ament_cmake_core_DIR}/environment_hooks/environment/ament_prefix_path${_ext}"
  "${ament_cmake_core_DIR}/environment_hooks/environment/path${_ext}"
)

if(AMENT_CMAKE_ENVIRONMENT_PACKAGE_GENERATION)
  ament_generate_package_environment()
endif()
