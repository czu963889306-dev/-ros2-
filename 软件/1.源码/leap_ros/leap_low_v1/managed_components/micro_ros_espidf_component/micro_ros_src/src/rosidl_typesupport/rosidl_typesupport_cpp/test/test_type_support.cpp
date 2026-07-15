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

// Provide a symbol so they can be checked with get_typesupport_handle_function()

#include "rosidl_runtime_c/message_type_support_struct.h"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_typesupport_cpp/visibility_control.h"

#ifdef __cplusplus
extern "C"
{
#endif

// If ROSIDL_TYPESUPPORT_CPP_PUBLIC is used, it selects dllimport instead of dllexport, but the
// function still needs to be defined separately. Windows has gotten picky with its compiler
// warnings recently.
#if defined _WIN32 || defined __CYGWIN__
__declspec(dllexport) const rosidl_message_type_support_t * test_message_type_support();
__declspec(dllexport) const rosidl_service_type_support_t * test_service_type_support();
#else
const rosidl_message_type_support_t * test_message_type_support();
const rosidl_service_type_support_t * test_service_type_support();
#endif

static const rosidl_message_type_support_t message_type_support = {
  0, 0, 0
};

static const rosidl_service_type_support_t service_type_support = {
  0, 0, 0
};

const rosidl_message_type_support_t * test_message_type_support() {return &message_type_support;}

const rosidl_service_type_support_t * test_service_type_support() {return &service_type_support;}

#ifdef __cplusplus
}
#endif
