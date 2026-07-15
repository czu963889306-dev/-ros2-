# Copyright 2016-2018 Open Source Robotics Foundation, Inc.
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

from rosidl_cmake import generate_files


def generate_c(generator_arguments_file, type_supports):
    """
    Generate the C type support to handle ROS messages.

    :param generator_arguments_file: Path location of the file containing the generator arguments
    :param type_support: List of type supports to be used
    """
    mapping = {
        'idl__type_support.cpp.em': '%s__type_support.cpp',
    }
    return generate_files(
        generator_arguments_file, mapping,
        additional_context={'type_supports': type_supports})
