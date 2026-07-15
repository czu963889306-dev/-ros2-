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
#   :param path:  file name
#
#   Uses ``configure_file`` to generate a file ``filepath.stamp`` hidden
#   somewhere in the build tree.  This will cause cmake to rebuild its
#   cache when ``filepath`` is modified.
#
function(stamp path)
  get_filename_component(filename "${path}" NAME)
  configure_file(
    "${path}"
    "${CMAKE_CURRENT_BINARY_DIR}/ament_cmake_core/stamps/${filename}.stamp"
    COPYONLY
  )
endfunction()
