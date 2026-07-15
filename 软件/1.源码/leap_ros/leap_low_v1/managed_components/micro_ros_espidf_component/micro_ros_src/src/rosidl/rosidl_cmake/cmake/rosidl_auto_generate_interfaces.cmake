# Copyright 2025 Open Source Robotics Foundation, Inc.
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
# Automatically generate code from msg/srv/action interface files.
#
# This macro provides a simplified interface to rosidl_generate_interfaces()
# by automatically:
# - Discovering interface files (.msg, .srv, .action, .idl) in msg/, srv/, and action/ directories
# - Using ${PROJECT_NAME}_BUILD_DEPENDS from package.xml as dependencies
# - Exporting rosidl_default_runtime as a dependency
#
# Requires that the package declares all of the following in `package.xml`:
# - exec_depend on `rosidl_default_runtime`
# - buildtool_depend on `rosidl_default_generators`
# - member_of_group `rosidl_interface_packages`
#
# :param TARGETS: list of targets in same package that should be linked against
#   same package's interface typesupport library so they can use the generated code of interfaces.
# :type TARGETS: list of strings
# :param TYPESUPPORT: the typesupport package to use for linking TARGETS.
#   Defaults to "rosidl_typesupport_cpp". Use "rosidl_typesupport_c" for C projects.
# :type TYPESUPPORT: string
# :param LIBRARY_NAME: the base name of the library, specific generators might
#   append their own suffix (passed to rosidl_generate_interfaces)
# :type LIBRARY_NAME: string
# :param ADD_LINTER_TESTS: if set lint the interface files using
#   the ``ament_lint`` package (passed to rosidl_generate_interfaces)
# :type ADD_LINTER_TESTS: option
# :param SKIP_INSTALL: if set skip installing the interface files
#   (passed to rosidl_generate_interfaces)
# :type SKIP_INSTALL: option
#
# @public
#
macro(rosidl_auto_generate_interfaces)
  cmake_parse_arguments(_ARG "ADD_LINTER_TESTS;SKIP_INSTALL" "TYPESUPPORT;LIBRARY_NAME" "TARGETS" ${ARGN})
  if(_ARG_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "rosidl_auto_generate_interfaces called with unused arguments: ${_ARG_UNPARSED_ARGUMENTS}")
  endif()

  # Set default typesupport if not specified
  if(NOT _ARG_TYPESUPPORT)
    set(_ARG_TYPESUPPORT "rosidl_typesupport_cpp")
  endif()

  # Ensure package.xml is parsed so *_DEPENDS variables are available
  if(NOT _AMENT_PACKAGE_NAME)
    ament_package_xml()
  endif()

  # Validate required <exec_depend> tags
  if(NOT rosidl_default_runtime IN_LIST ${PROJECT_NAME}_EXEC_DEPENDS)
    message(FATAL_ERROR
      "Packages installing interfaces must include "
      "'<exec_depend>rosidl_default_runtime</exec_depend>' "
      "in their package.xml")
  endif()

  # Validate required <buildtool_depend> tags
  if(NOT rosidl_default_generators IN_LIST ${PROJECT_NAME}_BUILDTOOL_DEPENDS)
    message(FATAL_ERROR
      "Packages installing interfaces must include "
      "'<buildtool_depend>rosidl_default_generators</buildtool_depend>' "
      "in their package.xml")
  endif()

  # Validate required <member_of_group> tags
  if(NOT rosidl_interface_packages IN_LIST ${PROJECT_NAME}_MEMBER_OF_GROUPS)
    message(FATAL_ERROR
      "Packages installing interfaces must include "
      "'<member_of_group>rosidl_interface_packages</member_of_group>' "
      "in their package.xml")
  endif()

  set(${PROJECT_NAME}_interface_files "")
  file(
    GLOB
    ${PROJECT_NAME}_interface_files
    RELATIVE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    CONFIGURE_DEPENDS
    "msg/*.msg"
    "msg/*.idl"
    "srv/*.srv"
    "srv/*.idl"
    "action/*.action"
    "action/*.idl"
  )

  if(NOT ${PROJECT_NAME}_interface_files)
    message(WARNING "rosidl_auto_generate_interfaces: no message, service, or action files found")
    return()
  endif()

  # Build arguments for rosidl_generate_interfaces
  set(_rosidl_args
    ${PROJECT_NAME}
    ${${PROJECT_NAME}_interface_files}
  )
  if(${PROJECT_NAME}_BUILD_DEPENDS)
    list(APPEND _rosidl_args DEPENDENCIES ${${PROJECT_NAME}_BUILD_DEPENDS})
  endif()
  if(_ARG_LIBRARY_NAME)
    list(APPEND _rosidl_args LIBRARY_NAME ${_ARG_LIBRARY_NAME})
  endif()
  if(_ARG_ADD_LINTER_TESTS)
    list(APPEND _rosidl_args ADD_LINTER_TESTS)
  endif()
  if(_ARG_SKIP_INSTALL)
    list(APPEND _rosidl_args SKIP_INSTALL)
  endif()
  rosidl_generate_interfaces(${_rosidl_args})
  ament_export_dependencies(rosidl_default_runtime)

  # Optionally wire interfaces into same-package targets
  if(_ARG_TARGETS)
    rosidl_get_typesupport_target(${PROJECT_NAME}_auto_typesupport_target ${PROJECT_NAME} rosidl_typesupport_cpp)
    foreach(target IN LISTS _ARG_TARGETS)
      target_link_libraries(${target} ${${PROJECT_NAME}_auto_typesupport_target})
    endforeach()
  endif()

  ament_execute_extensions(rosidl_auto_generate_interfaces)
endmacro()
