cmake_minimum_required(VERSION 3.5)
project(ament_cmake_gen_version_h)
find_package(ament_cmake_core REQUIRED)

# GTest needs it, Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()
# GTest needs it, Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

include(CTest)
if(BUILD_TESTING)
  # Simulate pre-installed package
  set(ament_cmake_gen_version_h_DIR ${CMAKE_SOURCE_DIR}/cmake)
  include(cmake/ament_cmake_gen_version_h.cmake)
  include(cmake/ament_generate_version_header.cmake)
  find_package(ament_cmake_gtest REQUIRED)

  if(DEFINED CMAKE_WARN_DEPRECATED)
    set(old_deprecation_value ${CMAKE_WARN_DEPRECATED})
  endif()
  set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

  # Generate version headers using different scenarios
  ament_cmake_gen_version_h(NO_INSTALL)
  ament_cmake_gen_version_h(NO_INSTALL VERSION_FILE_NAME "version1.h")
  ament_cmake_gen_version_h(NO_INSTALL INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include VERSION_FILE_NAME "version2.h")
  ament_cmake_gen_version_h(
    NO_INSTALL TRUE
    INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include
    VERSION_FILE_NAME "version_custom.h"
    VERSION_MAJOR 1
    VERSION_MINOR 2
    VERSION_PATCH 3
  )
  if(old_deprecation_value)
    set(CMAKE_WARN_DEPRECATED ${old_deprecation_value} CACHE BOOL "" FORCE)
  else()
    unset(CMAKE_WARN_DEPRECATED CACHE)
  endif()

  ament_add_gtest(test_${PROJECT_NAME}
    test/test_version_custom.cpp
    test/test_version_h.cpp
    test/test_version1_h.cpp
    test/test_version2_h.cpp
  )

  # Generate version headers that don't conflict with existing tests
  add_library(some_lib INTERFACE)
  ament_generate_version_header(some_lib SKIP_INSTALL
    HEADER_PATH "ament_generate_version_header/version.hpp")
  ament_add_gtest(test_ament_generate_version_header
    test/test_version_hpp.cpp
  )
  target_link_libraries(test_ament_generate_version_header some_lib)
endif()

ament_package(CONFIG_EXTRAS "ament_cmake_gen_version_h-extras.cmake")

install(
  DIRECTORY cmake
  DESTINATION share/${PROJECT_NAME}
)
