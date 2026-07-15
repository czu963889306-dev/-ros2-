cmake_minimum_required(VERSION 3.15)
project(exlib)

if(NOT DEFINED BUILD_SHARED_LIBS)
  set(BUILD_SHARED_LIBS ON)
endif()

add_library(exlib src/exlib.c)
target_include_directories(exlib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
target_compile_definitions(exlib
  PRIVATE "EXLIB_BUILDING_LIBRARY")
install(TARGETS exlib EXPORT exlibTargets
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)

install(EXPORT exlibTargets
  FILE exlibTargets.cmake
  DESTINATION lib/cmake/exlib)
include(CMakePackageConfigHelpers)
configure_package_config_file(exlibConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/exlibConfig.cmake"
  INSTALL_DESTINATION lib/cmake/exlib
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/exlibConfig.cmake"
  DESTINATION lib/cmake/exlib)
