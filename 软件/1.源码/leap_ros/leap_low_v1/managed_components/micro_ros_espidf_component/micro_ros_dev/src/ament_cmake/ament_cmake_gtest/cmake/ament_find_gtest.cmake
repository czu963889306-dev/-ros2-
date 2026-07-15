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
# Find gtest.
#
# Set the variables ``GTEST_FOUND``, ``GTEST_INCLUDE_DIRS``,
# ``GTEST_LIBRARIES``, ``GTEST_MAIN_LIBRARIES`` and
# ``GTEST_BOTH_LIBRARIES``.
#
# Note: you should always link against ``GTEST_LIBRARIES`` and optionally
# additionally against ``GTEST_MAIN_LIBRARIES``.
#
# @public
#
macro(ament_find_gtest)
  set(_ARGN "${ARGN}")
  if(_ARGN)
    message(FATAL_ERROR
      "ament_find_gtest() called with unused arguments: ${_ARGN}")
  endif()
  _ament_cmake_gtest_find_gtest()
endmacro()
