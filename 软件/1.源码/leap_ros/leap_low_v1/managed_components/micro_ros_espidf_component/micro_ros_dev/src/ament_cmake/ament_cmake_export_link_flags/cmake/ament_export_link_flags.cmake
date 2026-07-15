# Copyright 2016 Open Source Robotics Foundation, Inc.
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
# Export link flags to downstream packages.
#
# Each package name must be find_package()-able with the exact same case.
# Additionally the exported variables must have a prefix with the same case
# and the suffixes must be INCLUDE_DIRS and LIBRARIES.
#
# :param ARGN: a list of link flags
# :type ARGN: list of strings
#
# @public
#
macro(ament_export_link_flags)
  if(_${PROJECT_NAME}_AMENT_PACKAGE)
    message(FATAL_ERROR
      "ament_export_link_flags() must be called before ament_package()")
  endif()

  if(${ARGC} GREATER 0)
    _ament_cmake_export_link_flags_register_package_hook()
    foreach(_arg ${ARGN})
      list(APPEND _AMENT_CMAKE_EXPORT_LINK_FLAGS "${_arg}")
    endforeach()
  endif()
endmacro()
