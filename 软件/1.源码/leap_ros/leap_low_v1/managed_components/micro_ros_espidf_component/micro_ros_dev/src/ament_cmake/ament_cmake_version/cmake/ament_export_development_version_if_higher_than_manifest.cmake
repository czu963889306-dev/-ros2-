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
# Set the exported package version to the passed value if the package
# version in the manifest is lower.
#
# It is recommended to append the suffix ``-dev`` to the passed upcoming
# version number.
# If the package version in the manifest is equal or newer than the passed
# development version this function call becomes a no-op.
# If the function is called multiple times only the higher version number will
# be used.
#
# .. note:: It is indirectly calling``ament_package_xml()`` if that hasn't
#   happened already.
#
# :param target: development_version
# :type target: string
#
# @public
#
macro(ament_export_development_version_if_higher_than_manifest development_version)
  if(${ARGN})
    message(FATAL_ERROR
      "ament_export_development_version_if_higher_than_manifest() called with unused arguments: ${ARGN}")
  endif()

  if(DEFINED _${PROJECT_NAME}_AMENT_PACKAGE)
    message(FATAL_ERROR
      "ament_export_development_version_if_higher_than_manifest() must be called before "
      "ament_package()")
  endif()

  # call ament_package_xml() if it has not been called before
  if(NOT _AMENT_PACKAGE_NAME)
    ament_package_xml()
  endif()

  if("${${PROJECT_NAME}_VERSION}" VERSION_LESS "${development_version}")
    message(STATUS
      "Overriding exported package version '${${PROJECT_NAME}_VERSION}' with "
      "development version '${development_version}'")
    set(${PROJECT_NAME}_VERSION "${development_version}")
  endif()
endmacro()
