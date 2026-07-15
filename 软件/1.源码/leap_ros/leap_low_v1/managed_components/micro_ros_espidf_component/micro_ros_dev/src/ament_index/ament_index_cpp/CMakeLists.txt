cmake_minimum_required(VERSION 3.5)

project(ament_index_cpp)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-std=c++14 -Wall -Wextra -Wpedantic)
endif()

option(
  BUILD_SHARED_LIBS
  "Global flag to cause add_library() to create shared libraries if on. \
If set to true, this will cause all libraries to be built shared \
unless the library was explicitly added as a static library."
  ON)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_gen_version_h REQUIRED)

add_library(${PROJECT_NAME}
  src/get_package_prefix.cpp
  src/get_package_share_directory.cpp
  src/get_packages_with_prefixes.cpp
  src/get_resource.cpp
  src/get_resources.cpp
  src/get_search_paths.cpp
  src/has_resource.cpp
)
target_compile_definitions(${PROJECT_NAME} PRIVATE "AMENT_INDEX_CPP_BUILDING_DLL")
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  ament_add_gtest(${PROJECT_NAME}_utest test/utest.cpp)
  if(TARGET ${PROJECT_NAME}_utest)
    target_include_directories(${PROJECT_NAME}_utest PUBLIC include)
    target_link_libraries(${PROJECT_NAME}_utest ${PROJECT_NAME})
  endif()
endif()

ament_package()
ament_generate_version_header(ament_index_cpp)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME}
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
