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

# copied from ament_cmake_nose/ament_cmake_nose-extras.cmake

# find nosetests once
macro(_ament_cmake_nose_find_nosetests)
  if(NOT DEFINED _AMENT_CMAKE_NOSE_FIND_NOSETESTS)
    set(_AMENT_CMAKE_NOSE_FIND_NOSETESTS TRUE)

    find_package(ament_cmake_core QUIET REQUIRED)
    find_package(ament_cmake_test QUIET REQUIRED)

    find_program(NOSETESTS NAMES
      "nosetests${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}"
      "nosetests-${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}"
      "nosetests${Python3_VERSION_MAJOR}"
      "nosetests-${Python3_VERSION_MAJOR}"
      "nosetests")
    if(NOSETESTS)
      # if Python is located in a path containing spaces the shebang line of nosetests is invalid
      # to avoid using the shebang line of nosetests the script is being invoked through Python
      get_executable_path(_python_interpreter Python3::Interpreter CONFIGURE)
      set(_cmd "${_python_interpreter}" "${NOSETESTS}" "--version")
      execute_process(
        COMMAND ${_cmd}
        RESULT_VARIABLE _res
        OUTPUT_VARIABLE _output
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      if(NOT _res EQUAL 0)
        string(REPLACE ";" " " _cmd_str "${_cmd}")
        message(FATAL_ERROR "Failed to invoke nosetest: '${_cmd_str}' returned error code ${_res}")
      endif()
      string(REPLACE " version " ";" _output_list "${_output}")
      list(LENGTH _output_list _length)
      if(NOT _length EQUAL 2)
        message(FATAL_ERROR "Failed to extract nosetest version from '${_output}'")
      endif()
      list(GET _output_list 1 NOSETESTS_VERSION)
      message(STATUS "Using Python nosetests: ${NOSETESTS} (${NOSETESTS_VERSION})")
    else()
      message(WARNING
        "'nosetests' not found, Python nose tests can not be run (e.g. on "
        "Ubuntu/Debian install the package 'python3-nose')")
    endif()
  endif()
endmacro()

include("${ament_cmake_nose_DIR}/ament_add_nose_test.cmake")
include("${ament_cmake_nose_DIR}/ament_find_nosetests.cmake")
