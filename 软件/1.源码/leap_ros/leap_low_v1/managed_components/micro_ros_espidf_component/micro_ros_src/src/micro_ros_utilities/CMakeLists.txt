cmake_minimum_required(VERSION 3.5)
project(micro_ros_utilities)

option(BUILD_DOCUMENTATION "Use doxygen to create package documentation" OFF)

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 11)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

#################################################
# package dependencies
#################################################
find_package(ament_cmake REQUIRED)
find_package(rosidl_typesupport_introspection_c REQUIRED)
find_package(rosidl_runtime_c REQUIRED)
find_package(rcutils REQUIRED)

#################################################
# create library
#################################################

add_library(${PROJECT_NAME}
  src/string_utilities.c
  src/type_utilities.c
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
)

ament_target_dependencies(${PROJECT_NAME}
  rosidl_typesupport_introspection_c
  rosidl_runtime_c
  rcutils
)

#################################################
# install
#################################################
install(DIRECTORY include/ DESTINATION include)
install(TARGETS ${PROJECT_NAME} DESTINATION lib)

# specific order: dependents before dependencies
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})

#################################################
# testing
#################################################
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(ament_cmake_gtest REQUIRED)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE) # No copyright header check since link to NOTICE file is not recognized properly.

  find_package(std_msgs REQUIRED)
  find_package(trajectory_msgs REQUIRED)

  ament_lint_auto_find_test_dependencies()

  add_compile_options(-Werror)

  ament_add_gtest(${PROJECT_NAME}_test
    test/micro_ros_utilities/test_memory.cpp
    test/micro_ros_utilities/test_string.cpp
  )

  target_include_directories(${PROJECT_NAME}_test PRIVATE include src)
  target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME})
  ament_target_dependencies(${PROJECT_NAME}_test
    rosidl_typesupport_introspection_c
    rosidl_generator_c
    rosidl_runtime_c
    rcutils
    std_msgs
    trajectory_msgs
  )
endif()

#################################################
# documentation
#################################################
if(BUILD_DOCUMENTATION)
    find_package(Doxygen)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "doxygen is needed to build the documentation. Please install it correctly")
    endif()
    if(UNIX)
        find_program(DOXYFILE_MAKE make)
        if(DOXYFILE_MAKE)
            message(STATUS "Found Make: ${DOXYFILE_MAKE}")
        else()
            message(FATAL_ERROR "make is needed to build the documentation. Please install it correctly")
        endif()
    elseif(WIN32)
        set(DOXYFILE_MAKE make.bat)
    endif()

    # Target to create documentation directories
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/doc)
    add_custom_target(docdirs
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/doc/api_reference
        COMMENT "Creating documentation directory" VERBATIM)

    ### Doxygen ########################3
    # Configure the template doxyfile for or specific project
    configure_file(doxyfile.in ${PROJECT_BINARY_DIR}/doxyfile @ONLY IMMEDIATE)
    # Add custom target to run doxygen when ever the project is build
    add_custom_target(doxygen
        COMMAND "${DOXYGEN_EXECUTABLE}" "${PROJECT_BINARY_DIR}/doxyfile"
        SOURCES "${PROJECT_BINARY_DIR}/doxyfile"
        COMMENT "Generating API documentation with doxygen" VERBATIM)

    add_dependencies(doxygen docdirs)

    add_custom_target(doc ALL
    COMMENT "Generated project documentation" VERBATIM)

    add_dependencies(doc doxygen)
endif()

#################################################
# export dependencies
#################################################
# specific order: dependents before dependencies
ament_export_include_directories(include)
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(rcutils)
ament_export_dependencies(rosidl_runtime_c)

ament_package()
