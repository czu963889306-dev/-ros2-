cmake_minimum_required(VERSION 3.5)

project(rcl_logging_spdlog)

# Default to C11
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()
# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

find_package(ament_cmake_ros REQUIRED)
find_package(rcl_logging_interface REQUIRED)
find_package(rcpputils REQUIRED)
find_package(rcutils REQUIRED)
find_package(spdlog_vendor REQUIRED) # Provides spdlog on platforms without it.
find_package(spdlog REQUIRED)

if(NOT WIN32)
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(
    -Wformat=2 -Wconversion -Woverloaded-virtual -Wshadow
    -Wnon-virtual-dtor -Wold-style-cast -Wcast-qual
  )
endif()

add_library(${PROJECT_NAME} src/rcl_logging_spdlog.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE
  rcpputils::rcpputils
  rcutils::rcutils
  spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC
  rcl_logging_interface::rcl_logging_interface)

target_compile_definitions(${PROJECT_NAME} PRIVATE "RCL_LOGGING_INTERFACE_BUILDING_DLL")

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  find_package(performance_test_fixture REQUIRED)
  # Give cppcheck hints about macro definitions coming from outside this package
  get_target_property(ament_cmake_cppcheck_ADDITIONAL_INCLUDE_DIRS
    performance_test_fixture::performance_test_fixture INTERFACE_INCLUDE_DIRECTORIES)

  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gmock(test_logging_interface test/test_logging_interface.cpp)
  if(TARGET test_logging_interface)
    target_link_libraries(test_logging_interface ${PROJECT_NAME})
    target_compile_definitions(test_logging_interface PUBLIC RCUTILS_ENABLE_FAULT_INJECTION)
    ament_target_dependencies(test_logging_interface rcpputils)
  endif()
  add_performance_test(benchmark_logging_interface test/benchmark/benchmark_logging_interface.cpp)
  if(TARGET benchmark_logging_interface)
    target_link_libraries(benchmark_logging_interface ${PROJECT_NAME})
  endif()
endif()

ament_export_dependencies(rcl_logging_interface)
ament_export_dependencies(rcpputils)
ament_export_dependencies(rcutils)
ament_export_dependencies(spdlog_vendor)
ament_export_dependencies(spdlog)
ament_export_libraries(${PROJECT_NAME})
ament_export_targets(${PROJECT_NAME})
ament_package()
