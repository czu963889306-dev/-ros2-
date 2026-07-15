cmake_minimum_required(VERSION 3.12)

project(rosidl_generator_cpp)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

ament_export_dependencies(rosidl_cmake)

ament_index_register_resource("rosidl_generator_packages")

ament_python_install_package(${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  find_package(rosidl_runtime_c REQUIRED)
  find_package(rosidl_runtime_cpp REQUIRED)
  find_package(test_interface_files REQUIRED)
  ament_lint_auto_find_test_dependencies()

  include(cmake/register_cpp.cmake)
  set(rosidl_generator_cpp_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

  rosidl_generator_cpp_extras(
    "${CMAKE_CURRENT_SOURCE_DIR}/bin/rosidl_generator_cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/rosidl_generator_cpp/__init__.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/resource"
  )

  rosidl_generate_interfaces(${PROJECT_NAME}
    ${test_interface_files_MSG_FILES}
    ${test_interface_files_SRV_FILES}
    ADD_LINTER_TESTS
    SKIP_INSTALL
  )

  # Default to C++17
  if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
  endif()

  if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
  endif()

  ament_add_gtest(test_msg_builder test/test_msg_builder.cpp)
  if(TARGET test_msg_builder)
    add_dependencies(test_msg_builder ${PROJECT_NAME})
    target_include_directories(test_msg_builder PUBLIC
      ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})
    ament_target_dependencies(test_msg_builder
      rosidl_runtime_cpp
      rosidl_runtime_c)
  endif()
  ament_add_gtest(test_msg_initialization test/test_msg_initialization.cpp)
  if(TARGET test_msg_initialization)
    add_dependencies(test_msg_initialization ${PROJECT_NAME})
    ament_target_dependencies(test_msg_initialization
      rosidl_runtime_cpp
      rosidl_runtime_c)
    target_include_directories(test_msg_initialization PUBLIC
      ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    )
  endif()
  ament_add_gtest(test_srv_initialization test/test_srv_initialization.cpp)
  if(TARGET test_srv_initialization)
    add_dependencies(test_srv_initialization ${PROJECT_NAME})
    ament_target_dependencies(test_srv_initialization
      rosidl_runtime_cpp
      rosidl_runtime_c)
    target_include_directories(test_srv_initialization PUBLIC
      ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    )
  endif()
  ament_add_gtest(test_interfaces_cpp test/test_interfaces.cpp)
  if(TARGET test_interfaces_cpp)
    add_dependencies(test_interfaces_cpp ${PROJECT_NAME})
    ament_target_dependencies(test_interfaces_cpp
      rosidl_runtime_cpp
      rosidl_runtime_c)
    target_include_directories(test_interfaces_cpp PUBLIC
      ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    )
  endif()
  ament_add_gtest(test_msg_datatype test/test_msg_datatype.cpp)
  if(TARGET test_msg_datatype)
    add_dependencies(test_msg_datatype ${PROJECT_NAME})
    ament_target_dependencies(test_msg_datatype
      rosidl_runtime_cpp
      rosidl_runtime_c)
    target_include_directories(test_msg_datatype PUBLIC
      ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    )
  endif()
  ament_add_gtest(test_name test/test_name.cpp)
  if(TARGET test_name)
    add_dependencies(test_name ${PROJECT_NAME})
    ament_target_dependencies(test_name
      rosidl_runtime_cpp
      rosidl_runtime_c)
    target_include_directories(test_name PUBLIC
      ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    )
  endif()
  if(MSVC)
    # https://developercommunity.visualstudio.com/content/problem/919371/c2017-illegal-escape-sequence-when-using-in-a-raw.html
    set_source_files_properties(
      test/test_traits.cpp PROPERTIES COMPILE_FLAGS "/Zc:preprocessor")
  endif()
  ament_add_gtest(test_traits test/test_traits.cpp)
  if(TARGET test_traits)
    add_dependencies(test_traits ${PROJECT_NAME})
    ament_target_dependencies(test_traits
      rosidl_runtime_cpp
      rosidl_runtime_c)
    target_include_directories(test_traits PUBLIC
      ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}
    )
  endif()
endif()

ament_package(
  CONFIG_EXTRAS "rosidl_generator_cpp-extras.cmake.in"
)

install(
  PROGRAMS bin/rosidl_generator_cpp
  DESTINATION lib/rosidl_generator_cpp
)
install(
  DIRECTORY cmake resource
  DESTINATION share/${PROJECT_NAME}
)
