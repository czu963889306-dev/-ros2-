# Copyright 2020 Open Source Robotics Foundation, Inc.
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

#
# Export typesupport targets to downstream packages.
#
# :param library_suffix: the suffix of the library
# :type library_suffix: string
# :param ARGN: a list of targets.
#   Each element must be an exported CMake library target.
# :type ARGN: list of strings
#
# @public
#
macro(rosidl_export_typesupport_targets library_suffix)
  if(_${PROJECT_NAME}_AMENT_PACKAGE)
    message(FATAL_ERROR
      "rosidl_export_typesupport_targets() must be called before "
      "ament_package()")
  endif()

  if(${ARGC} GREATER 0)
    _rosidl_cmake_export_typesupport_targets_register_package_hook()
    # loop over targets
    foreach(_target ${ARGN})
      if(NOT TARGET "${_target}")
        message(FATAL_ERROR
          "rosidl_export_typesupport_targets() must be called with targets")
      endif()

      get_target_property(_is_imported "${_target}" IMPORTED)
      if(_is_imported)
        message(FATAL_ERROR
          "rosidl_export_typesupport_targets() must be called with "
          "not-imported targets")
      endif()

      list(APPEND
        _ROSIDL_CMAKE_EXPORT_TYPESUPPORT_TARGETS
        "${library_suffix}:${_target}"
      )
    endforeach()
  endif()
endmacro()
