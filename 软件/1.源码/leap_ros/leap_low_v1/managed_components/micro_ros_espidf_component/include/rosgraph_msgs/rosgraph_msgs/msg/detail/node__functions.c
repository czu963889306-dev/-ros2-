// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from rosgraph_msgs:msg/Node.idl
// generated code does not contain a copyright notice
#include "rosgraph_msgs/msg/detail/node__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `name`
#include "rosidl_runtime_c/string_functions.h"
// Member `parameters`
#include "rcl_interfaces/msg/detail/parameter_descriptor__functions.h"
// Member `parameter_values`
#include "rcl_interfaces/msg/detail/parameter_value__functions.h"
// Member `publishers`
// Member `subscriptions`
#include "rosgraph_msgs/msg/detail/topic__functions.h"
// Member `service_clients`
// Member `service_servers`
#include "rosgraph_msgs/msg/detail/service__functions.h"
// Member `action_clients`
// Member `action_servers`
#include "rosgraph_msgs/msg/detail/action__functions.h"

bool
rosgraph_msgs__msg__Node__init(rosgraph_msgs__msg__Node * msg)
{
  if (!msg) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__init(&msg->name)) {
    rosgraph_msgs__msg__Node__fini(msg);
    return false;
  }
  // parameters
  if (!rcl_interfaces__msg__ParameterDescriptor__Sequence__init(&msg->parameters, 0)) {
    rosgraph_msgs__msg__Node__fini(msg);
    return false;
  }
  // parameter_values
  if (!rcl_interfaces__msg__ParameterValue__Sequence__init(&msg->parameter_values, 0)) {
    rosgraph_msgs__msg__Node__fini(msg);
    return false;
  }
  // publishers
  if (!rosgraph_msgs__msg__Topic__Sequence__init(&msg->publishers, 0)) {
    rosgraph_msgs__msg__Node__fini(msg);
    return false;
  }
  // subscriptions
  if (!rosgraph_msgs__msg__Topic__Sequence__init(&msg->subscriptions, 0)) {
    rosgraph_msgs__msg__Node__fini(msg);
    return false;
  }
  // service_clients
  if (!rosgraph_msgs__msg__Service__Sequence__init(&msg->service_clients, 0)) {
    rosgraph_msgs__msg__Node__fini(msg);
    return false;
  }
  // service_servers
  if (!rosgraph_msgs__msg__Service__Sequence__init(&msg->service_servers, 0)) {
    rosgraph_msgs__msg__Node__fini(msg);
    return false;
  }
  // action_clients
  if (!rosgraph_msgs__msg__Action__Sequence__init(&msg->action_clients, 0)) {
    rosgraph_msgs__msg__Node__fini(msg);
    return false;
  }
  // action_servers
  if (!rosgraph_msgs__msg__Action__Sequence__init(&msg->action_servers, 0)) {
    rosgraph_msgs__msg__Node__fini(msg);
    return false;
  }
  return true;
}

void
rosgraph_msgs__msg__Node__fini(rosgraph_msgs__msg__Node * msg)
{
  if (!msg) {
    return;
  }
  // name
  rosidl_runtime_c__String__fini(&msg->name);
  // parameters
  rcl_interfaces__msg__ParameterDescriptor__Sequence__fini(&msg->parameters);
  // parameter_values
  rcl_interfaces__msg__ParameterValue__Sequence__fini(&msg->parameter_values);
  // publishers
  rosgraph_msgs__msg__Topic__Sequence__fini(&msg->publishers);
  // subscriptions
  rosgraph_msgs__msg__Topic__Sequence__fini(&msg->subscriptions);
  // service_clients
  rosgraph_msgs__msg__Service__Sequence__fini(&msg->service_clients);
  // service_servers
  rosgraph_msgs__msg__Service__Sequence__fini(&msg->service_servers);
  // action_clients
  rosgraph_msgs__msg__Action__Sequence__fini(&msg->action_clients);
  // action_servers
  rosgraph_msgs__msg__Action__Sequence__fini(&msg->action_servers);
}

bool
rosgraph_msgs__msg__Node__are_equal(const rosgraph_msgs__msg__Node * lhs, const rosgraph_msgs__msg__Node * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__are_equal(
      &(lhs->name), &(rhs->name)))
  {
    return false;
  }
  // parameters
  if (!rcl_interfaces__msg__ParameterDescriptor__Sequence__are_equal(
      &(lhs->parameters), &(rhs->parameters)))
  {
    return false;
  }
  // parameter_values
  if (!rcl_interfaces__msg__ParameterValue__Sequence__are_equal(
      &(lhs->parameter_values), &(rhs->parameter_values)))
  {
    return false;
  }
  // publishers
  if (!rosgraph_msgs__msg__Topic__Sequence__are_equal(
      &(lhs->publishers), &(rhs->publishers)))
  {
    return false;
  }
  // subscriptions
  if (!rosgraph_msgs__msg__Topic__Sequence__are_equal(
      &(lhs->subscriptions), &(rhs->subscriptions)))
  {
    return false;
  }
  // service_clients
  if (!rosgraph_msgs__msg__Service__Sequence__are_equal(
      &(lhs->service_clients), &(rhs->service_clients)))
  {
    return false;
  }
  // service_servers
  if (!rosgraph_msgs__msg__Service__Sequence__are_equal(
      &(lhs->service_servers), &(rhs->service_servers)))
  {
    return false;
  }
  // action_clients
  if (!rosgraph_msgs__msg__Action__Sequence__are_equal(
      &(lhs->action_clients), &(rhs->action_clients)))
  {
    return false;
  }
  // action_servers
  if (!rosgraph_msgs__msg__Action__Sequence__are_equal(
      &(lhs->action_servers), &(rhs->action_servers)))
  {
    return false;
  }
  return true;
}

bool
rosgraph_msgs__msg__Node__copy(
  const rosgraph_msgs__msg__Node * input,
  rosgraph_msgs__msg__Node * output)
{
  if (!input || !output) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__copy(
      &(input->name), &(output->name)))
  {
    return false;
  }
  // parameters
  if (!rcl_interfaces__msg__ParameterDescriptor__Sequence__copy(
      &(input->parameters), &(output->parameters)))
  {
    return false;
  }
  // parameter_values
  if (!rcl_interfaces__msg__ParameterValue__Sequence__copy(
      &(input->parameter_values), &(output->parameter_values)))
  {
    return false;
  }
  // publishers
  if (!rosgraph_msgs__msg__Topic__Sequence__copy(
      &(input->publishers), &(output->publishers)))
  {
    return false;
  }
  // subscriptions
  if (!rosgraph_msgs__msg__Topic__Sequence__copy(
      &(input->subscriptions), &(output->subscriptions)))
  {
    return false;
  }
  // service_clients
  if (!rosgraph_msgs__msg__Service__Sequence__copy(
      &(input->service_clients), &(output->service_clients)))
  {
    return false;
  }
  // service_servers
  if (!rosgraph_msgs__msg__Service__Sequence__copy(
      &(input->service_servers), &(output->service_servers)))
  {
    return false;
  }
  // action_clients
  if (!rosgraph_msgs__msg__Action__Sequence__copy(
      &(input->action_clients), &(output->action_clients)))
  {
    return false;
  }
  // action_servers
  if (!rosgraph_msgs__msg__Action__Sequence__copy(
      &(input->action_servers), &(output->action_servers)))
  {
    return false;
  }
  return true;
}

rosgraph_msgs__msg__Node *
rosgraph_msgs__msg__Node__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__Node * msg = (rosgraph_msgs__msg__Node *)allocator.allocate(sizeof(rosgraph_msgs__msg__Node), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(rosgraph_msgs__msg__Node));
  bool success = rosgraph_msgs__msg__Node__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
rosgraph_msgs__msg__Node__destroy(rosgraph_msgs__msg__Node * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    rosgraph_msgs__msg__Node__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
rosgraph_msgs__msg__Node__Sequence__init(rosgraph_msgs__msg__Node__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__Node * data = NULL;

  if (size) {
    data = (rosgraph_msgs__msg__Node *)allocator.zero_allocate(size, sizeof(rosgraph_msgs__msg__Node), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = rosgraph_msgs__msg__Node__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        rosgraph_msgs__msg__Node__fini(&data[i - 1]);
      }
      allocator.deallocate(data, allocator.state);
      return false;
    }
  }
  array->data = data;
  array->size = size;
  array->capacity = size;
  return true;
}

void
rosgraph_msgs__msg__Node__Sequence__fini(rosgraph_msgs__msg__Node__Sequence * array)
{
  if (!array) {
    return;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (array->data) {
    // ensure that data and capacity values are consistent
    assert(array->capacity > 0);
    // finalize all array elements
    for (size_t i = 0; i < array->capacity; ++i) {
      rosgraph_msgs__msg__Node__fini(&array->data[i]);
    }
    allocator.deallocate(array->data, allocator.state);
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
  } else {
    // ensure that data, size, and capacity values are consistent
    assert(0 == array->size);
    assert(0 == array->capacity);
  }
}

rosgraph_msgs__msg__Node__Sequence *
rosgraph_msgs__msg__Node__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__Node__Sequence * array = (rosgraph_msgs__msg__Node__Sequence *)allocator.allocate(sizeof(rosgraph_msgs__msg__Node__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = rosgraph_msgs__msg__Node__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
rosgraph_msgs__msg__Node__Sequence__destroy(rosgraph_msgs__msg__Node__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    rosgraph_msgs__msg__Node__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
rosgraph_msgs__msg__Node__Sequence__are_equal(const rosgraph_msgs__msg__Node__Sequence * lhs, const rosgraph_msgs__msg__Node__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!rosgraph_msgs__msg__Node__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
rosgraph_msgs__msg__Node__Sequence__copy(
  const rosgraph_msgs__msg__Node__Sequence * input,
  rosgraph_msgs__msg__Node__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(rosgraph_msgs__msg__Node);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rosgraph_msgs__msg__Node * data =
      (rosgraph_msgs__msg__Node *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!rosgraph_msgs__msg__Node__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          rosgraph_msgs__msg__Node__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!rosgraph_msgs__msg__Node__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
