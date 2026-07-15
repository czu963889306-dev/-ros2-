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

# copied from ament_cmake_gtest/ament_cmake_gtest-extras.cmake

# find gtest and create library targets once
macro(_ament_cmake_gtest_find_gtest)
  if(NOT DEFINED _AMENT_CMAKE_GTEST_FIND_GTEST)
    set(_AMENT_CMAKE_GTEST_FIND_GTEST TRUE)

    find_package(ament_cmake_test QUIET REQUIRED)

    # if gtest sources were not found in a previous run
    if(NOT GTEST_FROM_SOURCE_FOUND)
      # search path for gtest includes and sources
      set(_search_path_include "")
      set(_search_path_src "")

      # option() consider environment variable to find gtest
      if(NOT $ENV{GTEST_DIR} STREQUAL "")
        list(APPEND _search_path_include "$ENV{GTEST_DIR}/include/gtest")
        list(APPEND _search_path_src "$ENV{GTEST_DIR}/src")
      endif()

      # check to system installed path (i.e. on Ubuntu)
      set(_search_path_include "/usr/include/gtest")
      set(_search_path_src "/usr/src/gtest/src")

      # check gtest_vendor path, prefer this version over a system installed
      find_package(gtest_vendor QUIET)
      if(gtest_vendor_FOUND AND gtest_vendor_BASE_DIR)
        list(INSERT _search_path_include 0 "${gtest_vendor_BASE_DIR}/include/gtest")
        list(INSERT _search_path_src 0 "${gtest_vendor_BASE_DIR}/src")
      endif()

      find_file(_gtest_header_file "gtest.h"
        PATHS ${_search_path_include}
        NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH
      )
      find_file(_gtest_src_file
        "gtest.cc"
        "gtest-all.cc"
        PATHS ${_search_path_src}
        NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH
      )

      if(_gtest_header_file AND _gtest_src_file)
        # set from-source variables
        set(GTEST_FROM_SOURCE_FOUND TRUE CACHE INTERNAL "")

        get_filename_component(_gtest_base_dir "${_gtest_src_file}" PATH)
        get_filename_component(_gtest_base_dir "${_gtest_base_dir}" PATH)
        set(GTEST_FROM_SOURCE_BASE_DIR "${_gtest_base_dir}" CACHE INTERNAL "")

        get_filename_component(_gtest_include_dir "${_gtest_header_file}" PATH)
        get_filename_component(_gtest_include_dir ${_gtest_include_dir} PATH)
        set(GTEST_FROM_SOURCE_INCLUDE_DIRS "${_gtest_include_dir}" CACHE INTERNAL "")

        set(GTEST_FROM_SOURCE_LIBRARY_DIRS "${CMAKE_BINARY_DIR}/gtest" CACHE INTERNAL "")

        set(GTEST_FROM_SOURCE_LIBRARIES "gtest" CACHE INTERNAL "")
        set(GTEST_FROM_SOURCE_MAIN_LIBRARIES "gtest_main" CACHE INTERNAL "")
      endif()
    endif()

    if(GTEST_FROM_SOURCE_FOUND)
      message(STATUS "Found gtest sources under '${GTEST_FROM_SOURCE_BASE_DIR}': "
        "C++ tests using 'Google Test' will be built")

      # if the gtest sources are from a subfolder of gmock (not the case for gmock_vendor)
      # the subdirectory was already added by gmock
      if(NOT TARGET gtest)
        # add CMakeLists.txt from gtest dir
        add_subdirectory("${GTEST_FROM_SOURCE_BASE_DIR}" "${CMAKE_BINARY_DIR}/gtest")

        # mark gtest targets with EXCLUDE_FROM_ALL to only build
        # when tests are built which depend on them
        set_target_properties(gtest gtest_main PROPERTIES EXCLUDE_FROM_ALL 1)
        if(NOT WIN32)
          target_compile_options(gtest PRIVATE -Wno-null-dereference)
        endif()
        target_include_directories(gtest SYSTEM BEFORE PUBLIC "${GTEST_FROM_SOURCE_INCLUDE_DIRS}")
        target_include_directories(gtest_main SYSTEM BEFORE PUBLIC "${GTEST_FROM_SOURCE_INCLUDE_DIRS}")
      endif()

      # set the same variables as find_package()
      # but do NOT set GTEST_FOUND in the cache when using gtest from source
      # since the subdirectory must always be added to add the gtest targets
      set(GTEST_FOUND ${GTEST_FROM_SOURCE_FOUND})
      set(GTEST_INCLUDE_DIRS ${GTEST_FROM_SOURCE_INCLUDE_DIRS})
      set(GTEST_LIBRARY_DIRS ${GTEST_FROM_SOURCE_LIBRARY_DIRS})
      set(GTEST_LIBRARIES ${GTEST_FROM_SOURCE_LIBRARIES})
      set(GTEST_MAIN_LIBRARIES ${GTEST_FROM_SOURCE_MAIN_LIBRARIES})
      set(GTEST_BOTH_LIBRARIES ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
    endif()

    if(NOT GTEST_FOUND)
      message(WARNING
        "'gtest' not found, C++ tests using 'Google Test' can not be built. "
        "Please install the 'Google Test' headers globally in your system "
        "to enable these tests (e.g. on Ubuntu/Debian install the package "
        "'libgtest-dev') or get the ament package 'gtest_vendor'")
    endif()
  endif()
endmacro()

include("${ament_cmake_gtest_DIR}/ament_add_gtest.cmake")
include("${ament_cmake_gtest_DIR}/ament_add_gtest_executable.cmake")
include("${ament_cmake_gtest_DIR}/ament_add_gtest_test.cmake")
include("${ament_cmake_gtest_DIR}/ament_find_gtest.cmake")
