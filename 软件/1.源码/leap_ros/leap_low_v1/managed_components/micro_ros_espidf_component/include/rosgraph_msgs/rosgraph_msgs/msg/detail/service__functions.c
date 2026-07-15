// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from rosgraph_msgs:msg/Service.idl
// generated code does not contain a copyright notice
#include "rosgraph_msgs/msg/detail/service__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `name`
#include "rosidl_runtime_c/string_functions.h"
// Member `request_type`
// Member `response_type`
#include "rosgraph_msgs/msg/detail/interface_type__functions.h"
// Member `request_qos`
// Member `response_qos`
#include "rosgraph_msgs/msg/detail/qo_s_profile__functions.h"

bool
rosgraph_msgs__msg__Service__init(rosgraph_msgs__msg__Service * msg)
{
  if (!msg) {
    return false;
  }
  // name
  if (!rosidl_runtime_c__String__init(&msg->name)) {
    rosgraph_msgs__msg__Service__fini(msg);
    return false;
  }
  // request_type
  if (!rosgraph_msgs__msg__InterfaceType__init(&msg->request_type)) {
    rosgraph_msgs__msg__Service__fini(msg);
    return false;
  }
  // request_qos
  if (!rosgraph_msgs__msg__QoSProfile__init(&msg->request_qos)) {
    rosgraph_msgs__msg__Service__fini(msg);
    return false;
  }
  // response_type
  if (!rosgraph_msgs__msg__InterfaceType__init(&msg->response_type)) {
    rosgraph_msgs__msg__Service__fini(msg);
    return false;
  }
  // response_qos
  if (!rosgraph_msgs__msg__QoSProfile__init(&msg->response_qos)) {
    rosgraph_msgs__msg__Service__fini(msg);
    return false;
  }
  return true;
}

void
rosgraph_msgs__msg__Service__fini(rosgraph_msgs__msg__Service * msg)
{
  if (!msg) {
    return;
  }
  // name
  rosidl_runtime_c__String__fini(&msg->name);
  // request_type
  rosgraph_msgs__msg__InterfaceType__fini(&msg->request_type);
  // request_qos
  rosgraph_msgs__msg__QoSProfile__fini(&msg->request_qos);
  // response_type
  rosgraph_msgs__msg__InterfaceType__fini(&msg->response_type);
  // response_qos
  rosgraph_msgs__msg__QoSProfile__fini(&msg->response_qos);
}

bool
rosgraph_msgs__msg__Service__are_equal(const rosgraph_msgs__msg__Service * lhs, const rosgraph_msgs__msg__Service * rhs)
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
  // request_type
  if (!rosgraph_msgs__msg__InterfaceType__are_equal(
      &(lhs->request_type), &(rhs->request_type)))
  {
    return false;
  }
  // request_qos
  if (!rosgraph_msgs__msg__QoSProfile__are_equal(
      &(lhs->request_qos), &(rhs->request_qos)))
  {
    return false;
  }
  // response_type
  if (!rosgraph_msgs__msg__InterfaceType__are_equal(
      &(lhs->response_type), &(rhs->response_type)))
  {
    return false;
  }
  // response_qos
  if (!rosgraph_msgs__msg__QoSProfile__are_equal(
      &(lhs->response_qos), &(rhs->response_qos)))
  {
    return false;
  }
  return true;
}

bool
rosgraph_msgs__msg__Service__copy(
  const rosgraph_msgs__msg__Service * input,
  rosgraph_msgs__msg__Service * output)
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
  // request_type
  if (!rosgraph_msgs__msg__InterfaceType__copy(
      &(input->request_type), &(output->request_type)))
  {
    return false;
  }
  // request_qos
  if (!rosgraph_msgs__msg__QoSProfile__copy(
      &(input->request_qos), &(output->request_qos)))
  {
    return false;
  }
  // response_type
  if (!rosgraph_msgs__msg__InterfaceType__copy(
      &(input->response_type), &(output->response_type)))
  {
    return false;
  }
  // response_qos
  if (!rosgraph_msgs__msg__QoSProfile__copy(
      &(input->response_qos), &(output->response_qos)))
  {
    return false;
  }
  return true;
}

rosgraph_msgs__msg__Service *
rosgraph_msgs__msg__Service__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__Service * msg = (rosgraph_msgs__msg__Service *)allocator.allocate(sizeof(rosgraph_msgs__msg__Service), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(rosgraph_msgs__msg__Service));
  bool success = rosgraph_msgs__msg__Service__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
rosgraph_msgs__msg__Service__destroy(rosgraph_msgs__msg__Service * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    rosgraph_msgs__msg__Service__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
rosgraph_msgs__msg__Service__Sequence__init(rosgraph_msgs__msg__Service__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__Service * data = NULL;

  if (size) {
    data = (rosgraph_msgs__msg__Service *)allocator.zero_allocate(size, sizeof(rosgraph_msgs__msg__Service), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = rosgraph_msgs__msg__Service__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        rosgraph_msgs__msg__Service__fini(&data[i - 1]);
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
rosgraph_msgs__msg__Service__Sequence__fini(rosgraph_msgs__msg__Service__Sequence * array)
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
      rosgraph_msgs__msg__Service__fini(&array->data[i]);
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

rosgraph_msgs__msg__Service__Sequence *
rosgraph_msgs__msg__Service__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__Service__Sequence * array = (rosgraph_msgs__msg__Service__Sequence *)allocator.allocate(sizeof(rosgraph_msgs__msg__Service__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = rosgraph_msgs__msg__Service__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
rosgraph_msgs__msg__Service__Sequence__destroy(rosgraph_msgs__msg__Service__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    rosgraph_msgs__msg__Service__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
rosgraph_msgs__msg__Service__Sequence__are_equal(const rosgraph_msgs__msg__Service__Sequence * lhs, const rosgraph_msgs__msg__Service__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!rosgraph_msgs__msg__Service__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
rosgraph_msgs__msg__Service__Sequence__copy(
  const rosgraph_msgs__msg__Service__Sequence * input,
  rosgraph_msgs__msg__Service__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(rosgraph_msgs__msg__Service);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rosgraph_msgs__msg__Service * data =
      (rosgraph_msgs__msg__Service *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!rosgraph_msgs__msg__Service__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          rosgraph_msgs__msg__Service__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!rosgraph_msgs__msg__Service__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
