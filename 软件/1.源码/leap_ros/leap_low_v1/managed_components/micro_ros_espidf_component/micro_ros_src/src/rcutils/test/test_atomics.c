// Copyright 2019 Open Source Robotics Foundation, Inc.
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

#include <stdio.h>
#include "rcutils/stdatomic_helper.h"

// Cannot use gtest or C++ because stdatomic_helper.h forces a compiler error if C++ is used

#define TEST_ATOMIC_TYPE(BASE_TYPE, ATOMIC_TYPE) \
  do { \
    ATOMIC_TYPE uut; \
    atomic_init(&uut, (BASE_TYPE)0); \
    BASE_TYPE loaded_value; \
    rcutils_atomic_load(&uut, loaded_value); \
    if ((BASE_TYPE)0 != loaded_value) { \
      fprintf(stderr, "load test failed " #ATOMIC_TYPE " base " #BASE_TYPE "\n"); \
      return 1; \
    } \
    BASE_TYPE exchanged_value; \
    rcutils_atomic_store(&uut, (BASE_TYPE)28); \
    rcutils_atomic_exchange(&uut, exchanged_value, (BASE_TYPE)42); \
    rcutils_atomic_load(&uut, loaded_value); \
    if ((BASE_TYPE)28 != exchanged_value) { \
      fprintf(stderr, "exchange test failed " #ATOMIC_TYPE " base " #BASE_TYPE "\n"); \
      return 1; \
    } \
    if ((BASE_TYPE)42 != loaded_value) { \
      fprintf(stderr, "exchange test failed " #ATOMIC_TYPE " base " #BASE_TYPE "\n"); \
      return 1; \
    } \
  } while (0)

int
main()
{
  TEST_ATOMIC_TYPE(_Bool, atomic_bool);
  TEST_ATOMIC_TYPE(char, atomic_char);
  TEST_ATOMIC_TYPE(signed char, atomic_schar);
  TEST_ATOMIC_TYPE(unsigned char, atomic_uchar);
  TEST_ATOMIC_TYPE(short, atomic_short);  // NOLINT(runtime/int)
  TEST_ATOMIC_TYPE(unsigned short, atomic_ushort);  // NOLINT(runtime/int)
  TEST_ATOMIC_TYPE(int, atomic_int);
  TEST_ATOMIC_TYPE(unsigned int, atomic_uint);
  TEST_ATOMIC_TYPE(long, atomic_long);  // NOLINT(runtime/int)
  TEST_ATOMIC_TYPE(unsigned long, atomic_ulong);  // NOLINT(runtime/int)
  TEST_ATOMIC_TYPE(long long, atomic_llong);  // NOLINT(runtime/int)
  TEST_ATOMIC_TYPE(unsigned long long, atomic_ullong);  // NOLINT(runtime/int)
  TEST_ATOMIC_TYPE(int_least16_t, atomic_int_least16_t);
  TEST_ATOMIC_TYPE(uint_least16_t, atomic_uint_least16_t);
  TEST_ATOMIC_TYPE(int_least32_t, atomic_int_least32_t);
  TEST_ATOMIC_TYPE(uint_least32_t, atomic_uint_least32_t);
  TEST_ATOMIC_TYPE(int_least64_t, atomic_int_least64_t);
  TEST_ATOMIC_TYPE(uint_least64_t, atomic_uint_least64_t);
  TEST_ATOMIC_TYPE(int_fast16_t, atomic_int_fast16_t);
  TEST_ATOMIC_TYPE(uint_fast16_t, atomic_uint_fast16_t);
  TEST_ATOMIC_TYPE(int_fast32_t, atomic_int_fast32_t);
  TEST_ATOMIC_TYPE(uint_fast32_t, atomic_uint_fast32_t);
  TEST_ATOMIC_TYPE(int_fast64_t, atomic_int_fast64_t);
  TEST_ATOMIC_TYPE(uint_fast64_t, atomic_uint_fast64_t);
  TEST_ATOMIC_TYPE(intptr_t, atomic_intptr_t);
  TEST_ATOMIC_TYPE(uintptr_t, atomic_uintptr_t);
  TEST_ATOMIC_TYPE(size_t, atomic_size_t);
  TEST_ATOMIC_TYPE(ptrdiff_t, atomic_ptrdiff_t);
  TEST_ATOMIC_TYPE(intmax_t, atomic_intmax_t);
  TEST_ATOMIC_TYPE(uintmax_t, atomic_uintmax_t);

  TEST_ATOMIC_TYPE(int *, _Atomic(int *));
  TEST_ATOMIC_TYPE(int **, _Atomic(int **));
  return 0;
}
