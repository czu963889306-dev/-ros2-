cmake_minimum_required(VERSION 3.5)
project(tracetools)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(/W4)
endif()

find_package(ament_cmake_ros REQUIRED)

if(WIN32)
  set(DISABLED_DEFAULT ON)
  set(STATUS_CHECKING_TOOL_DEFAULT OFF)
else()
  set(DISABLED_DEFAULT OFF)
  set(STATUS_CHECKING_TOOL_DEFAULT ON)
endif()
option(TRACETOOLS_DISABLED "Explicitly disable support for tracing" ${DISABLED_DEFAULT})
option(TRACETOOLS_NO_RDYNAMIC "Disable export of -rdynamic link flag" OFF)
option(TRACETOOLS_STATUS_CHECKING_TOOL "Enable the status checking tool" ${STATUS_CHECKING_TOOL_DEFAULT})

if(NOT TRACETOOLS_DISABLED)
  # Set TRACETOOLS_LTTNG_ENABLED if we can find lttng-ust
  find_package(PkgConfig)
  if(PkgConfig_FOUND)
    pkg_check_modules(LTTNG lttng-ust)
    if(LTTNG_FOUND)
      set(TRACETOOLS_LTTNG_ENABLED TRUE)
      message("LTTng found: tracing enabled")
    endif()
  endif()
endif()

# Store configuration variables for runtime use
#   TRACETOOLS_DISABLED
#   TRACETOOLS_LTTNG_ENABLED
configure_file(include/${PROJECT_NAME}/config.h.in include/${PROJECT_NAME}/config.h)

# Tracetools lib
set(SOURCES
  src/tracetools.c
  src/utils.cpp
)
set(HEADERS
  include/${PROJECT_NAME}/tracetools.h
  include/${PROJECT_NAME}/utils.hpp
  include/${PROJECT_NAME}/visibility_control.hpp
)
if(TRACETOOLS_LTTNG_ENABLED)
  # We only need these if we're using LTTng
  list(APPEND SOURCES
    src/tp_call.c
  )
  list(APPEND HEADERS
    include/${PROJECT_NAME}/tp_call.h
  )
endif()

if(NOT TRACETOOLS_DISABLED)
  add_library(${PROJECT_NAME} ${SOURCES})
else()
  # only export a target with headers when disabled
  add_library(${PROJECT_NAME} INTERFACE)
endif()
if(TRACETOOLS_LTTNG_ENABLED)
  target_link_libraries(${PROJECT_NAME} ${LTTNG_LIBRARIES})
  # Export -rdynamic for downtream packages to use when calling
  #   ament_target_dependencies()
  # which is needed to resolve function addresses to symbols when
  # using function pointers directly/without std::bind()
  # (the flag should not be used on Windows, but TRACETOOLS_LTTNG_ENABLED
  # should never be true on Windows anyway, so there is no need to check)
  if(NOT TRACETOOLS_NO_RDYNAMIC)
    target_link_libraries(${PROJECT_NAME} "-rdynamic")
  endif()
endif()

if(NOT TRACETOOLS_DISABLED)
  # Only use output/binary include directory
  target_include_directories(${PROJECT_NAME} PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
    "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>"
  )
else()
  target_include_directories(${PROJECT_NAME} INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>"
    "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>"
  )
endif()

set(export_target_options)
if(NOT TRACETOOLS_DISABLED)
  set(export_target_options "HAS_LIBRARY_TARGET")
endif()
ament_export_targets(${PROJECT_NAME}_export ${export_target_options})

if(TRACETOOLS_STATUS_CHECKING_TOOL)
  # Lib for status checking
  add_library(${PROJECT_NAME}_status
    src/status.c
  )
  target_link_libraries(${PROJECT_NAME}_status
    ${PROJECT_NAME}
  )
  install(TARGETS
    ${PROJECT_NAME}_status
    DESTINATION lib
  )
  list(APPEND HEADERS
    include/${PROJECT_NAME}/status.h
  )
  # Status checking tool
  add_executable(status
    src/status_tool.c
  )
  target_link_libraries(status
    ${PROJECT_NAME}
    ${PROJECT_NAME}_status
  )
  install(TARGETS
    status
    DESTINATION lib/${PROJECT_NAME}
  )
endif()

# Copy select headers to the actual include/ directory that we will use and export
foreach(_header ${HEADERS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_header}
    ${PROJECT_BINARY_DIR}/${_header}
    COPYONLY
  )
endforeach()

install(
  DIRECTORY ${PROJECT_BINARY_DIR}/include/
  DESTINATION include/${PROJECT_NAME}
)
install(
  TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}_export
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
)

# Export old-style CMake variables
ament_export_include_directories("include/${PROJECT_NAME}")

if(TRACETOOLS_LTTNG_ENABLED)
  ament_export_libraries(${PROJECT_NAME} ${LTTNG_LIBRARIES})
  # Export -rdynamic for downstream packages using classic CMake variables
  if(NOT TRACETOOLS_NO_RDYNAMIC)
    ament_export_link_flags("-rdynamic")
  endif()
elseif(NOT TRACETOOLS_DISABLED)
  ament_export_libraries(${PROJECT_NAME})
endif()

if(BUILD_TESTING)
  set(ament_cmake_cppcheck_ADDITIONAL_INCLUDE_DIRS ${LTTNG_INCLUDE_DIRS})
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()

  if(TRACETOOLS_STATUS_CHECKING_TOOL)
    find_package(ament_cmake_gtest REQUIRED)
    ament_add_gtest(test_status test/test_status.cpp)
    if(TARGET test_status)
      target_link_libraries(test_status ${PROJECT_NAME} ${PROJECT_NAME}_status)
    endif()

    # Run status tool executable as test and set pass/fail expectation appropriately
    add_test(test_status_tool status)
    if(NOT TRACETOOLS_LTTNG_ENABLED)
      set_tests_properties(test_status_tool PROPERTIES WILL_FAIL TRUE)
    endif()
  endif()
endif()

ament_package()

if(NOT TRACETOOLS_DISABLED)
  target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_VERSION="${${PROJECT_NAME}_VERSION}")
endif()
