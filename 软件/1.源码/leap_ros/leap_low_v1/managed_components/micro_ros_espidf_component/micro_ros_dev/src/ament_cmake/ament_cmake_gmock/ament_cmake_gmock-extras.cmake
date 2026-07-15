# Copyright 2014-2015 Open Source Robotics Foundation, Inc.
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

# copied from ament_cmake_gmock/ament_cmake_gmock-extras.cmake

# find gmock and create library targets once
macro(_ament_cmake_gmock_find_gmock)
  if(NOT DEFINED _AMENT_CMAKE_GMOCK_FIND_GMOCK)
    set(_AMENT_CMAKE_GMOCK_FIND_GMOCK TRUE)

    find_package(ament_cmake_test QUIET REQUIRED)
    find_package(gmock_vendor QUIET)

    # if gmock sources were not found in a previous run
    if(NOT GMOCK_FROM_SOURCE_FOUND)
      # search path for gmock includes and sources
      set(_search_path_include "")
      set(_search_path_src "")

      # option() consider environment variable to find gmock
      if(NOT $ENV{GMOCK_DIR} STREQUAL "")
        list(APPEND _search_path_include "$ENV{GMOCK_DIR}/include/gmock")
        list(APPEND _search_path_src "$ENV{GMOCK_DIR}/src")
      endif()

      # check to system installed path (i.e. on Ubuntu)
      set(_search_path_include "/usr/include/gmock")
      set(_search_path_src "/usr/src/gmock/src")

      # check gmock_vendor path, prefer this version over a system installed
      if(gmock_vendor_FOUND AND gmock_vendor_BASE_DIR)
        list(INSERT _search_path_include 0 "${gmock_vendor_BASE_DIR}/include/gmock")
        list(INSERT _search_path_src 0 "${gmock_vendor_BASE_DIR}/src")
      endif()

      find_file(_gmock_header_file "gmock.h"
        PATHS ${_search_path_include}
        NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH
      )
      find_file(_gmock_src_file
        "gmock.cc"
        "gmock-all.cc"
        PATHS ${_search_path_src}
        NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH
      )

      if(_gmock_header_file AND _gmock_src_file)
        # set from-source variables
        set(GMOCK_FROM_SOURCE_FOUND TRUE CACHE INTERNAL "")

        get_filename_component(_gmock_base_dir "${_gmock_src_file}" PATH)
        get_filename_component(_gmock_base_dir "${_gmock_base_dir}" PATH)
        set(GMOCK_FROM_SOURCE_BASE_DIR "${_gmock_base_dir}" CACHE INTERNAL "")

        get_filename_component(_gmock_include_dir "${_gmock_header_file}" PATH)
        get_filename_component(_gmock_include_dir ${_gmock_include_dir} PATH)
        set(GMOCK_FROM_SOURCE_INCLUDE_DIRS "${_gmock_include_dir}" CACHE INTERNAL "")

        set(GMOCK_FROM_SOURCE_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/gmock" CACHE INTERNAL "")

        set(GMOCK_FROM_SOURCE_LIBRARIES "gmock" CACHE INTERNAL "")
        set(GMOCK_FROM_SOURCE_MAIN_LIBRARIES "gmock_main" CACHE INTERNAL "")
      endif()
    endif()

    if(GMOCK_FROM_SOURCE_FOUND)
      message(STATUS "Found gmock sources under '${GMOCK_FROM_SOURCE_BASE_DIR}': "
        "C++ tests using 'Google Mock' will be built")

      # if gmock is already a target, do not add again
      # this can happen when ament_add_gmock() is called from a subdirectory
      if(NOT TARGET gmock)
        # add CMakeLists.txt from gmock dir
        add_subdirectory("${GMOCK_FROM_SOURCE_BASE_DIR}" "${CMAKE_BINARY_DIR}/gmock")

        # mark gmock targets with EXCLUDE_FROM_ALL to only build
        # when tests are built which depend on them
        set_target_properties(gmock gmock_main PROPERTIES EXCLUDE_FROM_ALL 1)
      endif()

      # set the same variables as find_package() would set
      # but do NOT set GMOCK_FOUND in the cache when using gmock from source
      # since the subdirectory must always be added to add the gmock targets
      set(GMOCK_FOUND ${GMOCK_FROM_SOURCE_FOUND})
      set(GMOCK_INCLUDE_DIRS ${GMOCK_FROM_SOURCE_INCLUDE_DIRS})
      set(GMOCK_LIBRARY_DIRS ${GMOCK_FROM_SOURCE_LIBRARY_DIRS})
      set(GMOCK_LIBRARIES ${GMOCK_FROM_SOURCE_LIBRARIES})
      set(GMOCK_MAIN_LIBRARIES ${GMOCK_FROM_SOURCE_MAIN_LIBRARIES})

      if(GMOCK_FROM_SOURCE_BASE_DIR STREQUAL gmock_vendor_BASE_DIR)
        # the GMock headers require the GTest headers
        find_package(ament_cmake_gtest REQUIRED)
        ament_find_gtest()
        list(APPEND GMOCK_INCLUDE_DIRS ${GTEST_INCLUDE_DIRS})
      endif()
    endif()

    if(NOT GMOCK_FOUND)
      message(WARNING
        "'gmock' not found, C++ tests using 'Google Mock' can not be built. "
        "Please install the 'Google Mock' headers globally in your system to "
        "enable these tests (e.g. on Ubuntu/Debian install the package "
          "'google-mock') or get the ament package 'gmock_vendor'")
    endif()
  endif()
endmacro()

include("${ament_cmake_gmock_DIR}/ament_add_gmock.cmake")
include("${ament_cmake_gmock_DIR}/ament_find_gmock.cmake")
