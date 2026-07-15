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
#include "rmw/event.h"

TEST(rmw_event, get_zero_initialized_event)
{
  const rmw_event_t actual = rmw_get_zero_initialized_event();
  EXPECT_EQ(nullptr, actual.implementation_identifier);
  EXPECT_EQ(nullptr, actual.data);
  EXPECT_EQ(RMW_EVENT_INVALID, actual.event_type);
}

TEST(rmw_event, event_fini)
{
  EXPECT_EQ(rmw_event_fini(nullptr), RMW_RET_INVALID_ARGUMENT);

  rmw_event_t event;
  event.implementation_identifier = "identifier";
  char data[20] = "datadatadata";
  event.data = &data[0];
  event.event_type = RMW_EVENT_LIVELINESS_CHANGED;

  EXPECT_EQ(rmw_event_fini(&event), RMW_RET_OK);
  EXPECT_EQ(nullptr, event.implementation_identifier);
  EXPECT_EQ(nullptr, event.data);
  EXPECT_EQ(RMW_EVENT_INVALID, event.event_type);
}
