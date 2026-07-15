// generated from rosidl_generator_c/resource/idl__functions.c.em
// with input from rosgraph_msgs:msg/QoSProfile.idl
// generated code does not contain a copyright notice
#include "rosgraph_msgs/msg/detail/qo_s_profile__functions.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "rcutils/allocator.h"


// Include directives for member types
// Member `deadline`
// Member `lifespan`
// Member `liveliness_lease_duration`
#include "builtin_interfaces/msg/detail/duration__functions.h"

bool
rosgraph_msgs__msg__QoSProfile__init(rosgraph_msgs__msg__QoSProfile * msg)
{
  if (!msg) {
    return false;
  }
  // depth
  // deadline
  if (!builtin_interfaces__msg__Duration__init(&msg->deadline)) {
    rosgraph_msgs__msg__QoSProfile__fini(msg);
    return false;
  }
  // lifespan
  if (!builtin_interfaces__msg__Duration__init(&msg->lifespan)) {
    rosgraph_msgs__msg__QoSProfile__fini(msg);
    return false;
  }
  // history
  // reliability
  // durability
  // liveliness
  // liveliness_lease_duration
  if (!builtin_interfaces__msg__Duration__init(&msg->liveliness_lease_duration)) {
    rosgraph_msgs__msg__QoSProfile__fini(msg);
    return false;
  }
  return true;
}

void
rosgraph_msgs__msg__QoSProfile__fini(rosgraph_msgs__msg__QoSProfile * msg)
{
  if (!msg) {
    return;
  }
  // depth
  // deadline
  builtin_interfaces__msg__Duration__fini(&msg->deadline);
  // lifespan
  builtin_interfaces__msg__Duration__fini(&msg->lifespan);
  // history
  // reliability
  // durability
  // liveliness
  // liveliness_lease_duration
  builtin_interfaces__msg__Duration__fini(&msg->liveliness_lease_duration);
}

bool
rosgraph_msgs__msg__QoSProfile__are_equal(const rosgraph_msgs__msg__QoSProfile * lhs, const rosgraph_msgs__msg__QoSProfile * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  // depth
  if (lhs->depth != rhs->depth) {
    return false;
  }
  // deadline
  if (!builtin_interfaces__msg__Duration__are_equal(
      &(lhs->deadline), &(rhs->deadline)))
  {
    return false;
  }
  // lifespan
  if (!builtin_interfaces__msg__Duration__are_equal(
      &(lhs->lifespan), &(rhs->lifespan)))
  {
    return false;
  }
  // history
  if (lhs->history != rhs->history) {
    return false;
  }
  // reliability
  if (lhs->reliability != rhs->reliability) {
    return false;
  }
  // durability
  if (lhs->durability != rhs->durability) {
    return false;
  }
  // liveliness
  if (lhs->liveliness != rhs->liveliness) {
    return false;
  }
  // liveliness_lease_duration
  if (!builtin_interfaces__msg__Duration__are_equal(
      &(lhs->liveliness_lease_duration), &(rhs->liveliness_lease_duration)))
  {
    return false;
  }
  return true;
}

bool
rosgraph_msgs__msg__QoSProfile__copy(
  const rosgraph_msgs__msg__QoSProfile * input,
  rosgraph_msgs__msg__QoSProfile * output)
{
  if (!input || !output) {
    return false;
  }
  // depth
  output->depth = input->depth;
  // deadline
  if (!builtin_interfaces__msg__Duration__copy(
      &(input->deadline), &(output->deadline)))
  {
    return false;
  }
  // lifespan
  if (!builtin_interfaces__msg__Duration__copy(
      &(input->lifespan), &(output->lifespan)))
  {
    return false;
  }
  // history
  output->history = input->history;
  // reliability
  output->reliability = input->reliability;
  // durability
  output->durability = input->durability;
  // liveliness
  output->liveliness = input->liveliness;
  // liveliness_lease_duration
  if (!builtin_interfaces__msg__Duration__copy(
      &(input->liveliness_lease_duration), &(output->liveliness_lease_duration)))
  {
    return false;
  }
  return true;
}

rosgraph_msgs__msg__QoSProfile *
rosgraph_msgs__msg__QoSProfile__create()
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__QoSProfile * msg = (rosgraph_msgs__msg__QoSProfile *)allocator.allocate(sizeof(rosgraph_msgs__msg__QoSProfile), allocator.state);
  if (!msg) {
    return NULL;
  }
  memset(msg, 0, sizeof(rosgraph_msgs__msg__QoSProfile));
  bool success = rosgraph_msgs__msg__QoSProfile__init(msg);
  if (!success) {
    allocator.deallocate(msg, allocator.state);
    return NULL;
  }
  return msg;
}

void
rosgraph_msgs__msg__QoSProfile__destroy(rosgraph_msgs__msg__QoSProfile * msg)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (msg) {
    rosgraph_msgs__msg__QoSProfile__fini(msg);
  }
  allocator.deallocate(msg, allocator.state);
}


bool
rosgraph_msgs__msg__QoSProfile__Sequence__init(rosgraph_msgs__msg__QoSProfile__Sequence * array, size_t size)
{
  if (!array) {
    return false;
  }
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__QoSProfile * data = NULL;

  if (size) {
    data = (rosgraph_msgs__msg__QoSProfile *)allocator.zero_allocate(size, sizeof(rosgraph_msgs__msg__QoSProfile), allocator.state);
    if (!data) {
      return false;
    }
    // initialize all array elements
    size_t i;
    for (i = 0; i < size; ++i) {
      bool success = rosgraph_msgs__msg__QoSProfile__init(&data[i]);
      if (!success) {
        break;
      }
    }
    if (i < size) {
      // if initialization failed finalize the already initialized array elements
      for (; i > 0; --i) {
        rosgraph_msgs__msg__QoSProfile__fini(&data[i - 1]);
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
rosgraph_msgs__msg__QoSProfile__Sequence__fini(rosgraph_msgs__msg__QoSProfile__Sequence * array)
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
      rosgraph_msgs__msg__QoSProfile__fini(&array->data[i]);
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

rosgraph_msgs__msg__QoSProfile__Sequence *
rosgraph_msgs__msg__QoSProfile__Sequence__create(size_t size)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  rosgraph_msgs__msg__QoSProfile__Sequence * array = (rosgraph_msgs__msg__QoSProfile__Sequence *)allocator.allocate(sizeof(rosgraph_msgs__msg__QoSProfile__Sequence), allocator.state);
  if (!array) {
    return NULL;
  }
  bool success = rosgraph_msgs__msg__QoSProfile__Sequence__init(array, size);
  if (!success) {
    allocator.deallocate(array, allocator.state);
    return NULL;
  }
  return array;
}

void
rosgraph_msgs__msg__QoSProfile__Sequence__destroy(rosgraph_msgs__msg__QoSProfile__Sequence * array)
{
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  if (array) {
    rosgraph_msgs__msg__QoSProfile__Sequence__fini(array);
  }
  allocator.deallocate(array, allocator.state);
}

bool
rosgraph_msgs__msg__QoSProfile__Sequence__are_equal(const rosgraph_msgs__msg__QoSProfile__Sequence * lhs, const rosgraph_msgs__msg__QoSProfile__Sequence * rhs)
{
  if (!lhs || !rhs) {
    return false;
  }
  if (lhs->size != rhs->size) {
    return false;
  }
  for (size_t i = 0; i < lhs->size; ++i) {
    if (!rosgraph_msgs__msg__QoSProfile__are_equal(&(lhs->data[i]), &(rhs->data[i]))) {
      return false;
    }
  }
  return true;
}

bool
rosgraph_msgs__msg__QoSProfile__Sequence__copy(
  const rosgraph_msgs__msg__QoSProfile__Sequence * input,
  rosgraph_msgs__msg__QoSProfile__Sequence * output)
{
  if (!input || !output) {
    return false;
  }
  if (output->capacity < input->size) {
    const size_t allocation_size =
      input->size * sizeof(rosgraph_msgs__msg__QoSProfile);
    rcutils_allocator_t allocator = rcutils_get_default_allocator();
    rosgraph_msgs__msg__QoSProfile * data =
      (rosgraph_msgs__msg__QoSProfile *)allocator.reallocate(
      output->data, allocation_size, allocator.state);
    if (!data) {
      return false;
    }
    // If reallocation succeeded, memory may or may not have been moved
    // to fulfill the allocation request, invalidating output->data.
    output->data = data;
    for (size_t i = output->capacity; i < input->size; ++i) {
      if (!rosgraph_msgs__msg__QoSProfile__init(&output->data[i])) {
        // If initialization of any new item fails, roll back
        // all previously initialized items. Existing items
        // in output are to be left unmodified.
        for (; i-- > output->capacity; ) {
          rosgraph_msgs__msg__QoSProfile__fini(&output->data[i]);
        }
        return false;
      }
    }
    output->capacity = input->size;
  }
  output->size = input->size;
  for (size_t i = 0; i < input->size; ++i) {
    if (!rosgraph_msgs__msg__QoSProfile__copy(
        &(input->data[i]), &(output->data[i])))
    {
      return false;
    }
  }
  return true;
}
