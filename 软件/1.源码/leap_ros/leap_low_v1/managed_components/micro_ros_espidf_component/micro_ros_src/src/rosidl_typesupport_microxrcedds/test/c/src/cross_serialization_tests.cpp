// Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

#include <cstdbool>

#include "rosidl_runtime_c/message_type_support_struct.h"

#include <rosidl_typesupport_microxrcedds_c/identifier.h>
#include <rosidl_typesupport_fastrtps_c/identifier.h>
#include <rosidl_typesupport_introspection_c/identifier.h>

// Workaround for having two coexistent message_type_support_callbacks_t structs
#define message_type_support_callbacks_t message_type_support_callbacks_fast_t
#include <rosidl_typesupport_fastrtps_cpp/message_type_support.h>
#undef message_type_support_callbacks_t
#define message_type_support_callbacks_t message_type_support_callbacks_xrce_t
#include <rosidl_typesupport_microxrcedds_c/message_type_support.h>
#undef message_type_support_callbacks_t

#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosidl_typesupport_introspection_c/field_types.h"

#include <rosidl_runtime_c/string.h>
#include <rosidl_runtime_c/string_functions.h>

typedef struct genericSequence {
  void * data;
  size_t size;
  size_t capacity;
} genericSequence;

template <typename T>
class CrossSerialization {
  public:

  #define QUICK_RETURN(x) if(!(x)) return x;

  CrossSerialization(const rosidl_message_type_support_t * type_support_c) :
    xrce_buffer(nullptr),
    xrce_size(0),
    fast_buffer(nullptr),
    fast_size(0)
  {
    xrce_typesupport = get_message_typesupport_handle(type_support_c, rosidl_typesupport_microxrcedds_c__identifier);
    fast_typesupport = get_message_typesupport_handle(type_support_c, rosidl_typesupport_fastrtps_c__identifier);
    introspection_typesupport = get_message_typesupport_handle(type_support_c, rosidl_typesupport_introspection_c__identifier);

    xrce_callbacks = static_cast<const message_type_support_callbacks_xrce_t *>(xrce_typesupport->data);
    fast_callbacks = static_cast<const message_type_support_callbacks_fast_t *>(fast_typesupport->data);
    introspection = (rosidl_typesupport_introspection_c__MessageMembers*) introspection_typesupport->data;
  };

  bool check() {
    bool ret = true;
    ret &= xrce_typesupport != nullptr;
    ret &= fast_typesupport != nullptr;
    ret &= introspection_typesupport != nullptr;
    ret &= xrce_callbacks != nullptr;
    ret &= fast_callbacks != nullptr;
    ret &= introspection != nullptr;
    return ret;
  }

  ~CrossSerialization() {
    if(nullptr != xrce_buffer) {
      free(xrce_buffer);
    }
    if(nullptr != fast_buffer) {
      free(fast_buffer);
    }
  }

  bool serialize_and_compare_buffers(T & msg) {
    xrce_size = xrce_callbacks->get_serialized_size(&msg);
    fast_size = fast_callbacks->get_serialized_size(&msg);
    QUICK_RETURN(xrce_size == fast_size);

    xrce_buffer = (uint8_t *) calloc(xrce_size, sizeof(uint8_t));
    ucdrBuffer xrce_cdr;
    ucdr_init_buffer(&xrce_cdr, xrce_buffer, xrce_size);
    QUICK_RETURN(xrce_callbacks->cdr_serialize(&msg, &xrce_cdr));

    fast_buffer = (uint8_t *) calloc(fast_size, sizeof(uint8_t));
    eprosima::fastcdr::FastBuffer fast_cdr_buffer((char *) fast_buffer, fast_size);
    eprosima::fastcdr::Cdr fast_cdr(fast_cdr_buffer);
    QUICK_RETURN(fast_callbacks->cdr_serialize(&msg, fast_cdr));

    QUICK_RETURN(xrce_cdr.offset == fast_cdr.getSerializedDataLength());

    bool equal_buffers = memcmp(xrce_buffer, fast_buffer, xrce_size) == 0;
    QUICK_RETURN(equal_buffers);

    return true;
  }

  bool deserialize_with_fastcdr(uint8_t * buff, size_t size, T & out) {
    eprosima::fastcdr::FastBuffer input_buffer((char *) buff, size);
    eprosima::fastcdr::Cdr cdr(input_buffer);

    QUICK_RETURN(fast_callbacks->cdr_deserialize(cdr, &out));

    return true;
  }

  bool deserialize_with_microcdr(uint8_t * buff, size_t size, T & out) {
    ucdrBuffer cdr;
    ucdr_init_buffer(&cdr, buff, size);

    QUICK_RETURN(xrce_callbacks->cdr_deserialize(&cdr, &out));

    return true;
  }

  uint8_t * xrce_buffer;
  uint8_t * fast_buffer;

  size_t xrce_size;
  size_t fast_size;

  const rosidl_message_type_support_t * xrce_typesupport;
  const rosidl_message_type_support_t * fast_typesupport;
  const rosidl_message_type_support_t * introspection_typesupport;

  const message_type_support_callbacks_xrce_t * xrce_callbacks;
  const message_type_support_callbacks_fast_t * fast_callbacks;

  rosidl_typesupport_introspection_c__MessageMembers * introspection;
};

#include <rosidl_typesupport_microxrcedds_test_msg/msg/regression10.h>

TEST(SerDesTests, Regression10) {
  using DataType = rosidl_typesupport_microxrcedds_test_msg__msg__Regression10;
  CrossSerialization<DataType> serdes(ROSIDL_GET_MSG_TYPE_SUPPORT(rosidl_typesupport_microxrcedds_test_msg, msg, Regression10));

  ASSERT_TRUE(serdes.check());
  DataType msg = {};
  msg.a = 0x01;     // byte
  msg.b = 0x1111;   // uint16
  msg.c = 0x1010;   // uint16
  msg.d = 0x10;     // uint8

  // Regression11[10]
  for (size_t i = 0; i < sizeof(msg.e)/sizeof(msg.e[0]); i++) {
    msg.e[i].f = i + 0x01;       // byte
    msg.e[i].g = i + 0x0101;     // uint16
    msg.e[i].h = i + 0x10;       // byte
    msg.e[i].i = true;           // bool
    msg.e[i].j = i + 0x10101010; // int32
    msg.e[i].k = i + 0.1;        // float32
    msg.e[i].l = i + 0x1010;     // uint16
  }
  serdes.introspection->init_function(&msg, ROSIDL_RUNTIME_C_MSG_INIT_ALL);

  EXPECT_TRUE(serdes.serialize_and_compare_buffers(msg));

  DataType out = {};
  serdes.introspection->init_function(&out, ROSIDL_RUNTIME_C_MSG_INIT_ALL);

  EXPECT_TRUE(serdes.deserialize_with_microcdr(serdes.xrce_buffer, serdes.xrce_size, out));
  EXPECT_TRUE(serdes.deserialize_with_microcdr(serdes.fast_buffer, serdes.fast_size, out));
  EXPECT_TRUE(serdes.deserialize_with_fastcdr(serdes.xrce_buffer, serdes.xrce_size, out));

  serdes.introspection->fini_function(&msg);
  serdes.introspection->fini_function(&out);
}
