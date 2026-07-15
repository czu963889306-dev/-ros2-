# Copyright 2019 Open Source Robotics Foundation, Inc.
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
# `ament_cmake_gen_version_h` call creates and installs a version header file.
# The version is taken from `package.xml` file's `<version>` tag
# The function uses a provided "version.h.in" template file to generate
# the destination version file in the provided folder.
# The generated file is being (re-)created if:
# - the file does not exist
# - the file does exists but contains a version that differs from the
#   version in `package.xml` file
#
# :param NO_INSTALL: whether to autmatically install the generated version file
#   into DESTINATION include
#   default value NO_INSTALL: FALSE
# :type NO_INSTALL: BOOL
# :param INCLUDE_DIR: path to the folder where the file will be generated
#   ${INCLUDE_DIR} folder will be added to the include paths
#   the file will be placed into ${INCLUDE_DIR}/${PROJECT_NAME} folder according
#    to ROS2 standard
#   default value INCLUDE_DIR:
#     ${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_gen_version_h/include
# :type INCLUDE_DIR: string
# :param VERSION_FILE_NAME: file name of the generated header file
#   default value VERSION_FILE_NAME: version.h
# :type VERSION_FILE_NAME: string
# :param VERSION_MAJOR: override VERSION_MAJOR, default value VERSION_MAJOR
#   from the package.xml file
# :type VERSION_MAJOR: string
# :param VERSION_MINOR: override VERSION_MINOR, default value VERSION_MINOR
#   from the package.xml file
# :type VERSION_MINOR: string
# :param VERSION_PATCH: override VERSION_PATCH, default value VERSION_PATCH
#   from the package.xml file
# :type VERSION_PATCH: string
#
# @public
#
function(ament_cmake_gen_version_h)
  message(DEPRECATION "The ament_cmake_gen_version_h() function is deprecated. \
  Please use ament_generate_version_header(...) instead.")

  cmake_parse_arguments(
    ARG
    "NO_INSTALL"
    "INCLUDE_DIR;VERSION_FILE_NAME;VERSION_MAJOR;VERSION_MINOR;VERSION_PATCH"
    ""
    ${ARGN}
  )

  set(TEMPLATE_FILE "${ament_cmake_gen_version_h_DIR}/version.h.in")
  if(NOT EXISTS "${TEMPLATE_FILE}")
    message(FATAL_ERROR "Can't find ${TEMPLATE_FILE}. Reinstall ament_cmake_gen_version_h package.")
  endif()

  if(NOT ARG_INCLUDE_DIR)
    set(ARG_INCLUDE_DIR
      ${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_gen_version_h/include)
  endif()
  include_directories(${ARG_INCLUDE_DIR})
  set(TMP_INCLUDE_DIR ${ARG_INCLUDE_DIR}/${PROJECT_NAME})

  if(NOT ARG_VERSION_FILE_NAME)
    set(ARG_VERSION_FILE_NAME version.h)
  endif()
  set(VERSION_FILE_NAME ${TMP_INCLUDE_DIR}/${ARG_VERSION_FILE_NAME})
  set(NEED_TO_CREATE_VERSION_FILE TRUE)

  # retrieve version information from <package>.xml file
  # call ament_package_xml() if it has not been called before
  if(NOT _AMENT_PACKAGE_NAME)
    ament_package_xml()
  endif()

  string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
  set(VERSION_STR ${${PROJECT_NAME}_VERSION})

  # parse version information from the version string
  string(REGEX MATCH "([0-9]+)\.([0-9]+)\.([0-9]+)" "" dummy ${VERSION_STR})
  if(ARG_VERSION_MAJOR)
    set(VERSION_MAJOR ${ARG_VERSION_MAJOR})
  else()
    set(VERSION_MAJOR ${CMAKE_MATCH_1})
  endif()

  if(ARG_VERSION_MINOR)
    set(VERSION_MINOR ${ARG_VERSION_MINOR})
  else()
    set(VERSION_MINOR ${CMAKE_MATCH_2})
  endif()

  if(ARG_VERSION_PATCH)
    set(VERSION_PATCH ${ARG_VERSION_PATCH})
  else()
    set(VERSION_PATCH ${CMAKE_MATCH_3})
  endif()

  set(VERSION_STR ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

  # Check if the version file exist
  if(EXISTS "${VERSION_FILE_NAME}")
    # The file exists
    # Check if it contains the same version
    set(LINE_PATTERN "#define[ \t]+${PROJECT_NAME_UPPER}_VERSION_STR")
    file(STRINGS ${VERSION_FILE_NAME} VERSION_FILE_STRINGS REGEX ${LINE_PATTERN})
    set(VERSION_PATTERN "^#define[ \t]+${PROJECT_NAME_UPPER}_VERSION_STR[ \t]+\"([0-9]+\.[0-9]+\.[0-9]+)\"")
    string(REGEX MATCH ${VERSION_PATTERN} dummy ${VERSION_FILE_STRINGS})
    if("${CMAKE_MATCH_1}" STREQUAL "${VERSION_STR}")
      message(STATUS "File and project versions match: \"${CMAKE_MATCH_1}\" == \"${VERSION_STR}\"")
      set(NEED_TO_CREATE_VERSION_FILE FALSE)
    endif()
  endif()

  if(${NEED_TO_CREATE_VERSION_FILE})
    message(STATUS "Create new version file for version ${${PROJECT_NAME}_VERSION}")
    file(MAKE_DIRECTORY ${TMP_INCLUDE_DIR})
    # create the version.h file
    configure_file(${TEMPLATE_FILE} ${VERSION_FILE_NAME})
  else()
    message(STATUS "Skip version file creation")
  endif()

  if(NOT ARG_NO_INSTALL)
    install(
      DIRECTORY ${TMP_INCLUDE_DIR}
      DESTINATION include)
  endif()
endfunction()
