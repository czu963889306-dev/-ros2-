cmake_minimum_required(VERSION 3.15)
project(ament_cmake_vendor_package_test)

find_package(ament_cmake_test REQUIRED)

set(_${PROJECT_NAME}_AMENT_GENERATE_PACKAGE_ENVIRONMENT FALSE)

set(ament_cmake_vendor_package_DIR "${CMAKE_CURRENT_LIST_DIR}/../cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../ament_cmake_vendor_package-extras.cmake")

ament_vendor(exlib_bad
  VCS_URL exlib_bad
  VCS_TYPE path
  SKIP_INSTALL
)

ament_vendor(exlib_good
  VCS_URL exlib_good
  VCS_TYPE path
  SKIP_INSTALL
)

ament_vendor(depender
  VCS_URL depender
  VCS_TYPE path
  SKIP_INSTALL
)

externalproject_add_stepdependencies(depender configure exlib_bad exlib_good)

if(WIN32)
  set(INSTALL_LIBDIR bin)
else()
  set(INSTALL_LIBDIR lib)
endif()

externalproject_get_property(exlib_bad INSTALL_DIR)
set(exlib_bad_LIBDIR "${INSTALL_DIR}/${INSTALL_LIBDIR}")
externalproject_get_property(exlib_good INSTALL_DIR)
set(exlib_good_LIBDIR "${INSTALL_DIR}/${INSTALL_LIBDIR}")
externalproject_get_property(depender INSTALL_DIR)
set(depender_BINDIR "${INSTALL_DIR}/bin")

ament_add_test(layering_good
  COMMAND "${depender_BINDIR}/depender"
  GENERATE_RESULT_FOR_RETURN_CODE_ZERO
  APPEND_LIBRARY_DIRS "${exlib_good_LIBDIR}" "${exlib_bad_LIBDIR}")

ament_add_test(layering_bad
  COMMAND "${depender_BINDIR}/depender" "should-fail"
  GENERATE_RESULT_FOR_RETURN_CODE_ZERO
  APPEND_LIBRARY_DIRS "${exlib_bad_LIBDIR}" "${exlib_good_LIBDIR}")
