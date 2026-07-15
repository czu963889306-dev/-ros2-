// Copyright 2020 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>
#include "rcutils/error_handling.h"
#include "rcutils/testing/fault_injection.h"
#include "rcpputils/shared_library.hpp"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/identifier.hpp"
#include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"

constexpr size_t map_size = 4u;
constexpr const char package_name[] = "rosidl_typesupport_cpp";
constexpr const char * identifiers[map_size] = {
  "test_type_support1", "test_type_support2", "test_type_support3", "test_type_support4"
};

constexpr const char * symbols[map_size] = {
  "test_message_type_support",
  "test_message_type_support2",
  "test_message_type_support3",
  "test_message_type_support4"
};

rosidl_message_type_support_t get_rosidl_message_type_support(const char * identifier)
{
  return {identifier, nullptr, nullptr};
}

type_support_map_t get_typesupport_map(void ** library_array)
{
  return type_support_map_t{
    map_size,
    package_name,
    identifiers,
    symbols,
    library_array,
  };
}

TEST(TestMessageTypeSupportDispatch, get_handle_function) {
  // Both arguments are nullptrs
  EXPECT_DEATH(
    rosidl_typesupport_cpp::get_message_typesupport_handle_function(nullptr, nullptr), "");

  // Handle is a nullptr
  EXPECT_DEATH(
    rosidl_typesupport_cpp::get_message_typesupport_handle_function(
      nullptr,
      "identifier"), "");

  // Handle's identifier is a nullptr
  rosidl_message_type_support_t null_type_support = get_rosidl_message_type_support(nullptr);
  EXPECT_DEATH(
    rosidl_typesupport_cpp::get_message_typesupport_handle_function(
      &null_type_support,
      "identifier"), "");

  // Identifier is a nullptr
  rosidl_message_type_support_t type_support = get_rosidl_message_type_support("identifier");
  EXPECT_DEATH(
    rosidl_typesupport_cpp::get_message_typesupport_handle_function(
      &type_support,
      nullptr), "");

  // Identifier matches this handle's identifier
  EXPECT_EQ(
    rosidl_typesupport_cpp::get_message_typesupport_handle_function(
      &type_support,
      "identifier"), &type_support);

  // Identifier is not the same and isn't the rosidl_typesupport_cpp__typesupport_identifier
  EXPECT_EQ(
    rosidl_typesupport_cpp::get_message_typesupport_handle_function(
      &type_support,
      "different_identifier"), nullptr);
  EXPECT_TRUE(rcutils_error_is_set());
  rcutils_reset_error();

  rosidl_message_type_support_t type_support_cpp_identifier =
    get_rosidl_message_type_support(rosidl_typesupport_cpp::typesupport_identifier);
  rcpputils::SharedLibrary * library_array[map_size] = {nullptr, nullptr, nullptr};
  type_support_map_t support_map = get_typesupport_map(reinterpret_cast<void **>(&library_array));
  type_support_cpp_identifier.data = &support_map;

  // Successfully load library and find symbols
  auto * result = rosidl_typesupport_cpp::get_message_typesupport_handle_function(
    &type_support_cpp_identifier,
    "test_type_support1");
  ASSERT_NE(result, nullptr);
  ASSERT_NE(support_map.data[0], nullptr);
  auto * clib = static_cast<const rcpputils::SharedLibrary *>(support_map.data[0]);
  auto * lib = const_cast<rcpputils::SharedLibrary *>(clib);
  ASSERT_NE(lib, nullptr);

  EXPECT_TRUE(lib->has_symbol("test_message_type_support"));
  auto * sym = lib->get_symbol("test_message_type_support");
  ASSERT_NE(sym, nullptr);
  if (library_array[0] != nullptr) {
    delete library_array[0];
  }

  // Loads library, but symbol doesn't exist
  EXPECT_EQ(
    rosidl_typesupport_cpp::get_message_typesupport_handle_function(
      &type_support_cpp_identifier,
      "test_type_support2"), nullptr);
  EXPECT_TRUE(rcutils_error_is_set());
  rcutils_reset_error();

  // Library file exists, but loading shared library fails
  EXPECT_EQ(
    rosidl_typesupport_cpp::get_message_typesupport_handle_function(
      &type_support_cpp_identifier,
      "test_type_support3"), nullptr);
  EXPECT_TRUE(rcutils_error_is_set());
  rcutils_reset_error();

  // Library doesn't exist
  EXPECT_EQ(
    rosidl_typesupport_cpp::get_message_typesupport_handle_function(
      &type_support_cpp_identifier,
      "test_type_support4"), nullptr);
  EXPECT_TRUE(rcutils_error_is_set());
  rcutils_reset_error();
}

TEST(TestMessageTypeSupportDispatch, get_message_typesupport_maybe_fail_test)
{
  rosidl_message_type_support_t type_support_cpp_identifier =
    get_rosidl_message_type_support(rosidl_typesupport_cpp::typesupport_identifier);
  rcpputils::SharedLibrary * library_array[map_size] = {nullptr, nullptr, nullptr};
  type_support_map_t support_map = get_typesupport_map(reinterpret_cast<void **>(&library_array));
  type_support_cpp_identifier.data = &support_map;

  RCUTILS_FAULT_INJECTION_TEST(
  {
    // load library and find symbols
    auto * result = rosidl_typesupport_cpp::get_message_typesupport_handle_function(
      &type_support_cpp_identifier,
      "test_type_support1");
    if (nullptr == result) {
      EXPECT_TRUE(rcutils_error_is_set());
      rcutils_reset_error();
    }
  });
}
