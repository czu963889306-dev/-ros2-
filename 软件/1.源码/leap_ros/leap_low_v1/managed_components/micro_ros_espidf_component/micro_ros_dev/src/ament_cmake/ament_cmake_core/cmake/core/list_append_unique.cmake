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
# Append elements to a list if they are not already in the list.
#
# :param list: the list
# :type list: list variable
# :param ARGN: the elements
# :type ARGN: list of strings
#
# .. note:: Using CMake's ``list(APPEND ..)`` and
#   ``list(REMOVE_DUPLICATES ..)`` is not sufficient since its
#   implementation uses a set internally which makes the operation
#   unstable.
#
function(list_append_unique list)
  foreach(element ${ARGN})
    if(NOT element IN_LIST ${list})
      list(APPEND ${list} "${element}")
    endif()
  endforeach()
  set(${list} ${${list}} PARENT_SCOPE)
endfunction()
