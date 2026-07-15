// Copyright 2018 Apex.AI, Inc.
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

#include "./impl/add_to_arrays.h"

#define ADD_VALUE_TO_SIMPLE_ARRAY(val_array, value, value_type, allocator) \
  do { \
    if (NULL == val_array->values) { \
      val_array->values = value; \
      val_array->size = 1; \
    } else { \
      /* Increase the array size by one and add the new value */ \
      value_type * tmp_arr = val_array->values; \
      val_array->values = allocator.zero_allocate( \
        val_array->size + 1U, sizeof(value_type), allocator.state); \
      if (NULL == val_array->values) { \
        val_array->values = tmp_arr; \
        RCUTILS_SAFE_FWRITE_TO_STDERR("Error allocating mem\n"); \
        return RCUTILS_RET_BAD_ALLOC; \
      } \
      memcpy(val_array->values, tmp_arr, (val_array->size * sizeof(value_type))); \
      val_array->values[val_array->size] = *value; \
      val_array->size++; \
      allocator.deallocate(value, allocator.state); \
      allocator.deallocate(tmp_arr, allocator.state); \
    } \
    return RCUTILS_RET_OK; \
  } while (0)

rcutils_ret_t add_val_to_bool_arr(
  rcl_bool_array_t * const val_array,
  bool * value,
  const rcutils_allocator_t allocator)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(val_array, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(value, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
    &allocator, "invalid allocator", return RCUTILS_RET_INVALID_ARGUMENT);

  ADD_VALUE_TO_SIMPLE_ARRAY(val_array, value, bool, allocator);
}

///
/// Add a value to an integer array. Create the array if it does not exist
///
rcutils_ret_t add_val_to_int_arr(
  rcl_int64_array_t * const val_array,
  int64_t * value,
  const rcutils_allocator_t allocator)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(val_array, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(value, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
    &allocator, "invalid allocator", return RCUTILS_RET_INVALID_ARGUMENT);

  ADD_VALUE_TO_SIMPLE_ARRAY(val_array, value, int64_t, allocator);
}

///
/// Add a value to a double array. Create the array if it does not exist
///
rcutils_ret_t add_val_to_double_arr(
  rcl_double_array_t * const val_array,
  double * value,
  const rcutils_allocator_t allocator)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(val_array, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(value, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
    &allocator, "invalid allocator", return RCUTILS_RET_INVALID_ARGUMENT);

  ADD_VALUE_TO_SIMPLE_ARRAY(val_array, value, double, allocator);
}

///
/// Add a value to a string array. Create the array if it does not exist
///
rcutils_ret_t add_val_to_string_arr(
  rcutils_string_array_t * const val_array,
  char * value,
  const rcutils_allocator_t allocator)
{
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(val_array, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ARGUMENT_FOR_NULL(value, RCUTILS_RET_INVALID_ARGUMENT);
  RCUTILS_CHECK_ALLOCATOR_WITH_MSG(
    &allocator, "invalid allocator", return RCUTILS_RET_INVALID_ARGUMENT);

  if (NULL == val_array->data) {
    rcutils_ret_t ret = rcutils_string_array_init(val_array, 1, &allocator);
    if (RCUTILS_RET_OK != ret) {
      return ret;
    }
    val_array->data[0U] = value;
  } else {
    /// Increase the array size by one and add the new value
    char ** new_string_arr_ptr = allocator.reallocate(
      val_array->data,
      ((val_array->size + 1U) * sizeof(char *)), allocator.state);
    if (NULL == new_string_arr_ptr) {
      RCUTILS_SAFE_FWRITE_TO_STDERR("Error allocating mem\n");
      return RCUTILS_RET_BAD_ALLOC;
    }
    val_array->data = new_string_arr_ptr;
    val_array->data[val_array->size] = value;
    val_array->size++;
  }
  return RCUTILS_RET_OK;
}
