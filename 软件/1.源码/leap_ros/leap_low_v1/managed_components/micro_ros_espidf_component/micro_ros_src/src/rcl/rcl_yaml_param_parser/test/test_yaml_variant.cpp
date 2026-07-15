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

#include <yaml.h>

#include <string>
#include <vector>

#include "osrf_testing_tools_cpp/scope_exit.hpp"
#include "rcl_yaml_param_parser/parser.h"
#include "../src/impl/yaml_variant.h"
#include "rcutils/allocator.h"
#include "rcutils/strdup.h"
#include "rcutils/testing/fault_injection.h"

#define TEST_VARIANT_COPY(field, tmp_var) \
  do { \
    SCOPED_TRACE("TEST_VARIANT_COPY " #field); \
    rcl_variant_t src_variant{}; \
    rcl_variant_t dest_variant{}; \
    rcutils_allocator_t allocator = rcutils_get_default_allocator(); \
    src_variant.field = &tmp_var; \
    EXPECT_TRUE(rcl_yaml_variant_copy(&dest_variant, &src_variant, allocator)); \
    ASSERT_NE(nullptr, dest_variant.field); \
    EXPECT_EQ(*src_variant.field, *dest_variant.field); \
    rcl_yaml_variant_fini(&dest_variant, allocator); \
    src_variant.field = nullptr; \
  } while (0)

#define TEST_VARIANT_ARRAY_COPY(field, tmp_array) \
  do { \
    SCOPED_TRACE("TEST_VARIANT_ARRAY_COPY " #field); \
    constexpr size_t array_size = sizeof(tmp_array) / sizeof(tmp_array[0]); \
    rcl_variant_t src_variant{}; \
    rcl_variant_t dest_variant{}; \
    rcutils_allocator_t allocator = rcutils_get_default_allocator(); \
    using ArrayT = std::remove_pointer<decltype(src_variant.field)>::type; \
    src_variant.field = \
      static_cast<ArrayT *>(allocator.allocate(sizeof(ArrayT), allocator.state)); \
    ASSERT_NE(nullptr, src_variant.field); \
    using ValueT = std::remove_pointer<decltype(src_variant.field->values)>::type; \
    src_variant.field->values = \
      static_cast<ValueT *>( \
      allocator.zero_allocate(array_size, sizeof(ValueT), allocator.state)); \
    ASSERT_NE(nullptr, src_variant.field->values); \
    OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT( \
    { \
      rcl_yaml_variant_fini(&src_variant, allocator); \
      rcl_yaml_variant_fini(&dest_variant, allocator); \
      src_variant.field = nullptr; \
      dest_variant.field = nullptr; \
    }); \
    src_variant.field->size = array_size; \
    for (size_t i = 0; i < array_size; ++i) { \
      src_variant.field->values[i] = tmp_array[i]; \
    } \
    EXPECT_TRUE(rcl_yaml_variant_copy(&dest_variant, &src_variant, allocator)); \
    ASSERT_NE(nullptr, dest_variant.field); \
    ASSERT_NE(nullptr, dest_variant.field->values); \
    for (size_t i = 0; i < array_size; ++i) { \
      SCOPED_TRACE(i); \
      EXPECT_EQ(src_variant.field->values[i], dest_variant.field->values[i]); \
    } \
  } while (0)

TEST(TestYamlVariant, copy_fini) {
  rcl_variant_t variant{};
  rcl_variant_t copy{};
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  EXPECT_FALSE(rcl_yaml_variant_copy(nullptr, &variant, allocator));
  EXPECT_FALSE(rcl_yaml_variant_copy(&copy, nullptr, allocator));

  ASSERT_TRUE(rcl_yaml_variant_copy(&copy, &variant, allocator));

  rcl_yaml_variant_fini(&copy, allocator);

  // Check second fini works fine
  rcl_yaml_variant_fini(&copy, allocator);

  // Check fini with a nullptr doesn't crash.
  rcl_yaml_variant_fini(nullptr, allocator);
}

TEST(TestYamlVariant, copy_bool_value) {
  bool tmp_bool = true;
  TEST_VARIANT_COPY(bool_value, tmp_bool);
}

TEST(TestYamlVariant, copy_integer_value) {
  int64_t tmp_int = 42;
  TEST_VARIANT_COPY(integer_value, tmp_int);
}

TEST(TestYamlVariant, copy_double_value) {
  double tmp_double = 3.14159;
  TEST_VARIANT_COPY(double_value, tmp_double);
}

TEST(TestYamlVariant, copy_string_value) {
  // String version is slightly different and can't use the above macro
  rcl_variant_t src_variant{};
  rcl_variant_t dest_variant{};
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  char * tmp_string = rcutils_strdup("hello there", allocator);
  ASSERT_STREQ("hello there", tmp_string);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    allocator.deallocate(tmp_string, allocator.state);
  });
  src_variant.string_value = tmp_string;
  EXPECT_TRUE(rcl_yaml_variant_copy(&dest_variant, &src_variant, allocator));
  ASSERT_NE(nullptr, dest_variant.string_value);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_variant_fini(&dest_variant, allocator);
  });
  EXPECT_STREQ(tmp_string, dest_variant.string_value);
}

TEST(TestYamlVariant, copy_bool_array_values) {
  constexpr bool bool_arry[] = {true, false, true};
  TEST_VARIANT_ARRAY_COPY(bool_array_value, bool_arry);
}

TEST(TestYamlVariant, copy_integer_array_values) {
  constexpr int64_t int_arry[] = {1, 2, 3};
  TEST_VARIANT_ARRAY_COPY(integer_array_value, int_arry);
}

TEST(TestYamlVariant, copy_double_array_values) {
  constexpr double double_arry[] = {10.0, 11.0, 12.0};
  TEST_VARIANT_ARRAY_COPY(double_array_value, double_arry);
}

TEST(TestYamlVariant, copy_string_array_values) {
  // Strings just have to be different
  rcl_variant_t src_variant{};
  rcl_variant_t dest_variant{};
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  constexpr size_t size = 3u;
  src_variant.string_array_value =
    static_cast<rcutils_string_array_t *>(
    allocator.allocate(sizeof(rcutils_string_array_t), allocator.state));
  ASSERT_NE(nullptr, src_variant.string_array_value);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_variant_fini(&src_variant, allocator);
    rcl_yaml_variant_fini(&dest_variant, allocator);
  });
  *src_variant.string_array_value = rcutils_get_zero_initialized_string_array();
  ASSERT_EQ(
    RCUTILS_RET_OK, rcutils_string_array_init(src_variant.string_array_value, size, &allocator));
  src_variant.string_array_value->size = size;
  src_variant.string_array_value->data[0] = rcutils_strdup("string1", allocator);
  src_variant.string_array_value->data[1] = rcutils_strdup("string2", allocator);
  src_variant.string_array_value->data[2] = rcutils_strdup("string3", allocator);
  for (size_t i = 0; i < size; ++i) {
    ASSERT_NE(nullptr, src_variant.string_array_value->data[i]);
  }
  EXPECT_TRUE(rcl_yaml_variant_copy(&dest_variant, &src_variant, allocator));
  ASSERT_NE(nullptr, dest_variant.string_array_value);
  ASSERT_NE(nullptr, dest_variant.string_array_value->data);
  for (size_t i = 0; i < size; ++i) {
    SCOPED_TRACE(i);
    EXPECT_STREQ(
      src_variant.string_array_value->data[i], dest_variant.string_array_value->data[i]);
  }
}

TEST(TestYamlVariant, copy_string_array_maybe_fail) {
  rcl_variant_t src_variant{};
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  constexpr size_t size = 3u;
  src_variant.string_array_value =
    static_cast<rcutils_string_array_t *>(
    allocator.allocate(sizeof(rcutils_string_array_t), allocator.state));
  ASSERT_NE(nullptr, src_variant.string_array_value);
  OSRF_TESTING_TOOLS_CPP_SCOPE_EXIT(
  {
    rcl_yaml_variant_fini(&src_variant, allocator);
  });
  *src_variant.string_array_value = rcutils_get_zero_initialized_string_array();
  ASSERT_EQ(
    RCUTILS_RET_OK, rcutils_string_array_init(src_variant.string_array_value, size, &allocator));
  src_variant.string_array_value->size = size;
  src_variant.string_array_value->data[0] = rcutils_strdup("string1", allocator);
  src_variant.string_array_value->data[1] = rcutils_strdup("string2", allocator);
  src_variant.string_array_value->data[2] = rcutils_strdup("string3", allocator);
  for (size_t i = 0; i < size; ++i) {
    ASSERT_NE(nullptr, src_variant.string_array_value->data[i]);
  }

  RCUTILS_FAULT_INJECTION_TEST(
  {
    rcl_variant_t dest_variant{};
    rcutils_ret_t ret = rcl_yaml_variant_copy(&dest_variant, &src_variant, allocator);
    (void)ret;
    rcl_yaml_variant_fini(&dest_variant, allocator);
  });
}
