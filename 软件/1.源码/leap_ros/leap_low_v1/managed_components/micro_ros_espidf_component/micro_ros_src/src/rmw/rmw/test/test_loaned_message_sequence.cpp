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

#include "gmock/gmock.h"
#include "rmw/loaned_message_sequence.h"

TEST(rmw_loaned_message_sequence, get_zero_init)
{
  rmw_loaned_message_sequence_t sequence = rmw_get_zero_initialized_loaned_message_sequence();
  EXPECT_EQ(sequence.message_sequence, nullptr);
  EXPECT_EQ(sequence.size, 0u);
  EXPECT_EQ(sequence.capacity, 0u);
}
