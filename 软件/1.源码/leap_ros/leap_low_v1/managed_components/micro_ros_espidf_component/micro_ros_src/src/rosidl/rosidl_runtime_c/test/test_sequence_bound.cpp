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

#include "gtest/gtest.h"
#include "rosidl_runtime_c/sequence_bound.h"

const rosidl_runtime_c__Sequence__bound * dummy_sequence_bound_handle_function(
  const rosidl_runtime_c__Sequence__bound * handle, const char *) {return handle;}

TEST(sequence_bound, get_sequence_bound_handle) {
  rosidl_runtime_c__Sequence__bound sequence_bound;

  constexpr char identifier[] = "identifier";
  sequence_bound.typesupport_identifier = &identifier[0];
  sequence_bound.func = dummy_sequence_bound_handle_function;

  EXPECT_EQ(get_sequence_bound_handle(&sequence_bound, &identifier[0]), &sequence_bound);
  EXPECT_EQ(get_sequence_bound_handle_function(&sequence_bound, &identifier[0]), &sequence_bound);
  EXPECT_EQ(get_sequence_bound_handle_function(&sequence_bound, "different identifier"), nullptr);
}
