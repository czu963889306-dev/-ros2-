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

#
# Find gmock.
#
# Set the variables ``GMOCK_FOUND``, ``GMOCK_INCLUDE_DIRS``,
# ``GMOCK_LIBRARIES`` and ``GMOCK_MAIN_LIBRARIES``.
#
# Note: you should only link against one of the variables, either
# ``GMOCK_LIBRARIES`` or ``GMOCK_MAIN_LIBRARIES``
#
# @public
#
macro(ament_find_gmock)
  set(_ARGN "${ARGN}")
  if(_ARGN)
    message(FATAL_ERROR
      "ament_find_gmock() called with unused arguments: ${_ARGN}")
  endif()
  _ament_cmake_gmock_find_gmock()
endmacro()
