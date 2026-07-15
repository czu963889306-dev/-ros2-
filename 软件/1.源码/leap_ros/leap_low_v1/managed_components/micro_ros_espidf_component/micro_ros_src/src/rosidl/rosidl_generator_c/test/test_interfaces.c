// Copyright 2015 Open Source Robotics Foundation, Inc.
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


#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>
#include <stdint.h>
#include <string.h>

#include "rosidl_runtime_c/primitives_sequence_functions.h"
#include "rosidl_runtime_c/string_functions.h"
#include "rosidl_runtime_c/u16string_functions.h"

#include "rosidl_generator_c/msg/arrays.h"
#include "rosidl_generator_c/msg/defaults.h"
#include "rosidl_generator_c/msg/constants.h"
#include "rosidl_generator_c/msg/basic_types.h"
#include "rosidl_generator_c/msg/bounded_sequences.h"
#include "rosidl_generator_c/msg/multi_nested.h"
#include "rosidl_generator_c/msg/nested.h"
#include "rosidl_generator_c/msg/strings.h"
#include "rosidl_generator_c/msg/unbounded_sequences.h"
#include "rosidl_generator_c/msg/w_strings.h"

#define TEST_STRING \
  "Deep into that darkness peering, long I stood there wondering, fearing"
#define TEST_STRING2 \
  "The quick brown fox jumps over the lazy dog."
#define TEST_STRING3 \
  "PACK MY BOX WITH FIVE DOZEN LIQUOR JUGS."
#define TEST_WSTRING \
  u"Deep into that darkness peering, long I stood there wondering, fearing \u2122"
#define ARR_SIZE 3

#define STRINGIFY(x) _STRINGIFY(x)
#define _STRINGIFY(x) #x

#define EXPECT_FALSE(arg) if (arg) { \
    fputs(STRINGIFY(arg) " is not false\n", stderr); \
    return 1; \
}
#define EXPECT_TRUE(arg) if (!(arg)) { \
    fputs(STRINGIFY(arg) " is not true\n", stderr); \
    return 1; \
}
#define EXPECT_EQ(arg1, arg2) if ((arg1) != (arg2)) { \
    fputs(STRINGIFY(arg1) " != " STRINGIFY(arg2) "\n", stderr); \
    return 1; \
}
#define EXPECT_NE(arg1, arg2) if ((arg1) == (arg2)) return 1

const uint8_t test_values_byte[ARR_SIZE] = {0, 57, 110};
const char test_values_char[ARR_SIZE] = {'a', '5', '#'};
const float test_values_float32[ARR_SIZE] = {-3.000001f, 22143.541325f, 6331.00432f};
const double test_values_float64[ARR_SIZE] = {
  -120310.00843902140001, 22143.54483920141325, 6331.0048392104432
};
const int8_t test_values_int8[ARR_SIZE] = {-50, 13, 110};
const uint8_t test_values_uint8[ARR_SIZE] = {0, 125, 250};
const int16_t test_values_int16[ARR_SIZE] = {-22222, 0, 32767};
const uint16_t test_values_uint16[ARR_SIZE] = {0U, 33333U, 65535U};
const int32_t test_values_int32[ARR_SIZE] = {INT32_MIN / 2, 0L, INT32_MAX / 2};
const uint32_t test_values_uint32[ARR_SIZE] = {0UL, 444444UL, 4294967295UL};
const int64_t test_values_int64[ARR_SIZE] = {-9223372036854775807LL, 0, 9223372036854775807ULL};
const uint64_t test_values_uint64[ARR_SIZE] = {0ULL, 567890123456789ULL, 18446744073709551615ULL};
const char * test_values_string[ARR_SIZE] = {"", "max value", "min value"};

int test_arrays(void);
int test_basic_types(void);
int test_bounded_sequences(void);
int test_constants(void);
int test_defaults(void);
int test_multi_nested(void);
int test_nested(void);
int test_strings(void);
int test_unbounded_sequences(void);
int test_wstrings(void);

int main(void)
{
  int rc = 0;
  printf("Testing rosidl_generator_c basic types...\n");
  if (test_basic_types()) {
    fprintf(stderr, "test_basic_types() FAILED\n");
    rc++;
  }
  printf("Testing rosidl_generator_c constant types...\n");
  if (test_constants()) {
    fprintf(stderr, "test_constant() FAILED\n");
    rc++;
  }
  printf("Testing rosidl_generator_c default types...\n");
  if (test_defaults()) {
    fprintf(stderr, "test_defaults() FAILED\n");
    rc++;
  }
  printf("Testing rosidl_generator_c string types...\n");
  if (test_strings()) {
    fprintf(stderr, "test_strings() FAILED\n");
    rc++;
  }
  printf("Testing rosidl_generator_c array types\n");
  if (test_arrays()) {
    fprintf(stderr, "test_arrays() FAILED\n");
    rc++;
  }
  printf("Testing rosidl_generator_c wstring types\n");
  if (test_wstrings()) {
    fprintf(stderr, "test_wstrings() FAILED\n");
    rc++;
  }
  printf("Testing rosidl_generator_c bounded_sequences types\n");
  if (test_bounded_sequences()) {
    fprintf(stderr, "test_bounded_sequences() FAILED\n");
    rc++;
  }
  printf("Testing rosidl_generator_c nested types\n");
  if (test_nested()) {
    fprintf(stderr, "test_nested() FAILED\n");
    rc++;
  }
  printf("Testing rosidl_generator_c unbounded_sequences types\n");
  if (test_unbounded_sequences()) {
    fprintf(stderr, "test_unbounded_sequences() FAILED\n");
    rc++;
  }
  printf("Testing rosidl_generator_c multi_nested type\n");
  if (test_multi_nested()) {
    fprintf(stderr, "test_multi_nested() FAILED\n");
    rc++;
  }
  if (rc != 0) {
    fprintf(stderr, "Some tests failed!\n");
  } else {
    printf("All tests were good!\n");
  }
  return rc != 0;
}

int test_basic_types(void)
{
  // rosidl_generator_c__msg__BasicTypes basic;
  rosidl_generator_c__msg__BasicTypes * basic = NULL;
  basic = rosidl_generator_c__msg__BasicTypes__create();
  EXPECT_NE(basic, NULL);

  basic->bool_value = false;
  EXPECT_EQ(false, basic->bool_value);

  basic->byte_value = 25;
  EXPECT_EQ(25, basic->byte_value);

  basic->char_value = 0;
  EXPECT_EQ(0, basic->char_value);

  basic->float32_value = FLT_MIN;
  EXPECT_EQ(FLT_MIN, basic->float32_value);

  basic->float64_value = DBL_MIN;
  EXPECT_EQ(DBL_MIN, basic->float64_value);

  basic->int8_value = INT8_MIN;
  EXPECT_EQ(INT8_MIN, basic->int8_value);

  basic->uint8_value = UINT8_MAX;
  EXPECT_EQ(UINT8_MAX, basic->uint8_value);

  basic->int16_value = INT16_MIN;
  EXPECT_EQ(INT16_MIN, basic->int16_value);

  basic->uint16_value = UINT16_MAX;
  EXPECT_EQ(UINT16_MAX, basic->uint16_value);

  basic->int32_value = INT32_MIN;
  EXPECT_EQ(INT32_MIN, basic->int32_value);

  basic->uint32_value = UINT32_MAX;
  EXPECT_EQ(UINT32_MAX, basic->uint32_value);

  basic->int64_value = INT64_MIN;
  EXPECT_EQ(INT64_MIN, basic->int64_value);

  basic->uint64_value = UINT64_MAX;
  EXPECT_EQ(UINT64_MAX, basic->uint64_value);

  EXPECT_FALSE(rosidl_generator_c__msg__BasicTypes__are_equal(NULL, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__BasicTypes__are_equal(basic, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__BasicTypes__are_equal(NULL, basic));
  EXPECT_TRUE(rosidl_generator_c__msg__BasicTypes__are_equal(basic, basic));

  rosidl_generator_c__msg__BasicTypes * basic_copy = NULL;
  basic_copy = rosidl_generator_c__msg__BasicTypes__create();
  EXPECT_NE(basic_copy, NULL);
  EXPECT_FALSE(rosidl_generator_c__msg__BasicTypes__are_equal(basic, basic_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__BasicTypes__copy(basic, basic_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__BasicTypes__are_equal(basic, basic_copy));
  rosidl_generator_c__msg__BasicTypes__destroy(basic_copy);

  rosidl_generator_c__msg__BasicTypes__destroy(basic);
  return 0;
}

int test_constants(void)
{
  EXPECT_EQ(true, rosidl_generator_c__msg__Constants__BOOL_CONST);
  EXPECT_EQ(50, rosidl_generator_c__msg__Constants__BYTE_CONST);
  EXPECT_EQ(100, rosidl_generator_c__msg__Constants__CHAR_CONST);
  EXPECT_EQ(1.125, rosidl_generator_c__msg__Constants__FLOAT32_CONST);
  EXPECT_EQ(1.125, rosidl_generator_c__msg__Constants__FLOAT64_CONST);
  EXPECT_EQ(-50, rosidl_generator_c__msg__Constants__INT8_CONST);
  EXPECT_EQ(200, rosidl_generator_c__msg__Constants__UINT8_CONST);
  EXPECT_EQ(-1000, rosidl_generator_c__msg__Constants__INT16_CONST);
  EXPECT_EQ(2000, rosidl_generator_c__msg__Constants__UINT16_CONST);
  EXPECT_EQ(-30000, rosidl_generator_c__msg__Constants__INT32_CONST);
  EXPECT_EQ(60000, rosidl_generator_c__msg__Constants__UINT32_CONST);
  EXPECT_EQ(-40000000, rosidl_generator_c__msg__Constants__INT64_CONST);
  EXPECT_EQ(50000000, rosidl_generator_c__msg__Constants__UINT64_CONST);

  return 0;
}

int test_defaults()
{
  rosidl_generator_c__msg__Defaults * def = NULL;
  def = rosidl_generator_c__msg__Defaults__create();
  EXPECT_NE(def, NULL);

  EXPECT_EQ(true, def->bool_value);
  EXPECT_EQ(50, def->byte_value);
  EXPECT_EQ(100, def->char_value);
  EXPECT_EQ(1.125, def->float32_value);
  EXPECT_EQ(1.125, def->float64_value);
  EXPECT_EQ(-50, def->int8_value);
  EXPECT_EQ(200, def->uint8_value);
  EXPECT_EQ(-1000, def->int16_value);
  EXPECT_EQ(2000, def->uint16_value);
  EXPECT_EQ(-30000, def->int32_value);
  EXPECT_EQ(60000, def->uint32_value);
  EXPECT_EQ(-40000000, def->int64_value);
  EXPECT_EQ(50000000, def->uint64_value);

  EXPECT_FALSE(rosidl_generator_c__msg__Defaults__are_equal(NULL, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__Defaults__are_equal(def, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__Defaults__are_equal(NULL, def));
  EXPECT_TRUE(rosidl_generator_c__msg__Defaults__are_equal(def, def));

  rosidl_generator_c__msg__Defaults * def_copy = NULL;
  def_copy = rosidl_generator_c__msg__Defaults__create();
  EXPECT_NE(def_copy, NULL);
  def->bool_value = false;  // mutate message to force a difference
  EXPECT_FALSE(rosidl_generator_c__msg__Defaults__are_equal(def, def_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__Defaults__copy(def, def_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__Defaults__are_equal(def, def_copy));
  rosidl_generator_c__msg__Defaults__destroy(def_copy);

  rosidl_generator_c__msg__Defaults__destroy(def);
  return 0;
}

int test_bounded_sequences()
{
  rosidl_generator_c__msg__BoundedSequences * seq = NULL;
  seq = rosidl_generator_c__msg__BoundedSequences__create();
  EXPECT_NE(seq, NULL);
  bool res = false;
  res = rosidl_runtime_c__boolean__Sequence__init(&seq->bool_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  int i = 0;
  for (i = 0; i < ARR_SIZE; i++) {
    if (0 == (i % 2)) {
      seq->bool_values.data[i] = true;
    } else {
      seq->bool_values.data[i] = false;
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    if (0 == (i % 2)) {
      EXPECT_EQ(true, seq->bool_values.data[i]);
    } else {
      EXPECT_EQ(false, seq->bool_values.data[i]);
    }
  }

  // byte_values
  res = rosidl_runtime_c__byte__Sequence__init(&seq->byte_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->byte_values.data[i] = test_values_byte[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_byte[i], seq->byte_values.data[i]);
  }

  // char_values
  res = rosidl_runtime_c__uint8__Sequence__init(&seq->char_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->char_values.data[i] = test_values_char[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_char[i], seq->char_values.data[i]);
  }

  // float32_values
  res = rosidl_runtime_c__float32__Sequence__init(&seq->float32_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->float32_values.data[i] = test_values_float32[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_float32[i], seq->float32_values.data[i]);
  }
  // float64_values
  res = rosidl_runtime_c__float64__Sequence__init(&seq->float64_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->float64_values.data[i] = test_values_float64[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_float64[i], seq->float64_values.data[i]);
  }

  // int8_values
  res = rosidl_runtime_c__int8__Sequence__init(&seq->int8_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->int8_values.data[i] = test_values_int8[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_int8[i], seq->int8_values.data[i]);
  }

  // uint8_values
  res = rosidl_runtime_c__uint8__Sequence__init(&seq->uint8_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->uint8_values.data[i] = test_values_uint8[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_uint8[i], seq->uint8_values.data[i]);
  }

  // int16_values
  res = rosidl_runtime_c__int16__Sequence__init(&seq->int16_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->int16_values.data[i] = test_values_int16[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_int16[i], seq->int16_values.data[i]);
  }

  // uint16_values
  res = rosidl_runtime_c__uint16__Sequence__init(&seq->uint16_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->uint16_values.data[i] = test_values_uint16[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_uint16[i], seq->uint16_values.data[i]);
  }

  // int32_values
  res = rosidl_runtime_c__int32__Sequence__init(&seq->int32_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->int32_values.data[i] = test_values_int32[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_int32[i], seq->int32_values.data[i]);
  }

  // uint32_values
  res = rosidl_runtime_c__uint32__Sequence__init(&seq->uint32_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->uint32_values.data[i] = test_values_uint32[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_uint32[i], seq->uint32_values.data[i]);
  }

  // int64_values
  res = rosidl_runtime_c__int64__Sequence__init(&seq->int64_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->int64_values.data[i] = test_values_int64[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_int64[i], seq->int64_values.data[i]);
  }

  // uint64_values
  res = rosidl_runtime_c__uint64__Sequence__init(&seq->uint64_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->uint64_values.data[i] = test_values_uint64[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_uint64[i], seq->uint64_values.data[i]);
  }

  // string_values
  res = rosidl_runtime_c__String__Sequence__init(&seq->string_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    rosidl_runtime_c__String__assign(&seq->string_values.data[i], test_values_string[i]);
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(0, strcmp(seq->string_values.data[i].data, test_values_string[i]));
  }

  // *_values_default
  EXPECT_EQ(false, seq->bool_values_default.data[0]);
  EXPECT_EQ(true, seq->bool_values_default.data[1]);
  EXPECT_EQ(false, seq->bool_values_default.data[2]);

  EXPECT_EQ(0, seq->byte_values_default.data[0]);
  EXPECT_EQ(1, seq->byte_values_default.data[1]);
  EXPECT_EQ(255, seq->byte_values_default.data[2]);

  EXPECT_EQ(0, seq->char_values_default.data[0]);
  EXPECT_EQ(1, seq->char_values_default.data[1]);
  EXPECT_EQ(127, seq->char_values_default.data[2]);

  EXPECT_EQ(1.125, seq->float32_values_default.data[0]);
  EXPECT_EQ(0.0, seq->float32_values_default.data[1]);
  EXPECT_EQ(-1.125, seq->float32_values_default.data[2]);

  EXPECT_EQ(3.1415, seq->float64_values_default.data[0]);
  EXPECT_EQ(0.0, seq->float64_values_default.data[1]);
  EXPECT_EQ(-3.1415, seq->float64_values_default.data[2]);

  EXPECT_EQ(0, seq->int8_values_default.data[0]);
  EXPECT_EQ(INT8_MAX, seq->int8_values_default.data[1]);
  EXPECT_EQ(INT8_MIN, seq->int8_values_default.data[2]);

  EXPECT_EQ(0, seq->uint8_values_default.data[0]);
  EXPECT_EQ(1, seq->uint8_values_default.data[1]);
  EXPECT_EQ(UINT8_MAX, seq->uint8_values_default.data[2]);

  EXPECT_EQ(0, seq->int16_values_default.data[0]);
  EXPECT_EQ(INT16_MAX, seq->int16_values_default.data[1]);
  EXPECT_EQ(INT16_MIN, seq->int16_values_default.data[2]);

  EXPECT_EQ(0, seq->uint16_values_default.data[0]);
  EXPECT_EQ(1, seq->uint16_values_default.data[1]);
  EXPECT_EQ(UINT16_MAX, seq->uint16_values_default.data[2]);

  EXPECT_EQ(0, seq->int32_values_default.data[0]);
  EXPECT_EQ(INT32_MAX, seq->int32_values_default.data[1]);
  EXPECT_EQ(INT32_MIN, seq->int32_values_default.data[2]);

  EXPECT_EQ(0, seq->uint32_values_default.data[0]);
  EXPECT_EQ(1, seq->uint32_values_default.data[1]);
  EXPECT_EQ(UINT32_MAX, seq->uint32_values_default.data[2]);

  EXPECT_EQ(0, seq->int64_values_default.data[0]);
  EXPECT_EQ(INT64_MAX, seq->int64_values_default.data[1]);
  EXPECT_EQ(INT64_MIN, seq->int64_values_default.data[2]);

  EXPECT_EQ(0, seq->uint64_values_default.data[0]);
  EXPECT_EQ(1, seq->uint64_values_default.data[1]);
  EXPECT_EQ(UINT64_MAX, seq->uint64_values_default.data[2]);


  EXPECT_EQ(0, strcmp(seq->string_values_default.data[0].data, ""));
  EXPECT_EQ(0, strcmp(seq->string_values_default.data[1].data, "max value"));
  EXPECT_EQ(0, strcmp(seq->string_values_default.data[2].data, "min value"));

  EXPECT_FALSE(rosidl_generator_c__msg__BoundedSequences__are_equal(NULL, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__BoundedSequences__are_equal(seq, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__BoundedSequences__are_equal(NULL, seq));
  EXPECT_TRUE(rosidl_generator_c__msg__BoundedSequences__are_equal(seq, seq));

  rosidl_generator_c__msg__BoundedSequences * seq_copy = NULL;
  seq_copy = rosidl_generator_c__msg__BoundedSequences__create();
  EXPECT_NE(seq_copy, NULL);
  EXPECT_FALSE(rosidl_generator_c__msg__BoundedSequences__are_equal(seq, seq_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__BoundedSequences__copy(seq, seq_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__BoundedSequences__are_equal(seq, seq_copy));
  rosidl_generator_c__msg__BoundedSequences__destroy(seq_copy);

  rosidl_generator_c__msg__BoundedSequences__destroy(seq);
  return 0;
}

int test_unbounded_sequences()
{
  rosidl_generator_c__msg__UnboundedSequences * seq = NULL;
  seq = rosidl_generator_c__msg__UnboundedSequences__create();
  EXPECT_NE(seq, NULL);
  bool res = false;

  // bool_values
  res = rosidl_runtime_c__boolean__Sequence__init(&seq->bool_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  int i = 0;
  for (i = 0; i < ARR_SIZE; i++) {
    if (0 == (i % 2)) {
      seq->bool_values.data[i] = true;
    } else {
      seq->bool_values.data[i] = false;
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    if (0 == (i % 2)) {
      EXPECT_EQ(true, seq->bool_values.data[i]);
    } else {
      EXPECT_EQ(false, seq->bool_values.data[i]);
    }
  }

  // byte_values
  res = rosidl_runtime_c__byte__Sequence__init(&seq->byte_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->byte_values.data[i] = test_values_byte[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_byte[i], seq->byte_values.data[i]);
  }

  // char_values
  res = rosidl_runtime_c__uint8__Sequence__init(&seq->char_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->char_values.data[i] = test_values_char[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_char[i], seq->char_values.data[i]);
  }

  // float32_values
  res = rosidl_runtime_c__float32__Sequence__init(&seq->float32_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->float32_values.data[i] = test_values_float32[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_float32[i], seq->float32_values.data[i]);
  }
  // float64_values
  res = rosidl_runtime_c__float64__Sequence__init(&seq->float64_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->float64_values.data[i] = test_values_float64[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_float64[i], seq->float64_values.data[i]);
  }

  // int8_values
  res = rosidl_runtime_c__int8__Sequence__init(&seq->int8_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->int8_values.data[i] = test_values_int8[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_int8[i], seq->int8_values.data[i]);
  }

  // uint8_values
  res = rosidl_runtime_c__uint8__Sequence__init(&seq->uint8_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->uint8_values.data[i] = test_values_uint8[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_uint8[i], seq->uint8_values.data[i]);
  }

  // int16_values
  res = rosidl_runtime_c__int16__Sequence__init(&seq->int16_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->int16_values.data[i] = test_values_int16[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_int16[i], seq->int16_values.data[i]);
  }

  // uint16_values
  res = rosidl_runtime_c__uint16__Sequence__init(&seq->uint16_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->uint16_values.data[i] = test_values_uint16[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_uint16[i], seq->uint16_values.data[i]);
  }

  // int32_values
  res = rosidl_runtime_c__int32__Sequence__init(&seq->int32_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->int32_values.data[i] = test_values_int32[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_int32[i], seq->int32_values.data[i]);
  }

  // uint32_values
  res = rosidl_runtime_c__uint32__Sequence__init(&seq->uint32_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->uint32_values.data[i] = test_values_uint32[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_uint32[i], seq->uint32_values.data[i]);
  }

  // int64_values
  res = rosidl_runtime_c__int64__Sequence__init(&seq->int64_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->int64_values.data[i] = test_values_int64[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_int64[i], seq->int64_values.data[i]);
  }

  // uint64_values
  res = rosidl_runtime_c__uint64__Sequence__init(&seq->uint64_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    seq->uint64_values.data[i] = test_values_uint64[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_uint64[i], seq->uint64_values.data[i]);
  }

  // string_values
  res = rosidl_runtime_c__String__Sequence__init(&seq->string_values, ARR_SIZE);
  EXPECT_EQ(true, res);
  for (i = 0; i < ARR_SIZE; i++) {
    rosidl_runtime_c__String__assign(&seq->string_values.data[i], test_values_string[i]);
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(0, strcmp(seq->string_values.data[i].data, test_values_string[i]));
  }

  // *_values_default
  EXPECT_EQ(false, seq->bool_values_default.data[0]);
  EXPECT_EQ(true, seq->bool_values_default.data[1]);
  EXPECT_EQ(false, seq->bool_values_default.data[2]);

  EXPECT_EQ(0, seq->byte_values_default.data[0]);
  EXPECT_EQ(1, seq->byte_values_default.data[1]);
  EXPECT_EQ(255, seq->byte_values_default.data[2]);

  EXPECT_EQ(0, seq->char_values_default.data[0]);
  EXPECT_EQ(1, seq->char_values_default.data[1]);
  EXPECT_EQ(127, seq->char_values_default.data[2]);

  EXPECT_EQ(1.125, seq->float32_values_default.data[0]);
  EXPECT_EQ(0.0, seq->float32_values_default.data[1]);
  EXPECT_EQ(-1.125, seq->float32_values_default.data[2]);

  EXPECT_EQ(3.1415, seq->float64_values_default.data[0]);
  EXPECT_EQ(0.0, seq->float64_values_default.data[1]);
  EXPECT_EQ(-3.1415, seq->float64_values_default.data[2]);

  EXPECT_EQ(0, seq->int8_values_default.data[0]);
  EXPECT_EQ(INT8_MAX, seq->int8_values_default.data[1]);
  EXPECT_EQ(INT8_MIN, seq->int8_values_default.data[2]);

  EXPECT_EQ(0, seq->uint8_values_default.data[0]);
  EXPECT_EQ(1, seq->uint8_values_default.data[1]);
  EXPECT_EQ(UINT8_MAX, seq->uint8_values_default.data[2]);

  EXPECT_EQ(0, seq->int16_values_default.data[0]);
  EXPECT_EQ(INT16_MAX, seq->int16_values_default.data[1]);
  EXPECT_EQ(INT16_MIN, seq->int16_values_default.data[2]);

  EXPECT_EQ(0, seq->uint16_values_default.data[0]);
  EXPECT_EQ(1, seq->uint16_values_default.data[1]);
  EXPECT_EQ(UINT16_MAX, seq->uint16_values_default.data[2]);

  EXPECT_EQ(0, seq->int32_values_default.data[0]);
  EXPECT_EQ(INT32_MAX, seq->int32_values_default.data[1]);
  EXPECT_EQ(INT32_MIN, seq->int32_values_default.data[2]);

  EXPECT_EQ(0, seq->uint32_values_default.data[0]);
  EXPECT_EQ(1, seq->uint32_values_default.data[1]);
  EXPECT_EQ(UINT32_MAX, seq->uint32_values_default.data[2]);

  EXPECT_EQ(0, seq->int64_values_default.data[0]);
  EXPECT_EQ(INT64_MAX, seq->int64_values_default.data[1]);
  EXPECT_EQ(INT64_MIN, seq->int64_values_default.data[2]);

  EXPECT_EQ(0, seq->uint64_values_default.data[0]);
  EXPECT_EQ(1, seq->uint64_values_default.data[1]);
  EXPECT_EQ(UINT64_MAX, seq->uint64_values_default.data[2]);

  EXPECT_EQ(0, strcmp(seq->string_values_default.data[0].data, ""));
  EXPECT_EQ(0, strcmp(seq->string_values_default.data[1].data, "max value"));
  EXPECT_EQ(0, strcmp(seq->string_values_default.data[2].data, "min value"));

  EXPECT_FALSE(rosidl_generator_c__msg__UnboundedSequences__are_equal(NULL, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__UnboundedSequences__are_equal(seq, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__UnboundedSequences__are_equal(NULL, seq));
  EXPECT_TRUE(rosidl_generator_c__msg__UnboundedSequences__are_equal(seq, seq));

  rosidl_generator_c__msg__UnboundedSequences * seq_copy = NULL;
  seq_copy = rosidl_generator_c__msg__UnboundedSequences__create();
  EXPECT_NE(seq_copy, NULL);
  EXPECT_FALSE(rosidl_generator_c__msg__UnboundedSequences__are_equal(seq, seq_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__UnboundedSequences__copy(seq, seq_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__UnboundedSequences__are_equal(seq, seq_copy));
  rosidl_generator_c__msg__UnboundedSequences__destroy(seq_copy);

  rosidl_generator_c__msg__UnboundedSequences__destroy(seq);
  return 0;
}

int test_strings()
{
  rosidl_generator_c__msg__Strings * str = NULL;
  str = rosidl_generator_c__msg__Strings__create();
  EXPECT_NE(str, NULL);

  EXPECT_EQ(0, strcmp(str->string_value.data, ""));
  EXPECT_EQ(0, strcmp(str->string_value_default1.data, "Hello world!"));
  EXPECT_EQ(0, strcmp(str->string_value_default5.data, "Hello\"world!"));
  EXPECT_EQ(0, strcmp(rosidl_generator_c__msg__Strings__STRING_CONST, "Hello world!"));

  bool res = false;
  res = rosidl_runtime_c__String__assign(&str->bounded_string_value, TEST_STRING);
  EXPECT_EQ(true, res);
  EXPECT_EQ(0, strcmp(str->bounded_string_value.data, TEST_STRING));

  res = rosidl_runtime_c__String__assign(&str->bounded_string_value, TEST_STRING2);
  EXPECT_EQ(true, res);
  EXPECT_EQ(0, strcmp(str->bounded_string_value.data, TEST_STRING2));

  res = rosidl_runtime_c__String__assign(&str->bounded_string_value, TEST_STRING3);
  EXPECT_EQ(true, res);
  EXPECT_EQ(0, strcmp(str->bounded_string_value.data, TEST_STRING3));

  EXPECT_EQ(0, strcmp(str->bounded_string_value_default1.data, "Hello world!"));
  EXPECT_EQ(0, strcmp(str->bounded_string_value_default5.data, "Hello\"world!"));

  EXPECT_FALSE(rosidl_generator_c__msg__Strings__are_equal(NULL, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__Strings__are_equal(str, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__Strings__are_equal(NULL, str));
  EXPECT_TRUE(rosidl_generator_c__msg__Strings__are_equal(str, str));

  rosidl_generator_c__msg__Strings * str_copy = NULL;
  str_copy = rosidl_generator_c__msg__Strings__create();
  EXPECT_NE(str_copy, NULL);
  EXPECT_FALSE(rosidl_generator_c__msg__Strings__are_equal(str, str_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__Strings__copy(str, str_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__Strings__are_equal(str, str_copy));
  rosidl_generator_c__msg__Strings__destroy(str_copy);

  rosidl_generator_c__msg__Strings__destroy(str);
  return 0;
}

int test_nested()
{
  rosidl_generator_c__msg__Nested * nested = NULL;
  nested = rosidl_generator_c__msg__Nested__create();
  EXPECT_NE(nested, NULL);

  nested->basic_types_value.bool_value = true;
  nested->basic_types_value.byte_value = 50;
  nested->basic_types_value.char_value = 100;
  nested->basic_types_value.float32_value = 1.125;
  nested->basic_types_value.float64_value = 1.125;
  nested->basic_types_value.int8_value = -50;
  nested->basic_types_value.uint8_value = 200;
  nested->basic_types_value.int16_value = -1000;
  nested->basic_types_value.uint16_value = 2000;
  nested->basic_types_value.int32_value = -30000;
  nested->basic_types_value.uint32_value = 60000;
  nested->basic_types_value.int64_value = -40000000;
  nested->basic_types_value.uint64_value = 50000000;

  EXPECT_EQ(true, nested->basic_types_value.bool_value);
  EXPECT_EQ(50, nested->basic_types_value.byte_value);
  EXPECT_EQ(100, nested->basic_types_value.char_value);
  EXPECT_EQ(1.125, nested->basic_types_value.float32_value);
  EXPECT_EQ(1.125, nested->basic_types_value.float64_value);
  EXPECT_EQ(-50, nested->basic_types_value.int8_value);
  EXPECT_EQ(200, nested->basic_types_value.uint8_value);
  EXPECT_EQ(-1000, nested->basic_types_value.int16_value);
  EXPECT_EQ(2000, nested->basic_types_value.uint16_value);
  EXPECT_EQ(-30000, nested->basic_types_value.int32_value);
  EXPECT_EQ(60000, nested->basic_types_value.uint32_value);
  EXPECT_EQ(-40000000, nested->basic_types_value.int64_value);
  EXPECT_EQ(50000000, nested->basic_types_value.uint64_value);

  EXPECT_FALSE(rosidl_generator_c__msg__Nested__are_equal(NULL, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__Nested__are_equal(nested, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__Nested__are_equal(NULL, nested));
  EXPECT_TRUE(rosidl_generator_c__msg__Nested__are_equal(nested, nested));

  rosidl_generator_c__msg__Nested * nested_copy = NULL;
  nested_copy = rosidl_generator_c__msg__Nested__create();
  EXPECT_NE(nested_copy, NULL);
  EXPECT_FALSE(rosidl_generator_c__msg__Nested__are_equal(nested, nested_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__Nested__copy(nested, nested_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__Nested__are_equal(nested, nested_copy));
  rosidl_generator_c__msg__Nested__destroy(nested_copy);

  rosidl_generator_c__msg__Nested__destroy(nested);
  return 0;
}

int test_multi_nested()
{
  rosidl_generator_c__msg__MultiNested * msg = NULL;
  msg = rosidl_generator_c__msg__MultiNested__create();
  EXPECT_NE(msg, NULL);
  /* multi_nested fields tests todo:
  BoundedSequences[3] array_of_bounded_sequences
  UnboundedSequences[3] array_of_unbounded_sequences
  Arrays[<=3] bounded_sequence_of_arrays
  BoundedSequences[<=3] bounded_sequence_of_bounded_sequences
  UnboundedSequences[<=3] bounded_sequence_of_unbounded_sequences
  Arrays[] unbounded_sequence_of_arrays
  BoundedSequences[] unbounded_sequence_of_bounded_sequences
  UnboundedSequences[] unbounded_sequence_of_unbounded_sequences
  */
  int i = 0;
  int j = 0;
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      if (0 == (j % 2)) {
        msg->array_of_arrays[i].bool_values[j] = true;
      } else {
        msg->array_of_arrays[i].bool_values[j] = false;
      }
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      if (0 == (j % 2)) {
        EXPECT_EQ(true, msg->array_of_arrays[i].bool_values[j]);
      } else {
        EXPECT_EQ(false, msg->array_of_arrays[i].bool_values[j]);
      }
    }
  }

  // byte_values
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      msg->array_of_arrays[i].byte_values[j] = test_values_byte[j];
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      EXPECT_EQ(test_values_byte[j], msg->array_of_arrays[i].byte_values[j]);
    }
  }

  // char_values
  char test_values_char[ARR_SIZE] = {'a', '5', '#'};
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      msg->array_of_arrays[i].char_values[j] = test_values_char[j];
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      EXPECT_EQ(test_values_char[j], msg->array_of_arrays[i].char_values[j]);
    }
  }

  // float32_values
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      msg->array_of_arrays[i].float32_values[j] = test_values_float32[j];
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      EXPECT_EQ(test_values_float32[j], msg->array_of_arrays[i].float32_values[j]);
    }
  }

  // float64_values
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      msg->array_of_arrays[i].float64_values[j] = test_values_float64[j];
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      EXPECT_EQ(test_values_float64[j], msg->array_of_arrays[i].float64_values[j]);
    }
  }

  // int8_values
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      msg->array_of_arrays[i].int8_values[j] = test_values_int8[j];
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      EXPECT_EQ(test_values_int8[j], msg->array_of_arrays[i].int8_values[j]);
    }
  }

  // uint8_values
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      msg->array_of_arrays[i].uint8_values[j] = test_values_uint8[j];
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      EXPECT_EQ(test_values_uint8[j], msg->array_of_arrays[i].uint8_values[j]);
    }
  }

  // int16_values
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      msg->array_of_arrays[i].int16_values[j] = test_values_int16[j];
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      EXPECT_EQ(test_values_int16[j], msg->array_of_arrays[i].int16_values[j]);
    }
  }

  // uint16_values
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      msg->array_of_arrays[i].uint16_values[j] = test_values_uint16[j];
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      EXPECT_EQ(test_values_uint16[j], msg->array_of_arrays[i].uint16_values[j]);
    }
  }

  // int32_values
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      msg->array_of_arrays[i].int32_values[j] = test_values_int32[j];
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      EXPECT_EQ(test_values_int32[j], msg->array_of_arrays[i].int32_values[j]);
    }
  }

  // uint32_values
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      msg->array_of_arrays[i].uint32_values[j] = test_values_uint32[j];
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      EXPECT_EQ(test_values_uint32[j], msg->array_of_arrays[i].uint32_values[j]);
    }
  }

  // int64_values
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      msg->array_of_arrays[i].int64_values[j] = test_values_int64[j];
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      EXPECT_EQ(test_values_int64[j], msg->array_of_arrays[i].int64_values[j]);
    }
  }

  // uint64_values
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      msg->array_of_arrays[i].uint64_values[j] = test_values_uint64[j];
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    for (j = 0; j < ARR_SIZE; j++) {
      EXPECT_EQ(test_values_uint64[j], msg->array_of_arrays[i].uint64_values[j]);
    }
  }

  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(false, msg->array_of_arrays[i].bool_values_default[0]);
    EXPECT_EQ(true, msg->array_of_arrays[i].bool_values_default[1]);
    EXPECT_EQ(false, msg->array_of_arrays[i].bool_values_default[2]);

    EXPECT_EQ(0, msg->array_of_arrays[i].byte_values_default[0]);
    EXPECT_EQ(1, msg->array_of_arrays[i].byte_values_default[1]);
    EXPECT_EQ(255, msg->array_of_arrays[i].byte_values_default[2]);

    EXPECT_EQ(0, msg->array_of_arrays[i].char_values_default[0]);
    EXPECT_EQ(1, msg->array_of_arrays[i].char_values_default[1]);
    EXPECT_EQ(127, msg->array_of_arrays[i].char_values_default[2]);

    EXPECT_EQ(1.125, msg->array_of_arrays[i].float32_values_default[0]);
    EXPECT_EQ(0.0, msg->array_of_arrays[i].float32_values_default[1]);
    EXPECT_EQ(-1.125, msg->array_of_arrays[i].float32_values_default[2]);

    EXPECT_EQ(3.1415, msg->array_of_arrays[i].float64_values_default[0]);
    EXPECT_EQ(0.0, msg->array_of_arrays[i].float64_values_default[1]);
    EXPECT_EQ(-3.1415, msg->array_of_arrays[i].float64_values_default[2]);

    EXPECT_EQ(0, msg->array_of_arrays[i].int8_values_default[0]);
    EXPECT_EQ(INT8_MAX, msg->array_of_arrays[i].int8_values_default[1]);
    EXPECT_EQ(INT8_MIN, msg->array_of_arrays[i].int8_values_default[2]);

    EXPECT_EQ(0, msg->array_of_arrays[i].uint8_values_default[0]);
    EXPECT_EQ(1, msg->array_of_arrays[i].uint8_values_default[1]);
    EXPECT_EQ(UINT8_MAX, msg->array_of_arrays[i].uint8_values_default[2]);

    EXPECT_EQ(0, msg->array_of_arrays[i].int16_values_default[0]);
    EXPECT_EQ(INT16_MAX, msg->array_of_arrays[i].int16_values_default[1]);
    EXPECT_EQ(INT16_MIN, msg->array_of_arrays[i].int16_values_default[2]);

    EXPECT_EQ(0, msg->array_of_arrays[i].uint16_values_default[0]);
    EXPECT_EQ(1, msg->array_of_arrays[i].uint16_values_default[1]);
    EXPECT_EQ(UINT16_MAX, msg->array_of_arrays[i].uint16_values_default[2]);

    EXPECT_EQ(0, msg->array_of_arrays[i].int32_values_default[0]);
    EXPECT_EQ(INT32_MAX, msg->array_of_arrays[i].int32_values_default[1]);
    EXPECT_EQ(INT32_MIN, msg->array_of_arrays[i].int32_values_default[2]);

    EXPECT_EQ(0, msg->array_of_arrays[i].uint32_values_default[0]);
    EXPECT_EQ(1, msg->array_of_arrays[i].uint32_values_default[1]);
    EXPECT_EQ(UINT32_MAX, msg->array_of_arrays[i].uint32_values_default[2]);

    EXPECT_EQ(0, msg->array_of_arrays[i].int64_values_default[0]);
    EXPECT_EQ(INT64_MAX, msg->array_of_arrays[i].int64_values_default[1]);
    EXPECT_EQ(INT64_MIN, msg->array_of_arrays[i].int64_values_default[2]);

    EXPECT_EQ(0, msg->array_of_arrays[i].uint64_values_default[0]);
    EXPECT_EQ(1, msg->array_of_arrays[i].uint64_values_default[1]);
    EXPECT_EQ(UINT64_MAX, msg->array_of_arrays[0].uint64_values_default[2]);
  }

  EXPECT_FALSE(rosidl_generator_c__msg__MultiNested__are_equal(NULL, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__MultiNested__are_equal(msg, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__MultiNested__are_equal(NULL, msg));
  EXPECT_TRUE(rosidl_generator_c__msg__MultiNested__are_equal(msg, msg));

  rosidl_generator_c__msg__MultiNested * msg_copy = NULL;
  msg_copy = rosidl_generator_c__msg__MultiNested__create();
  EXPECT_NE(msg_copy, NULL);
  EXPECT_FALSE(rosidl_generator_c__msg__MultiNested__are_equal(msg, msg_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__MultiNested__copy(msg, msg_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__MultiNested__are_equal(msg, msg_copy));
  rosidl_generator_c__msg__MultiNested__destroy(msg_copy);

  rosidl_generator_c__msg__MultiNested__destroy(msg);
  return 0;
}

int test_wstrings()
{
  rosidl_generator_c__msg__WStrings * wstr = NULL;
  wstr = rosidl_generator_c__msg__WStrings__create();
  EXPECT_NE(wstr, NULL);

  bool res = false;
  res = rosidl_runtime_c__U16String__assign(&wstr->wstring_value, TEST_WSTRING);
  EXPECT_EQ(true, res);

  // EXPECT_EQ(0, strcmp(wstr->wstring_value_default1.data, "Hello world!"));
  // TODO(dirk-thomas) to be reenabled with the fields in the message
  // EXPECT_EQ(0, strcmp(wstrings->empty_wstring.data, TEST_WSTRING));
  // EXPECT_EQ(0, strcmp(wstrings->def_wstring.data, "Hello world!"));
  // EXPECT_EQ(0, strcmp(wstrings->def_wstring2.data, "Hello'world!"));
  // EXPECT_EQ(0, strcmp(wstrings->def_wstring3.data, "Hello\"world!"));
  // EXPECT_EQ(0, strcmp(wstrings->def_wstring4.data, "Hello'world!"));
  // EXPECT_EQ(0, strcmp(wstrings->def_wstring5.data, "Hello\"world!"));
  // since upper-bound checking is not implemented yet, we restrict the string copying

  rosidl_generator_c__msg__WStrings__destroy(wstr);
  return 0;
}

int test_arrays()
{
  rosidl_generator_c__msg__Arrays * arr = NULL;
  arr = rosidl_generator_c__msg__Arrays__create();
  EXPECT_NE(arr, NULL);

  int i = 0;
  // bool_values
  for (i = 0; i < ARR_SIZE; i++) {
    if (0 == (i % 2)) {
      arr->bool_values[i] = true;
    } else {
      arr->bool_values[i] = false;
    }
  }
  for (i = 0; i < ARR_SIZE; i++) {
    if (0 == (i % 2)) {
      EXPECT_EQ(true, arr->bool_values[i]);
    } else {
      EXPECT_EQ(false, arr->bool_values[i]);
    }
  }

  // byte_values
  for (i = 0; i < ARR_SIZE; i++) {
    arr->byte_values[i] = test_values_byte[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_byte[i], arr->byte_values[i]);
  }

  // char_values
  for (i = 0; i < ARR_SIZE; i++) {
    arr->char_values[i] = test_values_char[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_char[i], arr->char_values[i]);
  }

  // float32_values
  for (i = 0; i < ARR_SIZE; i++) {
    arr->float32_values[i] = test_values_float32[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_float32[i], arr->float32_values[i]);
  }
  // float64_values
  for (i = 0; i < ARR_SIZE; i++) {
    arr->float64_values[i] = test_values_float64[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_float64[i], arr->float64_values[i]);
  }

  // int8_values
  for (i = 0; i < ARR_SIZE; i++) {
    arr->int8_values[i] = test_values_int8[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_int8[i], arr->int8_values[i]);
  }

  // uint8_values
  for (i = 0; i < ARR_SIZE; i++) {
    arr->uint8_values[i] = test_values_uint8[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_uint8[i], arr->uint8_values[i]);
  }

  // int16_values
  for (i = 0; i < ARR_SIZE; i++) {
    arr->int16_values[i] = test_values_int16[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_int16[i], arr->int16_values[i]);
  }

  // uint16_values
  for (i = 0; i < ARR_SIZE; i++) {
    arr->uint16_values[i] = test_values_uint16[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_uint16[i], arr->uint16_values[i]);
  }

  // int32_values
  int32_t test_values_int32[ARR_SIZE] = {INT32_MIN / 2, 0L, INT32_MAX / 2};
  for (i = 0; i < ARR_SIZE; i++) {
    arr->int32_values[i] = test_values_int32[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_int32[i], arr->int32_values[i]);
  }

  // uint32_values
  for (i = 0; i < ARR_SIZE; i++) {
    arr->uint32_values[i] = test_values_uint32[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_uint32[i], arr->uint32_values[i]);
  }

  // int64_values
  for (i = 0; i < ARR_SIZE; i++) {
    arr->int64_values[i] = test_values_int64[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_int64[i], arr->int64_values[i]);
  }

  // uint64_values
  for (i = 0; i < ARR_SIZE; i++) {
    arr->uint64_values[i] = test_values_uint64[i];
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(test_values_uint64[i], arr->uint64_values[i]);
  }

  // string_values
  for (i = 0; i < ARR_SIZE; i++) {
    rosidl_runtime_c__String__assign(&arr->string_values[i], test_values_string[i]);
  }
  for (i = 0; i < ARR_SIZE; i++) {
    EXPECT_EQ(0, strcmp(arr->string_values[i].data, test_values_string[i]));
  }

  EXPECT_EQ(false, arr->bool_values_default[0]);
  EXPECT_EQ(true, arr->bool_values_default[1]);
  EXPECT_EQ(false, arr->bool_values_default[2]);

  EXPECT_EQ(0, arr->byte_values_default[0]);
  EXPECT_EQ(1, arr->byte_values_default[1]);
  EXPECT_EQ(255, arr->byte_values_default[2]);

  EXPECT_EQ(0, arr->char_values_default[0]);
  EXPECT_EQ(1, arr->char_values_default[1]);
  EXPECT_EQ(127, arr->char_values_default[2]);

  EXPECT_EQ(1.125, arr->float32_values_default[0]);
  EXPECT_EQ(0.0, arr->float32_values_default[1]);
  EXPECT_EQ(-1.125, arr->float32_values_default[2]);

  EXPECT_EQ(3.1415, arr->float64_values_default[0]);
  EXPECT_EQ(0.0, arr->float64_values_default[1]);
  EXPECT_EQ(-3.1415, arr->float64_values_default[2]);

  EXPECT_EQ(0, arr->int8_values_default[0]);
  EXPECT_EQ(INT8_MAX, arr->int8_values_default[1]);
  EXPECT_EQ(INT8_MIN, arr->int8_values_default[2]);

  EXPECT_EQ(0, arr->uint8_values_default[0]);
  EXPECT_EQ(1, arr->uint8_values_default[1]);
  EXPECT_EQ(UINT8_MAX, arr->uint8_values_default[2]);

  EXPECT_EQ(0, arr->int16_values_default[0]);
  EXPECT_EQ(INT16_MAX, arr->int16_values_default[1]);
  EXPECT_EQ(INT16_MIN, arr->int16_values_default[2]);

  EXPECT_EQ(0, arr->uint16_values_default[0]);
  EXPECT_EQ(1, arr->uint16_values_default[1]);
  EXPECT_EQ(UINT16_MAX, arr->uint16_values_default[2]);

  EXPECT_EQ(0, arr->int32_values_default[0]);
  EXPECT_EQ(INT32_MAX, arr->int32_values_default[1]);
  EXPECT_EQ(INT32_MIN, arr->int32_values_default[2]);

  EXPECT_EQ(0, arr->uint32_values_default[0]);
  EXPECT_EQ(1, arr->uint32_values_default[1]);
  EXPECT_EQ(UINT32_MAX, arr->uint32_values_default[2]);

  EXPECT_EQ(0, arr->int64_values_default[0]);
  EXPECT_EQ(INT64_MAX, arr->int64_values_default[1]);
  EXPECT_EQ(INT64_MIN, arr->int64_values_default[2]);

  EXPECT_EQ(0, arr->uint64_values_default[0]);
  EXPECT_EQ(1, arr->uint64_values_default[1]);
  EXPECT_EQ(UINT64_MAX, arr->uint64_values_default[2]);

  EXPECT_EQ(0, strcmp(arr->string_values_default[0].data, ""));
  EXPECT_EQ(0, strcmp(arr->string_values_default[1].data, "max value"));
  EXPECT_EQ(0, strcmp(arr->string_values_default[2].data, "min value"));

  EXPECT_FALSE(rosidl_generator_c__msg__Arrays__are_equal(NULL, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__Arrays__are_equal(arr, NULL));
  EXPECT_FALSE(rosidl_generator_c__msg__Arrays__are_equal(NULL, arr));
  EXPECT_TRUE(rosidl_generator_c__msg__Arrays__are_equal(arr, arr));

  rosidl_generator_c__msg__Arrays * arr_copy = NULL;
  arr_copy = rosidl_generator_c__msg__Arrays__create();
  EXPECT_NE(arr_copy, NULL);
  EXPECT_FALSE(rosidl_generator_c__msg__Arrays__are_equal(arr, arr_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__Arrays__copy(arr, arr_copy));
  EXPECT_TRUE(rosidl_generator_c__msg__Arrays__are_equal(arr, arr_copy));
  rosidl_generator_c__msg__Arrays__destroy(arr_copy);

  rosidl_generator_c__msg__Arrays__destroy(arr);
  return 0;
}
