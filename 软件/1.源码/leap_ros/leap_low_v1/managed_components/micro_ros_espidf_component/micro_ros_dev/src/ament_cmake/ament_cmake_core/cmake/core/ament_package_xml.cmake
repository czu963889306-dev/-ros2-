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
# Parse package.xml from ``DIRECTORY`` and
# make several information available to CMake.
#
# .. note:: It is called automatically by ``ament_package()`` if not
#   called manually before.  It must be called once in each package,
#   after calling ``project()`` where the project name must match the
#   package name.
#
# :param DIRECTORY: the directory of the package.xml (default
#   ``${CMAKE_CURRENT_SOURCE_DIR}``).
# :type DIRECTORY: string
#
# :outvar PACKAGE_NAME: the name of the package from the manifest
# :outvar <packagename>_VERSION: the version number
# :outvar <packagename>_MAINTAINER: the name and email of the
#   maintainer(s)
#
# @public
#
macro(ament_package_xml)
  # verify that project() has been called before
  if(NOT PROJECT_NAME)
    message(FATAL_ERROR "ament_package_xml() PROJECT_NAME is not set. "
      "You must call project() before you can call ament_package_xml().")
  endif()

  # ensure that function is not called multiple times per package
  if(DEFINED _AMENT_PACKAGE_NAME)
    message(FATAL_ERROR "ament_package_xml(): in '${CMAKE_CURRENT_LIST_FILE}',"
      " _AMENT_PACKAGE_NAME is already set (to: ${_AMENT_PACKAGE_NAME}). "
      "Did you call ament_package_xml() multiple times?")
  endif()

  _ament_package_xml(${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_core ${ARGN})

  # verify that the package name from package.xml equals the project() name
  if(NOT _AMENT_PACKAGE_NAME STREQUAL PROJECT_NAME)
    message(FATAL_ERROR "ament_package_xml() package name "
      "'${_AMENT_PACKAGE_NAME}'  in '${_PACKAGE_XML_DIRECTORY}/package.xml' "
      "does not match current PROJECT_NAME '${PROJECT_NAME}'. "
      "You must call project() with the same package name before.")
  endif()
endmacro()

macro(_ament_package_xml dest_dir)
  cmake_parse_arguments(PACKAGE_XML "" "DIRECTORY" "" ${ARGN})
  if(PACKAGE_XML_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_package_xml() called with unused arguments: "
      "${PACKAGE_XML_UNPARSED_ARGUMENTS}")
  endif()

  # set default directory
  if(NOT PACKAGE_XML_DIRECTORY)
    set(PACKAGE_XML_DIRECTORY ${PROJECT_SOURCE_DIR})
  endif()

  # stamp package.xml as well as script to generate CMake code from it
  stamp(${PACKAGE_XML_DIRECTORY}/package.xml)
  stamp("${ament_cmake_core_DIR}/core/package_xml_2_cmake.py")

  # extract information from package.xml
  file(MAKE_DIRECTORY ${dest_dir})
  if(NOT TARGET Python3::Interpreter)
    message(FATAL_ERROR
      "ament_package_xml() target 'Python3::Interpreter' must exist")
  endif()
  get_executable_path(_python_interpreter Python3::Interpreter CONFIGURE)
  set(_cmd
    "${_python_interpreter}"
    "${ament_cmake_core_DIR}/core/package_xml_2_cmake.py"
    "${PACKAGE_XML_DIRECTORY}/package.xml"
    "${dest_dir}/package.cmake"
  )
  execute_process(
    COMMAND ${_cmd}
    RESULT_VARIABLE _res
  )
  if(NOT _res EQUAL 0)
    string(REPLACE ";" " " _cmd_str "${_cmd}")
    message(FATAL_ERROR
      "execute_process(${_cmd_str}) returned error code ${_res}")
  endif()

  # load extracted variables into cmake
  include(${dest_dir}/package.cmake)
endmacro()
