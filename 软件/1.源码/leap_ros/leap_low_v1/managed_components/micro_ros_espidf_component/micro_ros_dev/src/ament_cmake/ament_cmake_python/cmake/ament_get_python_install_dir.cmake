# Copyright 2014-2020 Open Source Robotics Foundation, Inc.
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
# Get the Python installation directory.
#
# :param python_install_dir_out: this var will be populated with the path to the
#   Python installation directory.
# :type package_name: string
#
function(ament_get_python_install_dir python_install_dir_out)
  _ament_cmake_python_get_python_install_dir()
  if(NOT PYTHON_INSTALL_DIR)
    message(FATAL_ERROR "ament_get_python_install_dir() variable "
      "'PYTHON_INSTALL_DIR' must not be empty")
  endif()
  set(${python_install_dir_out} ${PYTHON_INSTALL_DIR} PARENT_SCOPE)
endfunction()
