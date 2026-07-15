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

#ifndef TEST_ARRAY_GENERATOR_HPP_
#define TEST_ARRAY_GENERATOR_HPP_

#include <climits>
#include <random>
#include <string>
#include <type_traits>

/**
 * Helper function to generate a test pattern for boolean type.
 * Alternating true (even index) and false (odd index) pattern.
 * @param C Container (vector, array, etc) to be filled
 * @param size How many elements to fill in. Must size<=container_size
 */
template<
  typename C,
  typename std::enable_if<
    std::is_same<typename C::value_type, bool>::value
  >::type * = nullptr
>
void test_vector_fill(
  C * container, size_t size, bool val1 = true, bool val2 = false)
{
  for (size_t i = 0; i < size; i++) {
    if ((i % 2) == 0) {
      (*container)[i] = val2;
    } else {
      (*container)[i] = val1;
    }
  }
}

/**
 * Helper function to generate a test pattern for integer number types.
 * The template type parameter must be an integer number type.
 * Mininum and maximum values for the type and distributed values in the middle.
 * @param C Container (vector, array, etc) to be filled
 * @param size How many elements to fill in. Must size<=container_size
 * @param min Minimum value in the range to fill.
 * @param max Maximum value in the range to fill.
 */
template<
  typename C,
  typename std::enable_if<
    std::is_integral<typename C::value_type>::value &&
    !std::is_same<typename C::value_type, bool>::value
  >::type * = nullptr
>
void test_vector_fill(
  C * container, size_t size,
  typename C::value_type min, typename C::value_type max)
{
  if (size > 0 && min < max) {
    size_t step = (max - min) / size;
    (*container)[0] = min;
    for (size_t i = 1; i < size - 1; i++) {
      (*container)[i] = min + static_cast<typename C::value_type>(i * step);
    }
    (*container)[size - 1] = max;
  }
}

/**
 * Helper function to generate a test pattern for float number types.
 * Mininum and maximum values for the type and distributed values in the middle.
 * @param C Container (vector, array, etc) to be filled
 * @param size How many elements to fill in. Must size<=container_size
 * @param min Minimum value in the range to fill.
 * @param max Maximum value in the range to fill.
 */
template<
  typename C,
  typename std::enable_if<
    std::is_floating_point<typename C::value_type>::value
  >::type * = nullptr
>
void test_vector_fill(
  C * container, size_t size,
  typename C::value_type min, typename C::value_type max)
{
  if (size > 0 && min < max) {
    typename C::value_type step = (max - min) / static_cast<typename C::value_type>(size);
    (*container)[0] = min;
    for (size_t i = 1; i < size - 1; i++) {
      (*container)[i] = min + static_cast<typename C::value_type>(i * step);
    }
    (*container)[size - 1] = max;
  }
}

/**
 * Helper function to generate a test pattern for string types.
 * Mininum and maximum values for the type and distributed values in the middle.
 * @param C Container (vector, array, etc) to be filled
 * @param size How many elements to fill in. Must size<=container_size
 * @param min Minimum value in the range to fill.
 * @param max Maximum value in the range to fill.
 * @param minlength Minimum length of the generated strings.
 * @param maxlength Maximum length of the generated strings.
 */
template<
  typename C,
  typename std::enable_if<
    std::is_same<typename C::value_type, typename std::string>::value
  >::type * = nullptr
>
void test_vector_fill(
  C * container, size_t size,
  int min, int max,
  int minlength, const int maxlength)
{
  if (size > 0 && min < max && minlength < maxlength) {
    size_t step = (max - min) / size;
    size_t step_length = (maxlength - minlength) / size;
    char * tmpstr = static_cast<char *>(malloc(maxlength + 1));
    std::snprintf(tmpstr, minlength + 1, "%*d", minlength, min);
    (*container)[0] = std::string(tmpstr);
    for (size_t i = 1; i < size - 1; i++) {
      int value = min + static_cast<int>(i * step);
      int length = minlength + static_cast<int>(i * step_length);
      std::snprintf(tmpstr, length + 1, "%*d", length, value);
      (*container)[i] = std::string(tmpstr);
    }
    std::snprintf(tmpstr, maxlength + 1, "%*d", maxlength, max);
    (*container)[size - 1] = std::string(tmpstr);
    free(tmpstr);
  }
}

#endif  // TEST_ARRAY_GENERATOR_HPP_
