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
# Order include directories according to chain of prefixes.
#
# :param var: the output variable name
# :type var: string
# :param ARGN: a list of include directories.
# :type ARGN: list of strings
#
# @public
#
function(ament_include_directories_order var)
  set(prefixes "$ENV{AMENT_PREFIX_PATH}")
  if(NOT WIN32)
    string(REPLACE ":" ";" prefixes "${prefixes}")
  endif()

  # create list of empty slots, one per prefix and one for unknown prefixes
  list(LENGTH prefixes prefix_count)
  foreach(index RANGE ${prefix_count})
    set(slot_${index} "")
  endforeach()

  # append the include directories to the first matching prefix
  foreach(include_dir ${ARGN})
    string(LENGTH "${include_dir}" include_dir_length)

    set(index 0)
    while(TRUE)
      if(NOT ${index} LESS ${prefix_count})
        # no match
        break()
      endif()

      list(GET prefixes ${index} prefix)
      # exact match
      if(prefix STREQUAL include_dir)
        break()
      endif()

      string(LENGTH "${prefix}" prefix_length)
      if(${prefix_length} LESS ${include_dir_length})
        math(EXPR prefix_length_plus_one "${prefix_length} + 1")
        string(SUBSTRING "${include_dir}"
          0 ${prefix_length_plus_one} include_dir_prefix)
        # prefix match
        if("${prefix}/" STREQUAL "${include_dir_prefix}")
          break()
        endif()
      endif()

      math(EXPR index "${index} + 1")
    endwhile()
    list(APPEND slot_${index} "${include_dir}")
  endforeach()

  # join the slot lists
  set(ordered "")
  foreach(index RANGE ${prefix_count})
    list(APPEND ordered ${slot_${index}})
  endforeach()
  set(${var} "${ordered}" PARENT_SCOPE)
endfunction()
