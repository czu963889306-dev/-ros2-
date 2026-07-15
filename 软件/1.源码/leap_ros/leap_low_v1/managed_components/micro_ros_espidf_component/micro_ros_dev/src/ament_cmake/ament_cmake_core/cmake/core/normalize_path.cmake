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
# Normalize a path by collapsing redundant parts and up-level references.
#
# This may change the meaning of a path that contains symbolic links.
#
# :param var: the output variable name
# :type var: string
# :param path: the path
# :type path: string
#
function(normalize_path var path)
  list(LENGTH path length)
  if(NOT length EQUAL 1)
    message(FATAL_ERROR "normalize_path() the path '${path}' must neither be empty nor contain semicolons")
  endif()
  # convert to list
  file(TO_CMAKE_PATH "${path}" path)
  string(REPLACE "/" ";" parts "${path}")

  _normalize_path__collapse_redundant(parts "${parts}")
  _normalize_path__collapse_uplevel_reference(parts "${parts}")

  # check if path has completely collapsed
  if(parts STREQUAL "")
    set(parts ".")
  endif()

  # convert back to string
  string(REPLACE ";" "/" normalized "${parts}")

  set(${var} "${normalized}" PARENT_SCOPE)
endfunction()


function(_normalize_path__collapse_redundant var)
  set(parts "${ARGN}")
  set(index 1)  # index 0 is empty for absolute paths
  while(TRUE)
    list(LENGTH parts length)
    if(NOT index LESS length)
      break()
    endif()
    list(GET parts ${index} part)
    # remove empty parts as well as current directory references
    if(part STREQUAL "" OR part STREQUAL ".")
      list(REMOVE_AT parts ${index})
    else()
      math(EXPR index "${index} + 1")
    endif()
  endwhile()
  set(${var} "${parts}" PARENT_SCOPE)
endfunction()


function(_normalize_path__collapse_uplevel_reference var)
  set(parts "${ARGN}")
  set(index 0)
  while(TRUE)
    list(LENGTH parts length)
    if(NOT index LESS length)
      break()
    endif()

    # get previous element
    set(previous_index ${index})
    list(GET parts ${previous_index} previous)
    math(EXPR index "${index} + 1")
    if(NOT index LESS length)
      break()
    endif()
    # get current element
    list(GET parts ${index} current)

    if(current STREQUAL ".." AND NOT previous STREQUAL "..")
      # collapse the '..'
      list(REMOVE_AT parts ${index})
      set(index ${previous_index})
      if(previous STREQUAL "")
        # only collapse the '..' when starting with '/../'
        if(previous_index GREATER 0)
          string(REPLACE ";" "/" path "${parts}")
          message(FATAL_ERROR "_normalize_path__collapse_uplevel_reference() the path '${path}' must not contain redundant separators")
        endif()
      else()
        # collapse the '..' as well as the part before
        list(REMOVE_AT parts ${previous_index})
        if(previous_index GREATER 0)
          math(EXPR index "${previous_index} - 1")
        else()
          math(EXPR index 0)
        endif()
      endif()
    endif()
  endwhile()
  set(${var} "${parts}" PARENT_SCOPE)
endfunction()
