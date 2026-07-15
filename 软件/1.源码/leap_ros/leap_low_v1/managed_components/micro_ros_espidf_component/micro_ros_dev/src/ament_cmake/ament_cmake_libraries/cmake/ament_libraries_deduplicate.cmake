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
# Deduplicate libraries.
#
# If the list contains duplicates only the last value is kept.
#
# :param VAR: the output variable name
# :type VAR: string
# :param ARGN: a list of libraries.
#   Each element might either a build configuration keyword or a library.
# :type ARGN: list of strings
#
# @public
#
macro(ament_libraries_deduplicate VAR)
  string(REGEX REPLACE
    "(^|;)(debug|optimized|general);([^;]+)"
    "\\1\\2${AMENT_BUILD_CONFIGURATION_KEYWORD_SEPARATOR}\\3"
    _packed
    "${ARGN}")
  list(REVERSE _packed)
  list(REMOVE_DUPLICATES _packed)
  list(REVERSE _packed)
  string(REGEX REPLACE
    "(^|;)(debug|optimized|general)${AMENT_BUILD_CONFIGURATION_KEYWORD_SEPARATOR}([^;]+)"
    "\\1\\2;\\3"
    ${VAR}
    "${_packed}")
endmacro()
