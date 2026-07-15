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
#include "rmw/types.h"

TEST(test_types, zero_initialized_message_info) {
  rmw_message_info_t info = rmw_get_zero_initialized_message_info();
  EXPECT_EQ(0u, info.source_timestamp);
  EXPECT_EQ(0u, info.received_timestamp);
  EXPECT_EQ(nullptr, info.publisher_gid.implementation_identifier);
  for (size_t i = 0; i < RMW_GID_STORAGE_SIZE; ++i) {
    EXPECT_EQ(0u, info.publisher_gid.data[i]);
  }

  EXPECT_FALSE(info.from_intra_process);
}
