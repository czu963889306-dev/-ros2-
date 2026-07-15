project(gtest CXX C)
cmake_minimum_required(VERSION 3.8)

include_directories(
  include
  .  # to find the source files included with "src/gtest*.cc"
)

add_library(gtest STATIC src/gtest-all.cc)
target_compile_features(gtest PUBLIC cxx_std_11)  # Require C++11

# When building with asan (i.e. using colcon build --mixin asan-gcc),
# asan itself provides a "fake" pthread that tricks the pthread
# detection logic into thinking no link libraries are necessary
# (see https://wiki.gentoo.org/wiki/AddressSanitizer/Problems#pthread_linking_issues
# for some additional information).  To work around that, we unconditionally
# add the -pthread flag for Linux machines so it will always work
if(UNIX AND NOT APPLE)
  target_link_libraries(gtest "-pthread")
endif()

if(NOT WIN32)
  set_target_properties(gtest PROPERTIES COMPILE_FLAGS "-Wno-missing-field-initializers")
  set_target_properties(gtest PROPERTIES POSITION_INDEPENDENT_CODE True)
endif()

add_library(gtest_main STATIC src/gtest_main.cc)
target_compile_features(gtest_main PUBLIC cxx_std_11)  # Require C++11
