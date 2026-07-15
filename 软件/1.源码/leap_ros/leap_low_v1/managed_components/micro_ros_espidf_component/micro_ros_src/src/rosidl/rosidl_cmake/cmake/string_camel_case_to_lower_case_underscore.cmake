# Copyright 2015 Open Source Robotics Foundation, Inc.
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
# Convert a camel case string to lower case and underscores.
#
# :param str: the string
# :type str: string
# :param var: the output variable name
# :type var: bool
#
function(string_camel_case_to_lower_case_underscore str var)
  # insert an underscore before any upper case letter
  # which is not followed by another upper case letter
  string(REGEX REPLACE "(.)([A-Z][a-z]+)" "\\1_\\2" value "${str}")
  # insert an underscore before any upper case letter
  # which is preseded by a lower case letter or number
  string(REGEX REPLACE "([a-z0-9])([A-Z])" "\\1_\\2" value "${value}")
  string(TOLOWER "${value}" value)
  set(${var} "${value}" PARENT_SCOPE)
endfunction()
