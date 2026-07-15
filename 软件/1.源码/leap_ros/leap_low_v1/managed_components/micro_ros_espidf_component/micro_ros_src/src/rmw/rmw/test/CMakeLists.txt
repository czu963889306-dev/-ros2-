find_package(ament_cmake_gmock REQUIRED)
find_package(osrf_testing_tools_cpp REQUIRED)


ament_add_gmock(test_allocators
  test_allocators.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_allocators)
  target_link_libraries(test_allocators ${PROJECT_NAME})
endif()

ament_add_gmock(test_convert_rcutils_ret_to_rmw_ret
  test_convert_rcutils_ret_to_rmw_ret.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_convert_rcutils_ret_to_rmw_ret)
  target_link_libraries(test_convert_rcutils_ret_to_rmw_ret ${PROJECT_NAME})
endif()

ament_add_gmock(test_event
  test_event.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_event)
  target_link_libraries(test_event ${PROJECT_NAME})
endif()

ament_add_gmock(test_init_options
  test_init_options.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_init_options)
  target_link_libraries(test_init_options ${PROJECT_NAME})
endif()

ament_add_gmock(test_init
  test_init.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_init)
  target_link_libraries(test_init ${PROJECT_NAME})
endif()

ament_add_gmock(test_message_sequence
  test_message_sequence.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_message_sequence)
  target_link_libraries(test_message_sequence ${PROJECT_NAME})
endif()

ament_add_gmock(test_names_and_types
  test_names_and_types.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_names_and_types)
  target_link_libraries(test_names_and_types ${PROJECT_NAME}
                        osrf_testing_tools_cpp::memory_tools)
endif()

ament_add_gmock(test_publisher_options
  test_publisher_options.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_publisher_options)
  target_link_libraries(test_publisher_options ${PROJECT_NAME})
endif()

ament_add_gtest(test_qos_string_conversions
test_qos_string_conversions.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_qos_string_conversions)
  target_link_libraries(test_qos_string_conversions ${PROJECT_NAME})
endif()

ament_add_gmock(test_sanity_checks
  test_sanity_checks.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_sanity_checks)
  target_link_libraries(test_sanity_checks ${PROJECT_NAME})
endif()

ament_add_gmock(test_security_options
  test_security_options.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_security_options)
  target_link_libraries(test_security_options ${PROJECT_NAME})
endif()

ament_add_gmock(test_serialized_message
  test_serialized_message.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_serialized_message)
  target_link_libraries(test_serialized_message ${PROJECT_NAME})
  target_link_libraries(test_serialized_message osrf_testing_tools_cpp::memory_tools)
endif()

ament_add_gmock(test_subscription_options
  test_subscription_options.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_subscription_options)
  target_link_libraries(test_subscription_options ${PROJECT_NAME})
endif()

ament_add_gmock(test_time
  test_time.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_time)
  target_link_libraries(test_time ${PROJECT_NAME})
endif()

ament_add_gmock(test_types
  test_types.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_types)
  target_link_libraries(test_types ${PROJECT_NAME})
endif()

ament_add_gmock(test_validate_full_topic_name
  test_validate_full_topic_name.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_validate_full_topic_name)
  target_link_libraries(test_validate_full_topic_name ${PROJECT_NAME})
endif()

ament_add_gmock(test_validate_node_name
  test_validate_node_name.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_validate_node_name)
  target_link_libraries(test_validate_node_name ${PROJECT_NAME})
endif()

ament_add_gmock(test_validate_namespace
  test_validate_namespace.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_validate_namespace)
  target_link_libraries(test_validate_namespace ${PROJECT_NAME})
  if(UNIX AND NOT APPLE AND NOT ANDROID)
    target_link_libraries(test_validate_namespace pthread)
  endif()
  if(NOT WIN32)
    set_target_properties(test_validate_namespace PROPERTIES COMPILE_FLAGS "-std=c++14")
  endif()
endif()

ament_add_gmock(test_topic_endpoint_info_array
  test_topic_endpoint_info_array.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_topic_endpoint_info_array)
  target_link_libraries(test_topic_endpoint_info_array ${PROJECT_NAME}
  osrf_testing_tools_cpp::memory_tools)
endif()

ament_add_gmock(test_topic_endpoint_info
  test_topic_endpoint_info.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_topic_endpoint_info)
  target_link_libraries(test_topic_endpoint_info ${PROJECT_NAME}
  osrf_testing_tools_cpp::memory_tools)
endif()

ament_add_gmock(test_network_flow_endpoint
  test_network_flow_endpoint.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_network_flow_endpoint)
  target_link_libraries(test_network_flow_endpoint ${PROJECT_NAME}
  osrf_testing_tools_cpp::memory_tools)
endif()

ament_add_gmock(test_network_flow_endpoint_array
  test_network_flow_endpoint_array.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_network_flow_endpoint_array)
  target_link_libraries(test_network_flow_endpoint_array ${PROJECT_NAME}
  osrf_testing_tools_cpp::memory_tools)
endif()

ament_add_gmock(test_subscription_content_filter_options
  test_subscription_content_filter_options.cpp
  # Append the directory of librmw so it is found at test time.
  APPEND_LIBRARY_DIRS "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)
if(TARGET test_subscription_content_filter_options)
  target_link_libraries(test_subscription_content_filter_options ${PROJECT_NAME})
endif()
