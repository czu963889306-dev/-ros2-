// generated from rosidl_typesupport_introspection_c/resource/idl__type_support.c.em
// with input from nav_msgs:srv/GetPlan.idl
// generated code does not contain a copyright notice

#include <stddef.h>
#include "nav_msgs/srv/detail/get_plan__rosidl_typesupport_introspection_c.h"
#include "nav_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
#include "rosidl_typesupport_introspection_c/field_types.h"
#include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/message_introspection.h"
#include "nav_msgs/srv/detail/get_plan__functions.h"
#include "nav_msgs/srv/detail/get_plan__struct.h"


// Include directives for member types
// Member `start`
// Member `goal`
#include "geometry_msgs/msg/pose_stamped.h"
// Member `start`
// Member `goal`
#include "geometry_msgs/msg/detail/pose_stamped__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  nav_msgs__srv__GetPlan_Request__init(message_memory);
}

void nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_fini_function(void * message_memory)
{
  nav_msgs__srv__GetPlan_Request__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_member_array[3] = {
  {
    "start",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__srv__GetPlan_Request, start),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "goal",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__srv__GetPlan_Request, goal),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  },
  {
    "tolerance",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_FLOAT,  // type
    0,  // upper bound of string
    NULL,  // members of sub message
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__srv__GetPlan_Request, tolerance),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_members = {
  "nav_msgs__srv",  // message namespace
  "GetPlan_Request",  // message name
  3,  // number of fields
  sizeof(nav_msgs__srv__GetPlan_Request),
  nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_member_array,  // message members
  nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_init_function,  // function to initialize message memory (memory has to be allocated)
  nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_type_support_handle = {
  0,
  &nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_nav_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Request)() {
  nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, PoseStamped)();
  nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_member_array[1].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, geometry_msgs, msg, PoseStamped)();
  if (!nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_type_support_handle.typesupport_identifier) {
    nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &nav_msgs__srv__GetPlan_Request__rosidl_typesupport_introspection_c__GetPlan_Request_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

// already included above
// #include <stddef.h>
// already included above
// #include "nav_msgs/srv/detail/get_plan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "nav_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "rosidl_typesupport_introspection_c/field_types.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
// already included above
// #include "rosidl_typesupport_introspection_c/message_introspection.h"
// already included above
// #include "nav_msgs/srv/detail/get_plan__functions.h"
// already included above
// #include "nav_msgs/srv/detail/get_plan__struct.h"


// Include directives for member types
// Member `plan`
#include "nav_msgs/msg/path.h"
// Member `plan`
#include "nav_msgs/msg/detail/path__rosidl_typesupport_introspection_c.h"

#ifdef __cplusplus
extern "C"
{
#endif

void nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_init_function(
  void * message_memory, enum rosidl_runtime_c__message_initialization _init)
{
  // TODO(karsten1987): initializers are not yet implemented for typesupport c
  // see https://github.com/ros2/ros2/issues/397
  (void) _init;
  nav_msgs__srv__GetPlan_Response__init(message_memory);
}

void nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_fini_function(void * message_memory)
{
  nav_msgs__srv__GetPlan_Response__fini(message_memory);
}

static rosidl_typesupport_introspection_c__MessageMember nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_member_array[1] = {
  {
    "plan",  // name
    rosidl_typesupport_introspection_c__ROS_TYPE_MESSAGE,  // type
    0,  // upper bound of string
    NULL,  // members of sub message (initialized later)
    false,  // is array
    0,  // array size
    false,  // is upper bound
    offsetof(nav_msgs__srv__GetPlan_Response, plan),  // bytes offset in struct
    NULL,  // default value
    NULL,  // size() function pointer
    NULL,  // get_const(index) function pointer
    NULL,  // get(index) function pointer
    NULL,  // fetch(index, &value) function pointer
    NULL,  // assign(index, value) function pointer
    NULL  // resize(index) function pointer
  }
};

static const rosidl_typesupport_introspection_c__MessageMembers nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_members = {
  "nav_msgs__srv",  // message namespace
  "GetPlan_Response",  // message name
  1,  // number of fields
  sizeof(nav_msgs__srv__GetPlan_Response),
  nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_member_array,  // message members
  nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_init_function,  // function to initialize message memory (memory has to be allocated)
  nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_fini_function  // function to terminate message instance (will not free memory)
};

// this is not const since it must be initialized on first access
// since C does not allow non-integral compile-time constants
static rosidl_message_type_support_t nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_type_support_handle = {
  0,
  &nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_members,
  get_message_typesupport_handle_function,
};

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_nav_msgs
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Response)() {
  nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_member_array[0].members_ =
    ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, msg, Path)();
  if (!nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_type_support_handle.typesupport_identifier) {
    nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  return &nav_msgs__srv__GetPlan_Response__rosidl_typesupport_introspection_c__GetPlan_Response_message_type_support_handle;
}
#ifdef __cplusplus
}
#endif

#include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "nav_msgs/msg/rosidl_typesupport_introspection_c__visibility_control.h"
// already included above
// #include "nav_msgs/srv/detail/get_plan__rosidl_typesupport_introspection_c.h"
// already included above
// #include "rosidl_typesupport_introspection_c/identifier.h"
#include "rosidl_typesupport_introspection_c/service_introspection.h"

// this is intentionally not const to allow initialization later to prevent an initialization race
static rosidl_typesupport_introspection_c__ServiceMembers nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_members = {
  "nav_msgs__srv",  // service namespace
  "GetPlan",  // service name
  // these two fields are initialized below on the first access
  NULL,  // request message
  // nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_Request_message_type_support_handle,
  NULL  // response message
  // nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_Response_message_type_support_handle
};

static rosidl_service_type_support_t nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_type_support_handle = {
  0,
  &nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_members,
  get_service_typesupport_handle_function,
};

// Forward declaration of request/response type support functions
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Request)();

const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Response)();

ROSIDL_TYPESUPPORT_INTROSPECTION_C_EXPORT_nav_msgs
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan)() {
  if (!nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_type_support_handle.typesupport_identifier) {
    nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_type_support_handle.typesupport_identifier =
      rosidl_typesupport_introspection_c__identifier;
  }
  rosidl_typesupport_introspection_c__ServiceMembers * service_members =
    (rosidl_typesupport_introspection_c__ServiceMembers *)nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_type_support_handle.data;

  if (!service_members->request_members_) {
    service_members->request_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Request)()->data;
  }
  if (!service_members->response_members_) {
    service_members->response_members_ =
      (const rosidl_typesupport_introspection_c__MessageMembers *)
      ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_c, nav_msgs, srv, GetPlan_Response)()->data;
  }

  return &nav_msgs__srv__detail__get_plan__rosidl_typesupport_introspection_c__GetPlan_service_type_support_handle;
}
