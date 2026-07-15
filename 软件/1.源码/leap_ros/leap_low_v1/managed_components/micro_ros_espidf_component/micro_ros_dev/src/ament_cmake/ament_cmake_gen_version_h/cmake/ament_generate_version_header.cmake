# Copyright 2022 Open Source Robotics Foundation, Inc.
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

# This functions creates and installs a version header file.
#
# It uses a provided "version.h.in" template file to generate
# the destination version file in the provided folder.
# The version is taken from `package.xml` file's `<version>` tag.
#
# The generated file is created when
# - the file does not exist or
# - the package.xml file changes
#
# Example with default arguments
#
#   CMake:
#     project(my_project)
#     ...
#     add_library(my_lib ...)
#     ament_generate_version_header(my_lib)
#
#   How to include the header:
#     #include <my_project/version.h>
#
#   The header is installed to:
#     ${CMAKE_INSTALL_PREFIX}/include/my_project/my_project/libversion.h
#
# Example with HEADER_PATH specified
#
#   CMake:
#     project(my_project)
#     ...
#     add_library(my_lib ...)
#     ament_generate_version_header(my_lib
#       HEADER_PATH "foobar/version.hpp")
#
#   How to include the header:
#     #include <foobar/version.hpp>
#
#   The header is installed to:
#     ${CMAKE_INSTALL_PREFIX}/include/my_project/foobar/version.hpp
#
# Example with INSTALL_PATH specified
#
#   CMake:
#     project(my_project)
#     ...
#     add_library(my_lib ...)
#     ament_generate_version_header(my_lib
#       INSTALL_PATH "include")
#
#   How to include the header:
#     #include <my_project/version.h>
#
#   The header is installed to:
#     ${CMAKE_INSTALL_PREFIX}/include/my_project/version.hpp
#
# :param target: A non-imported target to which the generated header will be
#   made available from.
#   `target_include_directories(${target} ...)` will be used such that linking
#   against the target will allow one to include this header.
# :type target: string
# :param HEADER_PATH: Path of the generated header including the file name
#   that describes how it should be included by downstream targets.
#   The default is `${PROJECT_NAME}/version.h`
# :type HEADER_PATH: string
# :param INSTALL_PATH: Path that the header should be installed at.
#   The default value is "include/${PROJECT_NAME}" to avoid include directory
#   search order problems when overriding packages from merged workspaces.
# :type INSTALL_PATH: string
# :param SKIP_INSTALL: whether to autmatically install the generated version
#   file.
#   The default value is FALSE.
# :type SKIP_INSTALL: BOOL
#
# @public
#
function(ament_generate_version_header target)
  # Validate arguments
  cmake_parse_arguments(
    ARG
    "SKIP_INSTALL"
    "HEADER_PATH;INSTALL_PATH"
    ""
    ${ARGN}
  )
  if(NOT ARG_HEADER_PATH)
    set(ARG_HEADER_PATH "${PROJECT_NAME}/version.h")
  endif()
  if(NOT ARG_INSTALL_PATH)
    set(ARG_INSTALL_PATH "include/${PROJECT_NAME}")
  endif()
  if(NOT TARGET ${target})
    message(FATAL_ERROR "A non-imported target called '${target}' must exist")
  endif()

  # Make sure the templates to use are available
  set(VERSION_TEMPLATE_FILE "${ament_cmake_gen_version_h_DIR}/version.h.in")
  if(NOT EXISTS "${VERSION_TEMPLATE_FILE}")
    message(FATAL_ERROR "Can't find ${VERSION_TEMPLATE_FILE}. Reinstall ament_cmake_gen_version_h package.")
  endif()
  set(SCRIPT_TEMPLATE_FILE "${ament_cmake_gen_version_h_DIR}/generate_version_header.cmake.in")
  if(NOT EXISTS "${SCRIPT_TEMPLATE_FILE}")
    message(FATAL_ERROR "Can't find ${SCRIPT_TEMPLATE_FILE}. Reinstall ament_cmake_gen_version_h package.")
  endif()

  # retrieve version information from <package>.xml file
  if(NOT _AMENT_PACKAGE_NAME)
    ament_package_xml()
  endif()
  string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
  set(VERSION_STR ${${PROJECT_NAME}_VERSION})

  # parse version information from the version string
  if(NOT VERSION_STR MATCHES "([0-9]+)\.([0-9]+)\.([0-9]+)")
    message(FATAL_ERROR "Version string must be of format MAJOR.MINOR.PATCH")
  endif()
  set(VERSION_MAJOR ${CMAKE_MATCH_1})
  set(VERSION_MINOR ${CMAKE_MATCH_2})
  set(VERSION_PATCH ${CMAKE_MATCH_3})

  set(BUILDTIME_HEADER_DIR "${CMAKE_CURRENT_BINARY_DIR}/ament_generate_version_header/${target}")
  set(GENERATED_HEADER_FILE "${BUILDTIME_HEADER_DIR}/${ARG_HEADER_PATH}")

  # Create a CMake script that will generate the version header
  set(GENERATOR_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/ament_generate_version_header/${target}/generate_version_header.cmake")
  configure_file("${SCRIPT_TEMPLATE_FILE}" "${GENERATOR_SCRIPT}" @ONLY)

  # Setup a command to run the generation script when input files change
  add_custom_command(
    OUTPUT "${GENERATED_HEADER_FILE}"
    COMMAND ${CMAKE_COMMAND} -P "${GENERATOR_SCRIPT}"
    DEPENDS
      "${PACKAGE_XML_DIRECTORY}/package.xml"
      "${VERSION_TEMPLATE_FILE}"
      "${SCRIPT_TEMPLATE_FILE}"
    COMMENT "Generating ${ARG_HEADER_PATH}")

  add_custom_target("ament_generate_version_header__${target}" ALL
    DEPENDS "${GENERATED_HEADER_FILE}")
  add_dependencies("${target}" "ament_generate_version_header__${target}")

  # Make generated header includable to this and downstream targets
  get_target_property(type "${target}" TYPE)
  if(${type} STREQUAL "INTERFACE_LIBRARY")
    set(keyword "INTERFACE")
  else()
    set(keyword "PUBLIC")
  endif()
  target_include_directories("${target}" "${keyword}"
    "$<BUILD_INTERFACE:${BUILDTIME_HEADER_DIR}>"
    "$<INSTALL_INTERFACE:${ARG_INSTALL_PATH}>")

  if(NOT ARG_SKIP_INSTALL)
    get_filename_component(HEADER_FOLDER "${ARG_HEADER_PATH}" DIRECTORY)
    install(FILES "${BUILDTIME_HEADER_DIR}/${ARG_HEADER_PATH}"
      DESTINATION "${ARG_INSTALL_PATH}/${HEADER_FOLDER}")
  endif()
  endfunction()
