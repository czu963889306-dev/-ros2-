# Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

cmake_minimum_required(VERSION 3.5)

project(rosidl_typesupport_microxrcedds_test_msg)

find_package(ament_cmake REQUIRED)

if(BUILD_TESTING)

  find_package(builtin_interfaces REQUIRED)
  find_package(rosidl_default_generators REQUIRED)

  set(msg_files
    "msg/UnboundedString.msg"
    "msg/Primitive.msg"
    "msg/NotEnoughtMemoryTest.msg"
    "msg/Sequence.msg"
    "msg/Compound.msg"
    "msg/BasicPrimitive.msg"
    "msg/Regression10.msg"
    "msg/Regression11.msg"
    )

  rosidl_generate_interfaces(${PROJECT_NAME}
      ${msg_files}
    DEPENDENCIES
      builtin_interfaces
    ADD_LINTER_TESTS
    )

  ament_export_dependencies(rosidl_default_runtime)

endif()

ament_package()
