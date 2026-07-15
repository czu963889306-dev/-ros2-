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

# copied from ament_cmake_core/ament_cmake_environment_hooks-extras.cmake

option(AMENT_CMAKE_ENVIRONMENT_PACKAGE_GENERATION
  "Generate environment files in the package share folder" ON)

# For each known environment hook the parameters are described
# to perform the same behavior as the shell specific scripts.
# Each CMake variable starts with `AMENT_CMAKE_ENVIRONMENT_HOOKS_DESC_`
# followed by the basename of the environment hook.
# Each of these variables contains a list of items:
# The first item identifies the kind of environment hook:
# - `source`: source another script.
#   This type of environment hook has a second item which contains the path of
#   the script to be sourced.
#   If the path is relative the absolute path of the current prefix is
#   prepended.
# - `prepend-non-duplicate`: prepend the value to an environment variable if it
#   is not already in it.
#   This type of environment hook has a two more item which contain the name of
#   the environment variable  and the path to be prepended (again absolute, or
#   relative to the current prefix.
# - `prepend-non-duplicate-if-exists`: prepend the value to an environment
#   variable if it is not already in it and if the path exists.
#   This type of environment hook has the same items as the previous one.
set(
  AMENT_CMAKE_ENVIRONMENT_HOOKS_DESC_ament_prefix_path
  "prepend-non-duplicate;AMENT_PREFIX_PATH;")
if(NOT WIN32)
  if(NOT APPLE)
    set(
      AMENT_CMAKE_ENVIRONMENT_HOOKS_DESC_library_path
      "prepend-non-duplicate;LD_LIBRARY_PATH;lib")
  else()
    set(
      AMENT_CMAKE_ENVIRONMENT_HOOKS_DESC_library_path
      "prepend-non-duplicate;DYLD_LIBRARY_PATH;lib")
  endif()
endif()
set(
  AMENT_CMAKE_ENVIRONMENT_HOOKS_DESC_path
  "prepend-non-duplicate-if-exists;PATH;bin")
set(
  AMENT_CMAKE_ENVIRONMENT_HOOKS_DESC_pkg_config_path
  "prepend-non-duplicate;PKG_CONFIG_PATH;lib/pkgconfig")

include(
  "${ament_cmake_core_DIR}/environment_hooks/ament_environment_hooks.cmake")
include(
  "${ament_cmake_core_DIR}/environment_hooks/ament_generate_package_environment.cmake")

ament_register_extension("ament_package" "ament_cmake_core"
  "environment_hooks/ament_cmake_environment_hooks_package_hook.cmake")
