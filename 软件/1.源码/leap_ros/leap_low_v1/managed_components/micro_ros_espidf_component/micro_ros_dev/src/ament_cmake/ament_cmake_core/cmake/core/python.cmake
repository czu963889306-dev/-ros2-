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

# ament_cmake needs a Python 3 interpreter
# If a specific Python version is required then find it before finding ament_cmake
# Example:
#   find_package(Python3 3.8 REQUIRED)
#   find_package(ament_cmake REQUIRED)
if(NOT TARGET Python3::Interpreter)
  # We expect that Python dependencies are met for whatever Python interpreter
  # is invoked by the "python3" executable, however this may not be the latest
  # Python version installed on the system. The default behavior of
  # find_package(Python3) is to use the latest version (e.x. python3.12), so we
  # specifically look for a "python3" executable and if found, instruct
  # find_package(Python3) to use that.
  # On Windows, the find_package(Python3) logic is different and doesn't
  # appear to prefer specific versions (e.x. python3.12) over plain
  # "python.exe" so this extra logic is unnecessary there.
  if(NOT WIN32 AND NOT Python3_EXECUTABLE)
    find_program(Python3_EXECUTABLE python3)
  endif()
  
  # FindPython3 is only available in CMake 3.12 or higher,
  # so we need at least that version.
  cmake_minimum_required(VERSION 3.12)
  find_package(Python3 REQUIRED COMPONENTS Interpreter)
endif()
