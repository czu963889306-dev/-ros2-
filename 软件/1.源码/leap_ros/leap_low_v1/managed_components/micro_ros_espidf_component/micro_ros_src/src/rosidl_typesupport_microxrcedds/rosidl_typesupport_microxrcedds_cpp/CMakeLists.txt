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

cmake_minimum_required(VERSION 3.12)

project(rosidl_typesupport_microxrcedds_cpp LANGUAGES CXX)

find_package(ament_cmake_python REQUIRED)
find_package(rosidl_runtime_c REQUIRED)
find_package(rosidl_typesupport_microxrcedds_c REQUIRED)

ament_export_dependencies(rmw)
ament_export_dependencies(rosidl_cmake)
ament_export_dependencies(rosidl_runtime_cpp)
ament_export_dependencies(rosidl_typesupport_microxrcedds_c)

ament_export_include_directories(include)

ament_python_install_package(${PROJECT_NAME})

add_library(${PROJECT_NAME}
  src/identifier.cpp
  )

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    $<$<PLATFORM_ID:Windows>:"ROSIDL_TYPESUPPORT_MICROXRCEDDS_CPP_BUILDING_DLL">
  )

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  )

ament_target_dependencies(${PROJECT_NAME}
  "rosidl_typesupport_microxrcedds_c"
  "rosidl_runtime_c"
  )
ament_export_libraries(${PROJECT_NAME})
ament_export_targets(${PROJECT_NAME})

ament_index_register_resource("rosidl_typesupport_cpp")

target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<$<C_COMPILER_ID:GNU>:-Wall>
    $<$<C_COMPILER_ID:GNU>:-Wextra>
    $<$<C_COMPILER_ID:GNU>:-Wpedantic>
  )

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD
    14
  CXX_STANDARD_REQUIRED
    YES
  )

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

if(BUILD_SHARED_LIBS)
  set(${PROJECT_NAME}_LIBRARY_TYPE "SHARED")
else()
  set(${PROJECT_NAME}_LIBRARY_TYPE "STATIC")
endif()

ament_package(
  CONFIG_EXTRAS "rosidl_typesupport_microxrcedds_cpp-extras.cmake.in"
  )

install(
  PROGRAMS
    main/rosidl_typesupport_microxrcedds_cpp
  DESTINATION
    lib/rosidl_typesupport_microxrcedds_cpp
  )

install(
  DIRECTORY
    cmake
    resource
  DESTINATION
    share/${PROJECT_NAME}
  )

install(
  DIRECTORY
    include/
  DESTINATION
    include
  )

install(
  TARGETS
    ${PROJECT_NAME}
  EXPORT
    ${PROJECT_NAME}
  ARCHIVE DESTINATION
    lib
  LIBRARY DESTINATION
    lib
  RUNTIME DESTINATION
    bin
  )
