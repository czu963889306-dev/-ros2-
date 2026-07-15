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
# Include all registered extensions.
#
# :param extension_point: the name of the extension point
# :type extension_point: string
# :param EXCLUDE: List of packages that should be skipped
# :type EXCLUDE: list of strings
#
# @public
#
macro(ament_execute_extensions extension_point)
  cmake_parse_arguments(_ARG_AMENT_EXECUTE_EXTENSIONS "" "" "EXCLUDE" ${ARGN})
  if(_ARG_AMENT_EXECUTE_EXTENSIONS_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_execute_extensions() called with "
      "unused arguments: ${_ARG_AMENT_EXECUTE_EXTENSIONS_UNPARSED_ARGUMENTS}")
  endif()
  if(AMENT_EXTENSIONS_${extension_point})
    foreach(_extension ${AMENT_EXTENSIONS_${extension_point}})
      string(REPLACE ":" ";" _extension_list "${_extension}")
      list(LENGTH _extension_list _length)
      if(NOT _length EQUAL 2)
        message(FATAL_ERROR "ament_execute_extensions(${extension_point}) "
          "registered extension '${_extension}' can not be split into package "
          "name and cmake filename")
      endif()
      list(GET _extension_list 0 _pkg_name)
      if("${_pkg_name}" IN_LIST _ARG_AMENT_EXECUTE_EXTENSIONS_EXCLUDE)
        continue()
      endif()
      list(GET _extension_list 1 _cmake_filename)
      set(_extension_file "${${_pkg_name}_DIR}/${_cmake_filename}")
      assert_file_exists("${_extension_file}"
        "ament_execute_extensions(${extension_point}) registered extension '${_extension_file}' does not exist")
      include("${_extension_file}")
    endforeach()
  endif()
endmacro()
