// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from lifecycle_msgs:srv/GetAvailableTransitions.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "lifecycle_msgs/srv/detail/get_available_transitions__rosidl_typesupport_introspection_c.h"
#include "lifecycle_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "lifecycle_msgs/srv/detail/get_available_transitions__functions.h"
#include "lifecycle_msgs/srv/detail/get_available_transitions__struct.h"


#ifdef __cplusplus
extern "C"
{
#endif

void lifecycle_msgs__srv__GetAvailableTransitions_Request__rosidl_typesupport_introspection_c__GetAvailableTransitions_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  lifecycle_msgs__srv__GetAvailableTransitions_Request__init(message_memory);
}

void lifecycle_msgs__srv__GetAvailableTransitions_Request__rosidl_typesupport_introspection_c__GetAvailableTransitions_Request_fini_function(void * message_memory)
{
  lifecycle_msgs__srv__GetAvailableTransitions_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember lifecycle_msgs__srv__GetAvailableTransitions_Request__rosidl_typesupport_introspection_c__GetAvailableTransitions_Request_message_member_array[1] = {
  {
    "structure_needs_at_least_one_member",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_UINT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lifecycle_msgs__srv__GetAvailableTransitions_Request, structure_needs_at_least_one_member),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers lifecycle_msgs__srv__GetAvailableTransitions_Request__rosidl_typesupport_introspection_c__GetAvailableTransitions_Request_message_members = {
  "lifecycle_msgs__srv",  // message namespace
  "GetAvailableTransitions_Request",  // message name
  1,  // number of fields
  sizeof(lifecycle_msgs__srv__GetAvailableTransitions_Request),
  lifecycle_msgs__srv__GetAvailableTransitions_Request__rosidl_typesupport_introspection_c__GetAvailableTransitions_Request_message_member_array,  // message members
  lifecycle_msgs__srv__GetAvailableTransitions_Request__rosidl_typesupport_introspection_c__GetAvailableTransitions_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  lifecycle_msgs__srv__GetAvailableTransitions_Request__rosidl_typesupport_introspection_c__GetAvailableTransitions_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t lifecycle_msgs__srv__GetAvailableTransitions_Request__rosidl_typesupport_introspection_c__GetAvailableTransitions_Request_message_type_support_handle = {
  0,
  &lifecycle_msgs__srv__GetAvailableTransitions_Request__rosidl_typesupport_introspection_c__GetAvailableTransitions_Request_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_lifecycle_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lifecycle_msgs, srv, GetAvailableTransitions_Request)() {
  if (!lifecycle_msgs__srv__GetAvailableTransitions_Request__rosidl_typesupport_introspection_c__GetAvailableTransitions_Request_message_type_support_handle.typesupport_identifier) {
    lifecycle_msgs__srv__GetAvailableTransitions_Request__rosidl_typesupport_introspection_c__GetAvailableTransitions_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &lifecycle_msgs__srv__GetAvailableTransitions_Request__rosidl_typesupport_introspection_c__GetAvailableTransitions_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "lifecycle_msgs/srv/detail/get_available_transitions__rosidl_typesupport_introspection_c.h"
// already included above
// #include "lifecycle_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "lifecycle_msgs/srv/detail/get_available_transitions__functions.h"
// already included above
// #include "lifecycle_msgs/srv/detail/get_available_transitions__struct.h"


// Include directives for member types
// Member `available_transitions`
#include "lifecycle_msgs/msg/transition_description.h"
// Member `available_transitions`
#include "lifecycle_msgs/msg/detail/transition_description__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  lifecycle_msgs__srv__GetAvailableTransitions_Response__init(message_memory);
}

void lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_fini_function(void * message_memory)
{
  lifecycle_msgs__srv__GetAvailableTransitions_Response__fini(message_memory);
}

size_t lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__size_function__GetAvailableTransitions_Response__available_transitions(
  const void * untyped_member)
{
  const lifecycle_msgs__msg__TransitionDescription__Sequence * member =
    (const lifecycle_msgs__msg__TransitionDescription__Sequence *)(untyped_member);
  return member->size;
}

const void * lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__get_const_function__GetAvailableTransitions_Response__available_transitions(
  const void * untyped_member, size_t index)
{
  const lifecycle_msgs__msg__TransitionDescription__Sequence * member =
    (const lifecycle_msgs__msg__TransitionDescription__Sequence *)(untyped_member);
  return &member->data[index];
}

void * lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__get_function__GetAvailableTransitions_Response__available_transitions(
  void * untyped_member, size_t index)
{
  lifecycle_msgs__msg__TransitionDescription__Sequence * member =
    (lifecycle_msgs__msg__TransitionDescription__Sequence *)(untyped_member);
  return &member->data[index];
}

void lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__fetch_function__GetAvailableTransitions_Response__available_transitions(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const lifecycle_msgs__msg__TransitionDescription * item =
    ((const lifecycle_msgs__msg__TransitionDescription *)
    lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__get_const_function__GetAvailableTransitions_Response__available_transitions(untyped_member, index));
  lifecycle_msgs__msg__TransitionDescription * value =
    (lifecycle_msgs__msg__TransitionDescription *)(untyped_value);
  *value = *item;
}

void lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__assign_function__GetAvailableTransitions_Response__available_transitions(
  void * untyped_member, size_t index, const void * untyped_value)
{
  lifecycle_msgs__msg__TransitionDescription * item =
    ((lifecycle_msgs__msg__TransitionDescription *)
    lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__get_function__GetAvailableTransitions_Response__available_transitions(untyped_member, index));
  const lifecycle_msgs__msg__TransitionDescription * value =
    (const lifecycle_msgs__msg__TransitionDescription *)(untyped_value);
  *item = *value;
}

bool lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__resize_function__GetAvailableTransitions_Response__available_transitions(
  void * untyped_member, size_t size)
{
  lifecycle_msgs__msg__TransitionDescription__Sequence * member =
    (lifecycle_msgs__msg__TransitionDescription__Sequence *)(untyped_member);
  lifecycle_msgs__msg__TransitionDescription__Sequence__fini(member);
  return lifecycle_msgs__msg__TransitionDescription__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_message_member_array[1] = {
  {
    "available_transitions",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(lifecycle_msgs__srv__GetAvailableTransitions_Response, available_transitions),  // bytes offset in struct
    NULL,  // default value
    lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__size_function__GetAvailableTransitions_Response__available_transitions,  // size() function pointer
    lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__get_const_function__GetAvailableTransitions_Response__available_transitions,  // get_const(index) function pointer
    lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__get_function__GetAvailableTransitions_Response__available_transitions,  // get(index) function pointer
    lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__fetch_function__GetAvailableTransitions_Response__available_transitions,  // fetch(index, &value) function pointer
    lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__assign_function__GetAvailableTransitions_Response__available_transitions,  // assign(index, value) function pointer
    lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__resize_function__GetAvailableTransitions_Response__available_transitions  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_message_members = {
  "lifecycle_msgs__srv",  // message namespace
  "GetAvailableTransitions_Response",  // message name
  1,  // number of fields
  sizeof(lifecycle_msgs__srv__GetAvailableTransitions_Response),
  lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_message_member_array,  // message members
  lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_message_type_support_handle = {
  0,
  &lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_lifecycle_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lifecycle_msgs, srv, GetAvailableTransitions_Response)() {
  lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lifecycle_msgs, msg, TransitionDescription)();
  if (!lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_message_type_support_handle.typesupport_identifier) {
    lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &lifecycle_msgs__srv__GetAvailableTransitions_Response__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "lifecycle_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "lifecycle_msgs/srv/detail/get_available_transitions__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers lifecycle_msgs__srv__detail__get_available_transitions__rosidl_typesupport_introspection_c__GetAvailableTransitions_service_members = {
  "lifecycle_msgs__srv",  // service namespace
  "GetAvailableTransitions",  // service name
  // these two fields are initialized below on the first access
  NULL,  // request message
  // lifecycle_msgs__srv__detail__get_available_transitions__rosidl_typesupport_introspection_c__GetAvailableTransitions_Request_message_type_support_handle,
  NULL  // response message
  // lifecycle_msgs__srv__detail__get_available_transitions__rosidl_typesupport_introspection_c__GetAvailableTransitions_Response_message_type_support_handle
};

static rosidl_service_type_support_t lifecycle_msgs__srv__detail__get_available_transitions__rosidl_typesupport_introspection_c__GetAvailableTransitions_service_type_support_handle = {
  0,
  &lifecycle_msgs__srv__detail__get_available_transitions__rosidl_typesupport_introspection_c__GetAvailableTransitions_service_members,
  get_service_typesupport_handle_function,
};

// Forward declaration of request/response type support functions
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lifecycle_msgs, srv, GetAvailableTransitions_Request)();

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lifecycle_msgs, srv, GetAvailableTransitions_Response)();

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_lifecycle_msgs
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lifecycle_msgs, srv, GetAvailableTransitions)() {
  if (!lifecycle_msgs__srv__detail__get_available_transitions__rosidl_typesupport_introspection_c__GetAvailableTransitions_service_type_support_handle.typesupport_identifier) {
    lifecycle_msgs__srv__detail__get_available_transitions__rosidl_typesupport_introspection_c__GetAvailableTransitions_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)lifecycle_msgs__srv__detail__get_available_transitions__rosidl_typesupport_introspection_c__GetAvailableTransitions_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lifecycle_msgs, srv, GetAvailableTransitions_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, lifecycle_msgs, srv, GetAvailableTransitions_Response)()->data;
  }

  return &lifecycle_msgs__srv__detail__get_available_transitions__rosidl_typesupport_introspection_c__GetAvailableTransitions_service_type_support_handle;
}
