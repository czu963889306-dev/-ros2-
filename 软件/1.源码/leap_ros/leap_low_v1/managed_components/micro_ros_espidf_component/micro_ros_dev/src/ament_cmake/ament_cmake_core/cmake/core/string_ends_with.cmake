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
# Check if a string ends with a specific suffix.
#
# :param str: the string
# :type str: string
# :param suffix: the suffix
# :type suffix: string
# :param var: the output variable name
# :type var: bool
#
function(string_ends_with str suffix var)
  string(LENGTH "${str}" str_length)
  string(LENGTH "${suffix}" suffix_length)
  set(value FALSE)
  if(NOT ${str_length} LESS ${suffix_length})
    math(EXPR str_offset "${str_length} - ${suffix_length}")
    string(SUBSTRING "${str}" ${str_offset} ${suffix_length} str_suffix)
    if(str_suffix STREQUAL suffix)
      set(value TRUE)
    endif()
  endif()
  set(${var} ${value} PARENT_SCOPE)
endfunction()
