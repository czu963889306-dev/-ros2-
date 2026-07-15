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
# Add an executable using google benchmark.
#
# Call add_executable(target ARGN) and link it against the google benchmark
# libraries.
# It does not register the executable as a test.
#
# If google benchmark is not available the specified target is not being created
# and therefore the target existence should be checked before being used.
#
# :param target: the target name which will also be used as the test name
# :type target: string
# :param ARGN: the list of source files
# :type ARGN: list of strings
# :param SKIP_LINKING_MAIN_LIBRARIES: if set skip linking against the google
#   benchmark main libraries
# :type SKIP_LINKING_MAIN_LIBRARIES: option
#
# @public
#
macro(ament_add_google_benchmark_executable target)
  _ament_cmake_google_benchmark_find_benchmark()
  if(benchmark_FOUND)
    _ament_add_google_benchmark_executable("${target}" ${ARGN})
  endif()
endmacro()

function(_ament_add_google_benchmark_executable target)
  cmake_parse_arguments(ARG "SKIP_LINKING_MAIN_LIBRARIES" "" "" ${ARGN})
  if(NOT ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR
      "ament_add_google_benchmark_executable() must be invoked with at least "
      "one source file")
  endif()

  # should be EXCLUDE_FROM_ALL if it would be possible
  # to add this target as a dependency to the "test" target
  add_executable("${target}" ${ARG_UNPARSED_ARGUMENTS})
  if(NOT ARG_SKIP_LINKING_MAIN_LIBRARIES)
    target_link_libraries("${target}" benchmark::benchmark_main)
  endif()
  target_link_libraries("${target}" benchmark::benchmark)
endfunction()
