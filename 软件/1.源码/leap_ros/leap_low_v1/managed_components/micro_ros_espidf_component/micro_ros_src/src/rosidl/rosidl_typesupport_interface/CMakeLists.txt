cmake_minimum_required(VERSION 3.5)

project(rosidl_typesupport_interface NONE)

find_package(ament_cmake REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")

# Export modern CMake targets
ament_export_targets(${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  # For gtest
  enable_language(C CXX)
  # Default to C11
  if(NOT CMAKE_C_STANDARD)
    set(CMAKE_C_STANDARD 11)
  endif()
  # Default to C++17
  if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
  endif()

  find_package(ament_cmake_gtest REQUIRED)

  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
  endif()

  ament_add_gtest(test_macros test/test_macros.cpp)
  if(TARGET test_macros)
    target_link_libraries(test_macros ${PROJECT_NAME})
  endif()

endif()

ament_package()

install(
  DIRECTORY include/
  DESTINATION include/${PROJECT_NAME}
)
install(
  TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
)
