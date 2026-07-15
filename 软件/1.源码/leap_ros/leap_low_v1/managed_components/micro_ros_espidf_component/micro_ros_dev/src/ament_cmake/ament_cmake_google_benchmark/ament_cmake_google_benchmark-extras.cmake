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

macro(_ament_cmake_google_benchmark_find_benchmark)
  if(NOT DEFINED _AMENT_CMAKE_GOOGLE_BENCHMARK_FIND_BENCHMARK)
    set(_AMENT_CMAKE_GOOGLE_BENCHMARK_FIND_BENCHMARK TRUE)

    option(AMENT_RUN_PERFORMANCE_TESTS
      "Enable performance tests instead of unconditionally skipping them" OFF)

    find_package(benchmark QUIET)

    if(NOT benchmark_FOUND)
      message(WARNING
        "'benchmark' not found, C++ tests using 'Google Benchmark' can not be "
        "built. Please install the 'Google Benchmark' headers globally in "
        "your system to enable these tests (e.g. on Ubuntu/Debian install the "
        "package 'libbenchmark-dev') or get the ament package "
        "'google_benchmark_vendor'")
    elseif(NOT AMENT_RUN_PERFORMANCE_TESTS)
      message(STATUS
        "Performance tests are disabled by default, so Google Benchmark tests "
        "will be skipped. To enable these tests, set the CMake variable "
        "'AMENT_RUN_PERFORMANCE_TESTS'.")
    endif()
  endif()
endmacro()

include("${ament_cmake_google_benchmark_DIR}/ament_add_google_benchmark.cmake")
include("${ament_cmake_google_benchmark_DIR}/ament_add_google_benchmark_executable.cmake")
include("${ament_cmake_google_benchmark_DIR}/ament_add_google_benchmark_test.cmake")
