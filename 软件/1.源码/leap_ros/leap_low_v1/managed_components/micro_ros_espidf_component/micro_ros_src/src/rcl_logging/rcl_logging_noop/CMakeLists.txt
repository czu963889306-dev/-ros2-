cmake_minimum_required(VERSION 3.5)

project(rcl_logging_noop)

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
find_package(rcutils REQUIRED)

if(NOT WIN32)
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(${PROJECT_NAME}_sources
  src/rcl_logging_noop/rcl_logging_noop.cpp
)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_sources})

target_link_libraries(${PROJECT_NAME} PRIVATE
  rcutils::rcutils)
target_link_libraries(${PROJECT_NAME} PUBLIC
  rcl_logging_interface::rcl_logging_interface)

target_compile_definitions(${PROJECT_NAME} PRIVATE "RCL_LOGGING_INTERFACE_BUILDING_DLL")

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)


# Export rcl_logging_interface to give downstream packages access to it's headers
ament_export_dependencies(rcl_logging_interface)
ament_export_libraries(${PROJECT_NAME})
ament_export_targets(${PROJECT_NAME})
ament_package()
