project(gmock CXX C)
cmake_minimum_required(VERSION 3.8)

find_package(gtest_vendor REQUIRED)

include_directories(
  include
  .  # to find the source files included with "src/gmock*.cc"
  ${gtest_vendor_BASE_DIR}/include
  ${gtest_vendor_BASE_DIR}
)

add_library(gmock STATIC
  ${gtest_vendor_BASE_DIR}/src/gtest-all.cc
  src/gmock-all.cc)
target_compile_features(gmock PUBLIC cxx_std_11)  # Require C++11

# When building with asan (i.e. using colcon build --mixin asan-gcc),
# asan itself provides a "fake" pthread that tricks the pthread
# detection logic into thinking no link libraries are necessary
# (see https://wiki.gentoo.org/wiki/AddressSanitizer/Problems#pthread_linking_issues
# for some additional information).  To work around that, we unconditionally
# add the -pthread flag for Linux machines so it will always work
if(UNIX AND NOT APPLE)
  target_link_libraries(gmock "-pthread")
endif()

if(NOT WIN32)
  set_target_properties(gmock PROPERTIES COMPILE_FLAGS "-Wno-missing-field-initializers")
  set_target_properties(gmock PROPERTIES POSITION_INDEPENDENT_CODE True)
endif()

add_library(gmock_main STATIC src/gmock_main.cc)
target_compile_features(gmock_main PUBLIC cxx_std_11)  # Require C++11
