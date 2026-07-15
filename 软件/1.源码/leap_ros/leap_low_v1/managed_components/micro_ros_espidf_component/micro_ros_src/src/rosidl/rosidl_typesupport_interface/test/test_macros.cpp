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

#include "rosidl_typesupport_interface/macros.h"

#define a__get_message_type_support_handle__b__c__d SUCCEED(); return;
#define a__get_service_type_support_handle__b__c__d SUCCEED(); return;
#define a__get_action_type_support_handle__b__c__d SUCCEED(); return;

TEST(rosidl_typesupport_interface, message_symbol_name) {
  ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(a, b, c, d);
  FAIL() << "This line should not be reached";
}

TEST(rosidl_typesupport_interface, service_symbol_name) {
  ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(a, b, c, d);
  FAIL() << "This line should not be reached";
}

TEST(rosidl_typesupport_interface, action_symbol_name) {
  ROSIDL_TYPESUPPORT_INTERFACE__ACTION_SYMBOL_NAME(a, b, c, d);
  FAIL() << "This line should not be reached";
}
