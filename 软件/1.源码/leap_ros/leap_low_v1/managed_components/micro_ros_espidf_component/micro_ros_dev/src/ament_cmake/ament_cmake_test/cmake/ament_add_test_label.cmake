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
# Add text labels to the LABELS property of a test.
#
# :param testname: the name of the test
# :type testname: string
# :param ARGN: a list of labels
# :type ARGN: list of strings
#
# @public
#
function(ament_add_test_label testname)
  get_test_property(${testname} LABELS labels)
  list(APPEND labels ${ARGN})
  set_tests_properties(${testname} PROPERTIES LABELS "${labels}")
endfunction()
