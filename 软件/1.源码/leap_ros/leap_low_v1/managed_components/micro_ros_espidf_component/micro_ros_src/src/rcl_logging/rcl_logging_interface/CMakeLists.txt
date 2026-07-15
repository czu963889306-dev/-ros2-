cmake_minimum_required(VERSION 3.5)

project(rcl_logging_interface)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()
# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(NOT WIN32)
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake_ros REQUIRED)
find_package(rcutils REQUIRED)

set(${PROJECT_NAME}_sources
  "src/logging_dir.c"
)
add_library(${PROJECT_NAME} ${${PROJECT_NAME}_sources})
target_include_directories(${PROJECT_NAME} PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")
ament_target_dependencies(${PROJECT_NAME} rcutils)
target_compile_definitions(${PROJECT_NAME} PRIVATE "RCL_LOGGING_INTERFACE_BUILDING_DLL")

install(
  DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME})

ament_export_dependencies(rcutils)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(ament_cmake_gtest REQUIRED)
  find_package(rcpputils REQUIRED)
  ament_add_gtest(test_get_logging_directory test/test_get_logging_directory.cpp)
  if(TARGET test_get_logging_directory)
    target_link_libraries(test_get_logging_directory ${PROJECT_NAME})
    ament_target_dependencies(test_get_logging_directory rcutils rcpputils)
  endif()
endif()

ament_package()
