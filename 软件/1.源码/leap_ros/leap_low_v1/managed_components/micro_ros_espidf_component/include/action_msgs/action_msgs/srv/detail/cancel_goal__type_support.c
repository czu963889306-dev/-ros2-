// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from action_msgs:srv/CancelGoal.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "action_msgs/srv/detail/cancel_goal__rosidl_typesupport_introspection_c.h"
#include "action_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "action_msgs/srv/detail/cancel_goal__functions.h"
#include "action_msgs/srv/detail/cancel_goal__struct.h"


// Include directives for member types
// Member `goal_info`
#include "action_msgs/msg/goal_info.h"
// Member `goal_info`
#include "action_msgs/msg/detail/goal_info__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void action_msgs__srv__CancelGoal_Request__rosidl_typesupport_introspection_c__CancelGoal_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  action_msgs__srv__CancelGoal_Request__init(message_memory);
}

void action_msgs__srv__CancelGoal_Request__rosidl_typesupport_introspection_c__CancelGoal_Request_fini_function(void * message_memory)
{
  action_msgs__srv__CancelGoal_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember action_msgs__srv__CancelGoal_Request__rosidl_typesupport_introspection_c__CancelGoal_Request_message_member_array[1] = {
  {
    "goal_info",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(action_msgs__srv__CancelGoal_Request, goal_info),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers action_msgs__srv__CancelGoal_Request__rosidl_typesupport_introspection_c__CancelGoal_Request_message_members = {
  "action_msgs__srv",  // message namespace
  "CancelGoal_Request",  // message name
  1,  // number of fields
  sizeof(action_msgs__srv__CancelGoal_Request),
  action_msgs__srv__CancelGoal_Request__rosidl_typesupport_introspection_c__CancelGoal_Request_message_member_array,  // message members
  action_msgs__srv__CancelGoal_Request__rosidl_typesupport_introspection_c__CancelGoal_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  action_msgs__srv__CancelGoal_Request__rosidl_typesupport_introspection_c__CancelGoal_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t action_msgs__srv__CancelGoal_Request__rosidl_typesupport_introspection_c__CancelGoal_Request_message_type_support_handle = {
  0,
  &action_msgs__srv__CancelGoal_Request__rosidl_typesupport_introspection_c__CancelGoal_Request_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_action_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, action_msgs, srv, CancelGoal_Request)() {
  action_msgs__srv__CancelGoal_Request__rosidl_typesupport_introspection_c__CancelGoal_Request_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, action_msgs, msg, GoalInfo)();
  if (!action_msgs__srv__CancelGoal_Request__rosidl_typesupport_introspection_c__CancelGoal_Request_message_type_support_handle.typesupport_identifier) {
    action_msgs__srv__CancelGoal_Request__rosidl_typesupport_introspection_c__CancelGoal_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &action_msgs__srv__CancelGoal_Request__rosidl_typesupport_introspection_c__CancelGoal_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "action_msgs/srv/detail/cancel_goal__rosidl_typesupport_introspection_c.h"
// already included above
// #include "action_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "action_msgs/srv/detail/cancel_goal__functions.h"
// already included above
// #include "action_msgs/srv/detail/cancel_goal__struct.h"


// Include directives for member types
// Member `goals_canceling`
// already included above
// #include "action_msgs/msg/goal_info.h"
// Member `goals_canceling`
// already included above
// #include "action_msgs/msg/detail/goal_info__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__CancelGoal_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  action_msgs__srv__CancelGoal_Response__init(message_memory);
}

void action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__CancelGoal_Response_fini_function(void * message_memory)
{
  action_msgs__srv__CancelGoal_Response__fini(message_memory);
}

size_t action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__size_function__CancelGoal_Response__goals_canceling(
  const void * untyped_member)
{
  const action_msgs__msg__GoalInfo__Sequence * member =
    (const action_msgs__msg__GoalInfo__Sequence *)(untyped_member);
  return member->size;
}

const void * action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__get_const_function__CancelGoal_Response__goals_canceling(
  const void * untyped_member, size_t index)
{
  const action_msgs__msg__GoalInfo__Sequence * member =
    (const action_msgs__msg__GoalInfo__Sequence *)(untyped_member);
  return &member->data[index];
}

void * action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__get_function__CancelGoal_Response__goals_canceling(
  void * untyped_member, size_t index)
{
  action_msgs__msg__GoalInfo__Sequence * member =
    (action_msgs__msg__GoalInfo__Sequence *)(untyped_member);
  return &member->data[index];
}

void action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__fetch_function__CancelGoal_Response__goals_canceling(
  const void * untyped_member, size_t index, void * untyped_value)
{
  const action_msgs__msg__GoalInfo * item =
    ((const action_msgs__msg__GoalInfo *)
    action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__get_const_function__CancelGoal_Response__goals_canceling(untyped_member, index));
  action_msgs__msg__GoalInfo * value =
    (action_msgs__msg__GoalInfo *)(untyped_value);
  *value = *item;
}

void action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__assign_function__CancelGoal_Response__goals_canceling(
  void * untyped_member, size_t index, const void * untyped_value)
{
  action_msgs__msg__GoalInfo * item =
    ((action_msgs__msg__GoalInfo *)
    action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__get_function__CancelGoal_Response__goals_canceling(untyped_member, index));
  const action_msgs__msg__GoalInfo * value =
    (const action_msgs__msg__GoalInfo *)(untyped_value);
  *item = *value;
}

bool action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__resize_function__CancelGoal_Response__goals_canceling(
  void * untyped_member, size_t size)
{
  action_msgs__msg__GoalInfo__Sequence * member =
    (action_msgs__msg__GoalInfo__Sequence *)(untyped_member);
  action_msgs__msg__GoalInfo__Sequence__fini(member);
  return action_msgs__msg__GoalInfo__Sequence__init(member, size);
}

static rosidl_typesupport_introspection_c__MessageMember action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__CancelGoal_Response_message_member_array[2] = {
  {
    "return_code",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_INT8,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(action_msgs__srv__CancelGoal_Response, return_code),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "goals_canceling",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    true,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(action_msgs__srv__CancelGoal_Response, goals_canceling),  // bytes offset in struct
    NULL,  // default value
    action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__size_function__CancelGoal_Response__goals_canceling,  // size() function pointer
    action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__get_const_function__CancelGoal_Response__goals_canceling,  // get_const(index) function pointer
    action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__get_function__CancelGoal_Response__goals_canceling,  // get(index) function pointer
    action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__fetch_function__CancelGoal_Response__goals_canceling,  // fetch(index, &value) function pointer
    action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__assign_function__CancelGoal_Response__goals_canceling,  // assign(index, value) function pointer
    action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__resize_function__CancelGoal_Response__goals_canceling  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__CancelGoal_Response_message_members = {
  "action_msgs__srv",  // message namespace
  "CancelGoal_Response",  // message name
  2,  // number of fields
  sizeof(action_msgs__srv__CancelGoal_Response),
  action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__CancelGoal_Response_message_member_array,  // message members
  action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__CancelGoal_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__CancelGoal_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__CancelGoal_Response_message_type_support_handle = {
  0,
  &action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__CancelGoal_Response_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_action_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, action_msgs, srv, CancelGoal_Response)() {
  action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__CancelGoal_Response_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, action_msgs, msg, GoalInfo)();
  if (!action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__CancelGoal_Response_message_type_support_handle.typesupport_identifier) {
    action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__CancelGoal_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &action_msgs__srv__CancelGoal_Response__rosidl_typesupport_introspection_c__CancelGoal_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "action_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "action_msgs/srv/detail/cancel_goal__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers action_msgs__srv__detail__cancel_goal__rosidl_typesupport_introspection_c__CancelGoal_service_members = {
  "action_msgs__srv",  // service namespace
  "CancelGoal",  // service name
  // these two fields are initialized below on the first access
  NULL,  // request message
  // action_msgs__srv__detail__cancel_goal__rosidl_typesupport_introspection_c__CancelGoal_Request_message_type_support_handle,
  NULL  // response message
  // action_msgs__srv__detail__cancel_goal__rosidl_typesupport_introspection_c__CancelGoal_Response_message_type_support_handle
};

static rosidl_service_type_support_t action_msgs__srv__detail__cancel_goal__rosidl_typesupport_introspection_c__CancelGoal_service_type_support_handle = {
  0,
  &action_msgs__srv__detail__cancel_goal__rosidl_typesupport_introspection_c__CancelGoal_service_members,
  get_service_typesupport_handle_function,
};

// Forward declaration of request/response type support functions
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, action_msgs, srv, CancelGoal_Request)();

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, action_msgs, srv, CancelGoal_Response)();

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_action_msgs
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, action_msgs, srv, CancelGoal)() {
  if (!action_msgs__srv__detail__cancel_goal__rosidl_typesupport_introspection_c__CancelGoal_service_type_support_handle.typesupport_identifier) {
    action_msgs__srv__detail__cancel_goal__rosidl_typesupport_introspection_c__CancelGoal_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)action_msgs__srv__detail__cancel_goal__rosidl_typesupport_introspection_c__CancelGoal_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, action_msgs, srv, CancelGoal_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, action_msgs, srv, CancelGoal_Response)()->data;
  }

  return &action_msgs__srv__detail__cancel_goal__rosidl_typesupport_introspection_c__CancelGoal_service_type_support_handle;
}
