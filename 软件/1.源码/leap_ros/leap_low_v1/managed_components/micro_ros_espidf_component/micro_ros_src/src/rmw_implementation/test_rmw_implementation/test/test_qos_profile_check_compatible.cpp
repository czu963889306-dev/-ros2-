// Copyright 2021 Open Source Robotics Foundation, Inc.
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

#include "rmw/qos_profiles.h"

#ifdef RMW_IMPLEMENTATION
# define CLASSNAME_(NAME, SUFFIX) NAME ## __ ## SUFFIX
# define CLASSNAME(NAME, SUFFIX) CLASSNAME_(NAME, SUFFIX)
#else
# define CLASSNAME(NAME, SUFFIX) NAME
#endif

TEST(CLASSNAME(TestQoSProfilesAreCompatible, RMW_IMPLEMENTATION), compatible) {
  // A profile without system default or unknown values should be compatible with itself
  rmw_qos_profile_t qos = {
    RMW_QOS_POLICY_HISTORY_KEEP_LAST,
    5,
    RMW_QOS_POLICY_RELIABILITY_BEST_EFFORT,
    RMW_QOS_POLICY_DURABILITY_VOLATILE,
    {1, 0},   // deadline
    {1, 0},   // lifespan
    RMW_QOS_POLICY_LIVELINESS_AUTOMATIC,
    {1, 0},   // liveliness lease duration
    false
  };
  rmw_qos_compatibility_type_t compatible;
  size_t reason_size = 100u;
  char reason[100];
  rmw_ret_t ret = rmw_qos_profile_check_compatible(qos, qos, &compatible, reason, reason_size);
  EXPECT_EQ(ret, RMW_RET_OK);
  EXPECT_EQ(compatible, RMW_QOS_COMPATIBILITY_OK);
}

TEST(CLASSNAME(TestQoSProfilesAreCompatible, RMW_IMPLEMENTATION), invalid_input) {
  // Error on null 'compatible' parameter
  {
    rmw_ret_t ret = rmw_qos_profile_check_compatible(
      rmw_qos_profile_sensor_data, rmw_qos_profile_sensor_data, nullptr, nullptr, 0u);
    EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT);
  }
  // Error on null reason and non-zero size
  {
    rmw_qos_compatibility_type_t compatible;
    rmw_ret_t ret = rmw_qos_profile_check_compatible(
      rmw_qos_profile_sensor_data, rmw_qos_profile_sensor_data, &compatible, nullptr, 1u);
    EXPECT_EQ(ret, RMW_RET_INVALID_ARGUMENT);
  }
}
