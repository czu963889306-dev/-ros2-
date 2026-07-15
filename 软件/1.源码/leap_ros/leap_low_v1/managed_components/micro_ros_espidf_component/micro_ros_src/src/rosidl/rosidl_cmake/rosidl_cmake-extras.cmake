# Copyright 2014-2018 Open Source Robotics Foundation, Inc.
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

# copied from rosidl_cmake/rosidl_cmake-extras.cmake

# register ament_package() hook for definitions once
macro(_rosidl_cmake_register_package_hook)
  if(NOT DEFINED _ROSIDL_CMAKE_PACKAGE_HOOK_REGISTERED)
    set(_ROSIDL_CMAKE_PACKAGE_HOOK_REGISTERED TRUE)

    find_package(ament_cmake_core QUIET REQUIRED)
    ament_register_extension("ament_package" "rosidl_cmake"
      "rosidl_cmake_package_hook.cmake")

    find_package(ament_cmake_export_dependencies QUIET REQUIRED)

    _rosidl_cmake_aggregate_target_register_package_hook()
  endif()
endmacro()

# register ament_package() hook for the aggregate interface target once
macro(_rosidl_cmake_aggregate_target_register_package_hook)
  if(NOT DEFINED _ROSIDL_CMAKE_AGGREGATE_TARGET_PACKAGE_HOOK_REGISTERED)
    set(_ROSIDL_CMAKE_AGGREGATE_TARGET_PACKAGE_HOOK_REGISTERED TRUE)

    find_package(ament_cmake_core QUIET REQUIRED)
    ament_register_extension("ament_package" "rosidl_cmake"
      "rosidl_cmake_aggregate_target_package_hook.cmake")
  endif()
endmacro()

find_package(rosidl_adapter)  # not required, being used when available

include("${rosidl_cmake_DIR}/rosidl_generate_interfaces.cmake")
include("${rosidl_cmake_DIR}/rosidl_auto_generate_interfaces.cmake")
include("${rosidl_cmake_DIR}/rosidl_get_typesupport_target.cmake")
include("${rosidl_cmake_DIR}/rosidl_target_interfaces.cmake")
include("${rosidl_cmake_DIR}/rosidl_write_generator_arguments.cmake")
include("${rosidl_cmake_DIR}/string_camel_case_to_lower_case_underscore.cmake")

# register ament_package() hook for typesupport libraries once
macro(_rosidl_cmake_export_typesupport_libraries_register_package_hook)
  if(NOT DEFINED _ROSIDL_CMAKE_EXPORT_TYPESUPPORT_LIBRARIES_PACKAGE_HOOK_REGISTERED)
    set(_ROSIDL_CMAKE_EXPORT_TYPESUPPORT_LIBRARIES_PACKAGE_HOOK_REGISTERED TRUE)

    find_package(ament_cmake_core QUIET REQUIRED)
    ament_register_extension("ament_package" "rosidl_cmake"
      "rosidl_cmake_export_typesupport_libraries_package_hook.cmake")
  endif()
endmacro()
# register ament_package() hook for typesupport targets once
macro(_rosidl_cmake_export_typesupport_targets_register_package_hook)
  if(NOT DEFINED _ROSIDL_CMAKE_EXPORT_TYPESUPPORT_TARGETS_PACKAGE_HOOK_REGISTERED)
    set(_ROSIDL_CMAKE_EXPORT_TYPESUPPORT_TARGETS_PACKAGE_HOOK_REGISTERED TRUE)

    find_package(ament_cmake_core QUIET REQUIRED)
    ament_register_extension("ament_package" "rosidl_cmake"
      "rosidl_cmake_export_typesupport_targets_package_hook.cmake")
  endif()
endmacro()

include("${rosidl_cmake_DIR}/rosidl_export_typesupport_libraries.cmake")
include("${rosidl_cmake_DIR}/rosidl_export_typesupport_targets.cmake")
