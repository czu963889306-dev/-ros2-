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
# Install the package.xml file, and generate code for
# ``find_package`` so that other packages can get information about
# this package.
#
# .. note:: It must be called once for each package.
#   It is indirectly calling``ament_package_xml()`` which will
#   provide additional output variables.
#
# :param CONFIG_EXTRAS: a list of CMake files containing extra stuff
#   that should be accessible to users of this package after
#   ``find_package``\ -ing it.
#   The file can either be a plain CMake file (ending in '.cmake') or
#   a template which is expanded using configure_file() (ending in
#   '.cmake.in') with @ONLY.
#   If the global variable ${PROJECT_NAME}_CONFIG_EXTRAS is set it
#   will be appended to the explicitly passed argument.
# :type CONFIG_EXTRAS: list of files
# :param CONFIG_EXTRAS_POST: a list of CMake files containing extra
#   stuff that should be accessible to users of this package after
#   ``find_package``\ -ing it.
#   The file can either be a plain CMake file (ending in '.cmake') or
#   a template which is expanded using configure_file() (ending in
#   '.cmake.in') with @ONLY.
#   If the global variable ${PROJECT_NAME}_CONFIG_EXTRAS_POST is set it
#   will be prepended to the explicitly passed argument.
# :type CONFIG_EXTRAS_POST: list of files
#
# @public
#
macro(ament_package)
  # verify that project() has been called before
  if(NOT PROJECT_NAME)
    message(FATAL_ERROR "ament_package() PROJECT_NAME is not set. You must "
      "call project() before calling ament_package().")
  endif()
  if(PROJECT_NAME STREQUAL "Project")
    message(FATAL_ERROR "ament_package() PROJECT_NAME is set to 'Project', "
      "which is not a valid project name. "
      "You must call project() before calling ament_package().")
  endif()

  # mark that ament_package() was called
  # in order to detect wrong order of calling
  set(_${PROJECT_NAME}_AMENT_PACKAGE TRUE)

  # call ament_package_xml() if it has not been called before
  if(NOT _AMENT_PACKAGE_NAME)
    ament_package_xml()
  endif()

  ament_execute_extensions(ament_package)

  _ament_package(${ARGN})
endmacro()

function(_ament_package)
  cmake_parse_arguments(PACKAGE "" "" "CONFIG_EXTRAS;CONFIG_EXTRAS_POST" ${ARGN})
  if(PACKAGE_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "ament_package() called with unused arguments: "
      "${PACKAGE_UNPARSED_ARGUMENTS}")
  endif()

  # set variable required by the configured files
  set(PACKAGE_VERSION "${${PROJECT_NAME}_VERSION}")
  set(PACKAGE_DEPRECATED "${${PROJECT_NAME}_DEPRECATED}")

  # expand and install config extras
  set(PACKAGE_CONFIG_EXTRA_FILES "")
  set(extras)
  if(DEFINED PACKAGE_CONFIG_EXTRAS)
    list(APPEND extras ${PACKAGE_CONFIG_EXTRAS})
  endif()
  if(DEFINED ${PROJECT_NAME}_CONFIG_EXTRAS)
    list(APPEND extras ${${PROJECT_NAME}_CONFIG_EXTRAS})
  endif()
  if(DEFINED ${PROJECT_NAME}_CONFIG_EXTRAS_POST)
    list(APPEND extras ${${PROJECT_NAME}_CONFIG_EXTRAS_POST})
  endif()
  if(DEFINED PACKAGE_CONFIG_EXTRAS_POST)
    list(APPEND extras ${PACKAGE_CONFIG_EXTRAS_POST})
  endif()
  foreach(extra ${extras})
    assert_file_exists("${extra}"
      "ament_package() called with extra file '${extra}' which does not exist")
    stamp("${extra}")

    # expand template
    string_ends_with("${extra}" ".cmake.in" is_template)
    if(is_template)
      get_filename_component(extra_filename "${extra}" NAME)
      # cut of .in extension
      string(LENGTH "${extra_filename}" length)
      math(EXPR offset "${length} - 3")
      string(SUBSTRING "${extra_filename}" 0 ${offset} extra_filename)
      configure_file(
        "${extra}"
        ${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_core/${extra_filename}
        @ONLY
      )
      set(extra
        "${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_core/${extra_filename}")
    endif()

    # install cmake file and register for CMake config file
    string_ends_with("${extra}" ".cmake" is_cmake)
    if(is_cmake)
      install(FILES
        ${extra}
        DESTINATION share/${PROJECT_NAME}/cmake
      )
      get_filename_component(extra_filename "${extra}" NAME)
      list(APPEND PACKAGE_CONFIG_EXTRA_FILES "${extra_filename}")
    else()
      message(FATAL_ERROR "ament_package() the CONFIG_EXTRAS file '${extra}' "
        "does neither end with '.cmake' nor with '.cmake.in'.")
    endif()
  endforeach()

  # generate CMake config file
  stamp("${ament_cmake_core_DIR}/core/templates/nameConfig.cmake.in")
  configure_file(
    ${ament_cmake_core_DIR}/core/templates/nameConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_core/${PROJECT_NAME}Config.cmake
    @ONLY
  )
  # generate CMake config-version file
  stamp("${ament_cmake_core_DIR}/core/templates/nameConfig-version.cmake.in")
  configure_file(
    ${ament_cmake_core_DIR}/core/templates/nameConfig-version.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_core/${PROJECT_NAME}Config-version.cmake
    @ONLY
  )

  # install CMake config and config-version files
  install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_core/${PROJECT_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_core/${PROJECT_NAME}Config-version.cmake
    DESTINATION share/${PROJECT_NAME}/cmake
  )

  # install package.xml
  install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/package.xml
    DESTINATION share/${PROJECT_NAME}
  )
endfunction()
