// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from rosgraph_msgs:msg/Node.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "rosgraph_msgs/msg/detail/node__rosidl_typesupport_introspection_c.h"
#include "rosgraph_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "rosgraph_msgs/msg/detail/node__functions.h"
#include "rosgraph_msgs/msg/detail/node__struct.h"


// Include directives for member types
// Member `name`
#include "rosidl_runtime_c/string_functions.h"
// Member `parameters`
#include "rcl_interfaces/msg/parameter_descriptor.h"
// Member `parameters`
#include "rcl_interfaces/msg/detail/parameter_descriptor__rosidl_typesupport_introspection_c.h"
// Member `parameter_values`
#include "rcl_interfaces/msg/parameter_value.h"
// Member `parameter_values`
#include "rcl_interfaces/msg/detail/parameter_value__rosidl_typesupport_introspection_c.h"
// Member `publishers`
// Member `subscriptions`
#include "rosgraph_msgs/msg/topic.h"
// Member `publishers`
// Member `subscriptions`
#include "rosgraph_msgs/msg/detail/topic__rosidl_typesupport_introspection_c.h"
// Member `service_clients`
// Member `service_servers`
#include "rosgraph_msgs/msg/service.h"
// Member `service_clients`
// Member `service_servers`
#include "rosgraph_msgs/msg/detail/service__rosidl_typesupport_introspection_c.h"
// Member `action_clients`
// Member `action_servers`
#include "rosgraph_msgs/msg/action.h"
// Member `action_clients`
// Member `action_servers`
#include "rosgraph_msgs/msg/detail/action__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  rosgraph_msgs__msg__Node__init(message_memory);
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_fini_function(void * message_memory)
{
  rosgraph_msgs__msg__Node__fini(message_memory);
}

size_t rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__parameters(
  const void * untyped_member)
{
  const rcl_interfaces__msg__ParameterDescriptor__Sequence * member =
    (const rcl_interfaces__msg__ParameterDescriptor__Sequence *)(untyped_member);
  return member->size;
}

const void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__parameters(
  const void * untyped_member, size_t index)
{
  const rcl_interfaces__msg__ParameterDescriptor__Sequence * member =
    (const rcl_interfaces__msg__ParameterDescriptor__Sequence *)(untyped_member);
  return &member->data[index];
}

void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__parameters(
  void * untyped_member, size_t index)
{
  rcl_interfaces__msg__ParameterDescriptor__Sequence * member =
    (rcl_interfaces__msg__ParameterDescriptor__Sequence *)(untyped_member);
  return &member->data[index];
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__parameters(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rcl_interfaces__msg__ParameterDescriptor * item =
    ((const rcl_interfaces__msg__ParameterDescriptor *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__parameters(untyped_member, index));
  rcl_interfaces__msg__ParameterDescriptor * value =
    (rcl_interfaces__msg__ParameterDescriptor *)(untyped_value);
  *value = *item;
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__parameters(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rcl_interfaces__msg__ParameterDescriptor * item =
    ((rcl_interfaces__msg__ParameterDescriptor *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__parameters(untyped_member, index));
  const rcl_interfaces__msg__ParameterDescriptor * value =
    (const rcl_interfaces__msg__ParameterDescriptor *)(untyped_value);
  *item = *value;
}

bool rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__parameters(
  void * untyped_member, size_t size)
{
  rcl_interfaces__msg__ParameterDescriptor__Sequence * member =
    (rcl_interfaces__msg__ParameterDescriptor__Sequence *)(untyped_member);
  rcl_interfaces__msg__ParameterDescriptor__Sequence__fini(member);
  return rcl_interfaces__msg__ParameterDescriptor__Sequence__init(member, size);
}

size_t rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__parameter_values(
  const void * untyped_member)
{
  const rcl_interfaces__msg__ParameterValue__Sequence * member =
    (const rcl_interfaces__msg__ParameterValue__Sequence *)(untyped_member);
  return member->size;
}

const void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__parameter_values(
  const void * untyped_member, size_t index)
{
  const rcl_interfaces__msg__ParameterValue__Sequence * member =
    (const rcl_interfaces__msg__ParameterValue__Sequence *)(untyped_member);
  return &member->data[index];
}

void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__parameter_values(
  void * untyped_member, size_t index)
{
  rcl_interfaces__msg__ParameterValue__Sequence * member =
    (rcl_interfaces__msg__ParameterValue__Sequence *)(untyped_member);
  return &member->data[index];
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__parameter_values(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rcl_interfaces__msg__ParameterValue * item =
    ((const rcl_interfaces__msg__ParameterValue *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__parameter_values(untyped_member, index));
  rcl_interfaces__msg__ParameterValue * value =
    (rcl_interfaces__msg__ParameterValue *)(untyped_value);
  *value = *item;
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__parameter_values(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rcl_interfaces__msg__ParameterValue * item =
    ((rcl_interfaces__msg__ParameterValue *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__parameter_values(untyped_member, index));
  const rcl_interfaces__msg__ParameterValue * value =
    (const rcl_interfaces__msg__ParameterValue *)(untyped_value);
  *item = *value;
}

bool rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__parameter_values(
  void * untyped_member, size_t size)
{
  rcl_interfaces__msg__ParameterValue__Sequence * member =
    (rcl_interfaces__msg__ParameterValue__Sequence *)(untyped_member);
  rcl_interfaces__msg__ParameterValue__Sequence__fini(member);
  return rcl_interfaces__msg__ParameterValue__Sequence__init(member, size);
}

size_t rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__publishers(
  const void * untyped_member)
{
  const rosgraph_msgs__msg__Topic__Sequence * member =
    (const rosgraph_msgs__msg__Topic__Sequence *)(untyped_member);
  return member->size;
}

const void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__publishers(
  const void * untyped_member, size_t index)
{
  const rosgraph_msgs__msg__Topic__Sequence * member =
    (const rosgraph_msgs__msg__Topic__Sequence *)(untyped_member);
  return &member->data[index];
}

void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__publishers(
  void * untyped_member, size_t index)
{
  rosgraph_msgs__msg__Topic__Sequence * member =
    (rosgraph_msgs__msg__Topic__Sequence *)(untyped_member);
  return &member->data[index];
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__publishers(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosgraph_msgs__msg__Topic * item =
    ((const rosgraph_msgs__msg__Topic *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__publishers(untyped_member, index));
  rosgraph_msgs__msg__Topic * value =
    (rosgraph_msgs__msg__Topic *)(untyped_value);
  *value = *item;
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__publishers(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosgraph_msgs__msg__Topic * item =
    ((rosgraph_msgs__msg__Topic *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__publishers(untyped_member, index));
  const rosgraph_msgs__msg__Topic * value =
    (const rosgraph_msgs__msg__Topic *)(untyped_value);
  *item = *value;
}

bool rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__publishers(
  void * untyped_member, size_t size)
{
  rosgraph_msgs__msg__Topic__Sequence * member =
    (rosgraph_msgs__msg__Topic__Sequence *)(untyped_member);
  rosgraph_msgs__msg__Topic__Sequence__fini(member);
  return rosgraph_msgs__msg__Topic__Sequence__init(member, size);
}

size_t rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__subscriptions(
  const void * untyped_member)
{
  const rosgraph_msgs__msg__Topic__Sequence * member =
    (const rosgraph_msgs__msg__Topic__Sequence *)(untyped_member);
  return member->size;
}

const void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__subscriptions(
  const void * untyped_member, size_t index)
{
  const rosgraph_msgs__msg__Topic__Sequence * member =
    (const rosgraph_msgs__msg__Topic__Sequence *)(untyped_member);
  return &member->data[index];
}

void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__subscriptions(
  void * untyped_member, size_t index)
{
  rosgraph_msgs__msg__Topic__Sequence * member =
    (rosgraph_msgs__msg__Topic__Sequence *)(untyped_member);
  return &member->data[index];
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__subscriptions(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosgraph_msgs__msg__Topic * item =
    ((const rosgraph_msgs__msg__Topic *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__subscriptions(untyped_member, index));
  rosgraph_msgs__msg__Topic * value =
    (rosgraph_msgs__msg__Topic *)(untyped_value);
  *value = *item;
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__subscriptions(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosgraph_msgs__msg__Topic * item =
    ((rosgraph_msgs__msg__Topic *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__subscriptions(untyped_member, index));
  const rosgraph_msgs__msg__Topic * value =
    (const rosgraph_msgs__msg__Topic *)(untyped_value);
  *item = *value;
}

bool rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__subscriptions(
  void * untyped_member, size_t size)
{
  rosgraph_msgs__msg__Topic__Sequence * member =
    (rosgraph_msgs__msg__Topic__Sequence *)(untyped_member);
  rosgraph_msgs__msg__Topic__Sequence__fini(member);
  return rosgraph_msgs__msg__Topic__Sequence__init(member, size);
}

size_t rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__service_clients(
  const void * untyped_member)
{
  const rosgraph_msgs__msg__Service__Sequence * member =
    (const rosgraph_msgs__msg__Service__Sequence *)(untyped_member);
  return member->size;
}

const void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__service_clients(
  const void * untyped_member, size_t index)
{
  const rosgraph_msgs__msg__Service__Sequence * member =
    (const rosgraph_msgs__msg__Service__Sequence *)(untyped_member);
  return &member->data[index];
}

void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__service_clients(
  void * untyped_member, size_t index)
{
  rosgraph_msgs__msg__Service__Sequence * member =
    (rosgraph_msgs__msg__Service__Sequence *)(untyped_member);
  return &member->data[index];
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__service_clients(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosgraph_msgs__msg__Service * item =
    ((const rosgraph_msgs__msg__Service *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__service_clients(untyped_member, index));
  rosgraph_msgs__msg__Service * value =
    (rosgraph_msgs__msg__Service *)(untyped_value);
  *value = *item;
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__service_clients(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosgraph_msgs__msg__Service * item =
    ((rosgraph_msgs__msg__Service *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__service_clients(untyped_member, index));
  const rosgraph_msgs__msg__Service * value =
    (const rosgraph_msgs__msg__Service *)(untyped_value);
  *item = *value;
}

bool rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__service_clients(
  void * untyped_member, size_t size)
{
  rosgraph_msgs__msg__Service__Sequence * member =
    (rosgraph_msgs__msg__Service__Sequence *)(untyped_member);
  rosgraph_msgs__msg__Service__Sequence__fini(member);
  return rosgraph_msgs__msg__Service__Sequence__init(member, size);
}

size_t rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__service_servers(
  const void * untyped_member)
{
  const rosgraph_msgs__msg__Service__Sequence * member =
    (const rosgraph_msgs__msg__Service__Sequence *)(untyped_member);
  return member->size;
}

const void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__service_servers(
  const void * untyped_member, size_t index)
{
  const rosgraph_msgs__msg__Service__Sequence * member =
    (const rosgraph_msgs__msg__Service__Sequence *)(untyped_member);
  return &member->data[index];
}

void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__service_servers(
  void * untyped_member, size_t index)
{
  rosgraph_msgs__msg__Service__Sequence * member =
    (rosgraph_msgs__msg__Service__Sequence *)(untyped_member);
  return &member->data[index];
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__service_servers(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosgraph_msgs__msg__Service * item =
    ((const rosgraph_msgs__msg__Service *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__service_servers(untyped_member, index));
  rosgraph_msgs__msg__Service * value =
    (rosgraph_msgs__msg__Service *)(untyped_value);
  *value = *item;
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__service_servers(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosgraph_msgs__msg__Service * item =
    ((rosgraph_msgs__msg__Service *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__service_servers(untyped_member, index));
  const rosgraph_msgs__msg__Service * value =
    (const rosgraph_msgs__msg__Service *)(untyped_value);
  *item = *value;
}

bool rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__service_servers(
  void * untyped_member, size_t size)
{
  rosgraph_msgs__msg__Service__Sequence * member =
    (rosgraph_msgs__msg__Service__Sequence *)(untyped_member);
  rosgraph_msgs__msg__Service__Sequence__fini(member);
  return rosgraph_msgs__msg__Service__Sequence__init(member, size);
}

size_t rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__action_clients(
  const void * untyped_member)
{
  const rosgraph_msgs__msg__Action__Sequence * member =
    (const rosgraph_msgs__msg__Action__Sequence *)(untyped_member);
  return member->size;
}

const void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__action_clients(
  const void * untyped_member, size_t index)
{
  const rosgraph_msgs__msg__Action__Sequence * member =
    (const rosgraph_msgs__msg__Action__Sequence *)(untyped_member);
  return &member->data[index];
}

void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__action_clients(
  void * untyped_member, size_t index)
{
  rosgraph_msgs__msg__Action__Sequence * member =
    (rosgraph_msgs__msg__Action__Sequence *)(untyped_member);
  return &member->data[index];
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__action_clients(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosgraph_msgs__msg__Action * item =
    ((const rosgraph_msgs__msg__Action *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__action_clients(untyped_member, index));
  rosgraph_msgs__msg__Action * value =
    (rosgraph_msgs__msg__Action *)(untyped_value);
  *value = *item;
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__action_clients(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosgraph_msgs__msg__Action * item =
    ((rosgraph_msgs__msg__Action *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__action_clients(untyped_member, index));
  const rosgraph_msgs__msg__Action * value =
    (const rosgraph_msgs__msg__Action *)(untyped_value);
  *item = *value;
}

bool rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__action_clients(
  void * untyped_member, size_t size)
{
  rosgraph_msgs__msg__Action__Sequence * member =
    (rosgraph_msgs__msg__Action__Sequence *)(untyped_member);
  rosgraph_msgs__msg__Action__Sequence__fini(member);
  return rosgraph_msgs__msg__Action__Sequence__init(member, size);
}

size_t rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__action_servers(
  const void * untyped_member)
{
  const rosgraph_msgs__msg__Action__Sequence * member =
    (const rosgraph_msgs__msg__Action__Sequence *)(untyped_member);
  return member->size;
}

const void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__action_servers(
  const void * untyped_member, size_t index)
{
  const rosgraph_msgs__msg__Action__Sequence * member =
    (const rosgraph_msgs__msg__Action__Sequence *)(untyped_member);
  return &member->data[index];
}

void * rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__action_servers(
  void * untyped_member, size_t index)
{
  rosgraph_msgs__msg__Action__Sequence * member =
    (rosgraph_msgs__msg__Action__Sequence *)(untyped_member);
  return &member->data[index];
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__action_servers(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const rosgraph_msgs__msg__Action * item =
    ((const rosgraph_msgs__msg__Action *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__action_servers(untyped_member, index));
  rosgraph_msgs__msg__Action * value =
    (rosgraph_msgs__msg__Action *)(untyped_value);
  *value = *item;
}

void rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__action_servers(
  void * untyped_member, size_t index, const void * untyped_value)
{
  rosgraph_msgs__msg__Action * item =
    ((rosgraph_msgs__msg__Action *)
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__action_servers(untyped_member, index));
  const rosgraph_msgs__msg__Action * value =
    (const rosgraph_msgs__msg__Action *)(untyped_value);
  *item = *value;
}

bool rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__action_servers(
  void * untyped_member, size_t size)
{
  rosgraph_msgs__msg__Action__Sequence * member =
    (rosgraph_msgs__msg__Action__Sequence *)(untyped_member);
  rosgraph_msgs__msg__Action__Sequence__fini(member);
  return rosgraph_msgs__msg__Action__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_member_array[9] = {
  {
    "name",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_STRING,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__Node, name),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "parameters",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__Node, parameters),  // bytes offset in struct
    NULL,  // default value
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__parameters,  // size() function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__parameters,  // get_const(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__parameters,  // get(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__parameters,  // fetch(index, &value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__parameters,  // assign(index, value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__parameters  // resize(index) function pointer
  },
  {
    "parameter_values",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__Node, parameter_values),  // bytes offset in struct
    NULL,  // default value
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__parameter_values,  // size() function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__parameter_values,  // get_const(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__parameter_values,  // get(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__parameter_values,  // fetch(index, &value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__parameter_values,  // assign(index, value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__parameter_values  // resize(index) function pointer
  },
  {
    "publishers",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__Node, publishers),  // bytes offset in struct
    NULL,  // default value
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__publishers,  // size() function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__publishers,  // get_const(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__publishers,  // get(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__publishers,  // fetch(index, &value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__publishers,  // assign(index, value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__publishers  // resize(index) function pointer
  },
  {
    "subscriptions",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__Node, subscriptions),  // bytes offset in struct
    NULL,  // default value
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__subscriptions,  // size() function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__subscriptions,  // get_const(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__subscriptions,  // get(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__subscriptions,  // fetch(index, &value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__subscriptions,  // assign(index, value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__subscriptions  // resize(index) function pointer
  },
  {
    "service_clients",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__Node, service_clients),  // bytes offset in struct
    NULL,  // default value
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__service_clients,  // size() function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__service_clients,  // get_const(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__service_clients,  // get(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__service_clients,  // fetch(index, &value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__service_clients,  // assign(index, value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__service_clients  // resize(index) function pointer
  },
  {
    "service_servers",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__Node, service_servers),  // bytes offset in struct
    NULL,  // default value
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__service_servers,  // size() function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__service_servers,  // get_const(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__service_servers,  // get(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__service_servers,  // fetch(index, &value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__service_servers,  // assign(index, value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__service_servers  // resize(index) function pointer
  },
  {
    "action_clients",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__Node, action_clients),  // bytes offset in struct
    NULL,  // default value
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__action_clients,  // size() function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__action_clients,  // get_const(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__action_clients,  // get(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__action_clients,  // fetch(index, &value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__action_clients,  // assign(index, value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__action_clients  // resize(index) function pointer
  },
  {
    "action_servers",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(rosgraph_msgs__msg__Node, action_servers),  // bytes offset in struct
    NULL,  // default value
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__size_function__Node__action_servers,  // size() function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_const_function__Node__action_servers,  // get_const(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__get_function__Node__action_servers,  // get(index) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__fetch_function__Node__action_servers,  // fetch(index, &value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__assign_function__Node__action_servers,  // assign(index, value) function pointer
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__resize_function__Node__action_servers  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_members = {
  "rosgraph_msgs__msg",  // message namespace
  "Node",  // message name
  9,  // number of fields
  sizeof(rosgraph_msgs__msg__Node),
  rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_member_array,  // message members
  rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_init_function,  // function to initialize message memory (memory has to be allocated)
  rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_type_support_handle = {
  0,
  &rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_rosgraph_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, Node)() {
  rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rcl_interfaces, msg, ParameterDescriptor)();
  rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_member_array[2].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rcl_interfaces, msg, ParameterValue)();
  rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_member_array[3].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, Topic)();
  rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_member_array[4].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, Topic)();
  rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_member_array[5].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, Service)();
  rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_member_array[6].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, Service)();
  rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_member_array[7].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, Action)();
  rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_member_array[8].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, rosgraph_msgs, msg, Action)();
  if (!rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_type_support_handle.typesupport_identifier) {
    rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &rosgraph_msgs__msg__Node__rosidl_typesupport_introspection_c__Node_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif
