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
#include "rosidl_runtime_c/service_type_support_struct.h"

const rosidl_service_type_support_t * dummy_service_typesupport_handle_function(
  const rosidl_service_type_support_t * handle, const char *) {return handle;}

TEST(service_typesupport, get_service_typesupport_handle) {
  rosidl_service_type_support_t service_typesupport;

  constexpr char identifier[] = "identifier";
  service_typesupport.typesupport_identifier = &identifier[0];
  service_typesupport.func = dummy_service_typesupport_handle_function;

  EXPECT_EQ(
    get_service_typesupport_handle(
      &service_typesupport,
      &identifier[0]), &service_typesupport);
  EXPECT_EQ(
    get_service_typesupport_handle_function(
      &service_typesupport,
      &identifier[0]), &service_typesupport);
  EXPECT_EQ(
    get_service_typesupport_handle_function(
      &service_typesupport,
      "different identifier"), nullptr);
}
