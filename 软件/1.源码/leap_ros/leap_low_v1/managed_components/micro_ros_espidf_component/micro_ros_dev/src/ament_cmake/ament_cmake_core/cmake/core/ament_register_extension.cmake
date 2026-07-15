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
# Register a CMake filename to be included as part of an extension
# point.
#
# :param extension_point: the name of the extension point
# :type extension_point: string
# :param package_name: the name of the package containing the CMake
#   file
# :type package_name: string
# :param cmake_filename: the path to a CMake file relative to the
#   ${package_name}_DIR folder
# :type cmake_filename: string
#
# @public
#
macro(ament_register_extension extension_point package_name cmake_filename)
  list(APPEND AMENT_EXTENSIONS_${extension_point}
    "${package_name}:${cmake_filename}")
endmacro()
