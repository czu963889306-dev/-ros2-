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
# Export information, install files and targets, execute the
# extension point ``ament_auto_package`` and invoke
# ``ament_package()``.
#
# :param INSTALL_TO_PATH: if set, install executables to `bin` so that
#   they are available on the `PATH`.
#   By default they are being installed into `lib/${PROJECT_NAME}`.
#   It is currently not possible to install some executable into `bin`
#   and some into `lib/${PROJECT_NAME}`.
#   Libraries are not affected by this option.
#   They are always installed into `lib` and `dll`s into `bin`.
# :type INSTALL_TO_PATH: option
# :param INSTALL_TO_SHARE: a list of directories to be installed to the
#   package's share directory
# :type INSTALL_TO_SHARE: list of strings
# :param USE_SCOPED_HEADER_INSTALL_DIR: if set, install headers to
#   `include/${PROJECT_NAME}` instead of `include` directory.
#   This helps prevent include path pollution when user package is
#   merge-installed underlay package.
#   For detail, see https://github.com/ros2/ros2/issues/1150.
# :type USE_SCOPED_HEADER_INSTALL_DIR: option
# :param ARGN: any other arguments are passed through to ament_package()
# :type ARGN: list of strings
#
# Export all found build dependencies which are also run
# dependencies.
# If the package has an include directory install all recursively
# found header files (ending in h, hh, hpp, hxx) and export the
# include directory.
# Export and install all library targets and install all executable
# targets.
#
# @public
#

macro(ament_auto_package)
  cmake_parse_arguments(_ARG_AMENT_AUTO_PACKAGE
    "INSTALL_TO_PATH;USE_SCOPED_HEADER_INSTALL_DIR"
    ""
    "INSTALL_TO_SHARE"
    ${ARGN})
  # passing all unparsed arguments to ament_package()

  # export all found build dependencies which are also run dependencies
  set(_run_depends
    ${${PROJECT_NAME}_BUILD_EXPORT_DEPENDS}
    ${${PROJECT_NAME}_BUILDTOOL_EXPORT_DEPENDS}
    ${${PROJECT_NAME}_EXEC_DEPENDS})
  foreach(_dep
      ${${PROJECT_NAME}_FOUND_BUILD_DEPENDS}
      ${${PROJECT_NAME}_FOUND_BUILDTOOL_DEPENDS})
    if(_dep IN_LIST _run_depends)
      ament_export_dependencies("${_dep}")
    endif()
  endforeach()

  # export and install include directory of this package if it has one
  if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include")
    if(_ARG_AMENT_AUTO_PACKAGE_USE_SCOPED_HEADER_INSTALL_DIR)
      ament_export_include_directories("include/${PROJECT_NAME}")
      install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
    else()
      ament_export_include_directories("include")
      install(DIRECTORY include/ DESTINATION include)
      message(
        "In this package, headers install destination is set to `include` "
        "by ament_auto_package. It is recommended to install "
        "`include/${PROJECT_NAME}` instead and will be the default behavior "
        "of ament_auto_package from ROS 2 Kilted Kaiju. On distributions before "
        "Kilted, ament_auto_package behaves the same way when you use "
        "USE_SCOPED_HEADER_INSTALL_DIR option.")
    endif()
  endif()

  # export and install all libraries
  if(NOT ${PROJECT_NAME}_LIBRARIES STREQUAL "")
    ament_export_libraries(${${PROJECT_NAME}_LIBRARIES})
    install(
      TARGETS ${${PROJECT_NAME}_LIBRARIES}
      ARCHIVE DESTINATION lib
      LIBRARY DESTINATION lib
      RUNTIME DESTINATION bin
    )
  endif()

  # install all executables
  if(NOT ${PROJECT_NAME}_EXECUTABLES STREQUAL "")
    if(_ARG_AMENT_AUTO_PACKAGE_INSTALL_TO_PATH)
      set(_destination "bin")
    else()
      set(_destination "lib/${PROJECT_NAME}")
    endif()
    install(
      TARGETS ${${PROJECT_NAME}_EXECUTABLES}
      DESTINATION ${_destination}
    )
  endif()

  # install directories to share
  foreach(_dir ${_ARG_AMENT_AUTO_PACKAGE_INSTALL_TO_SHARE})
    install(
      DIRECTORY "${_dir}"
      DESTINATION "share/${PROJECT_NAME}"
    )
  endforeach()

  ament_execute_extensions(ament_auto_package)

  ament_package(${_ARG_AMENT_AUTO_PACKAGE_UNPARSED_ARGUMENTS})
endmacro()
