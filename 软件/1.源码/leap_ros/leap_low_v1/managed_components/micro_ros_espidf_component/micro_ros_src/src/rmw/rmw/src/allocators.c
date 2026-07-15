// Copyright 2014 Open Source Robotics Foundation, Inc.
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

#include "rmw/allocators.h"

#include <stdlib.h>
#include <string.h>

#include <rcutils/allocator.h>

#include "rmw/types.h"

void *
rmw_allocate(size_t size)
{
  // Could be overridden with a general purpose allocator
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  void * ptr = allocator.allocate(size, allocator.state);
  if (ptr) {
    memset(ptr, 0, size);
  }
  return ptr;
}

void
rmw_free(void * pointer)
{
  // Should have a corresponding override with rmw_allocate
  rcutils_allocator_t allocator = rcutils_get_default_allocator();
  allocator.deallocate(pointer, allocator.state);
}

rmw_node_t *
rmw_node_allocate()
{
  // Could be overridden with custom (maybe static) node struct allocator
  return (rmw_node_t *)rmw_allocate(sizeof(rmw_node_t));
}

void
rmw_node_free(rmw_node_t * node)
{
  // Should have matching overide with rmw_node_allocate
  rmw_free(node);
}

rmw_publisher_t *
rmw_publisher_allocate()
{
  // Could be overridden with custom (maybe static) publisher struct allocator
  return (rmw_publisher_t *)rmw_allocate(sizeof(rmw_publisher_t));
}

void
rmw_publisher_free(rmw_publisher_t * publisher)
{
  // Should have matching overide with rmw_publisher_allocate
  rmw_free(publisher);
}

rmw_subscription_t *
rmw_subscription_allocate()
{
  // Could be overridden with custom (maybe static) subscription struct allocator
  return (rmw_subscription_t *)rmw_allocate(sizeof(rmw_subscription_t));
}

void
rmw_subscription_free(rmw_subscription_t * subscription)
{
  // Should have matching overide with rmw_subscription_allocate
  rmw_free(subscription);
}

rmw_guard_condition_t *
rmw_guard_condition_allocate()
{
  // Could be overridden with custom (maybe static) guard_condition
  // struct allocator
  return (rmw_guard_condition_t *)rmw_allocate(sizeof(rmw_guard_condition_t));
}

void
rmw_guard_condition_free(rmw_guard_condition_t * guard_condition)
{
  // Should have matching overide with rmw_guard_condition_allocate
  rmw_free(guard_condition);
}

rmw_client_t *
rmw_client_allocate()
{
  // Could be overridden with custom (maybe static) client struct allocator
  return (rmw_client_t *)rmw_allocate(sizeof(rmw_client_t));
}

void
rmw_client_free(rmw_client_t * client)
{
  // Should have matching overide with rmw_client_allocate
  rmw_free(client);
}

rmw_service_t *
rmw_service_allocate()
{
  // Could be overridden with custom (maybe static) client struct allocator
  return (rmw_service_t *)rmw_allocate(sizeof(rmw_service_t));
}

void
rmw_service_free(rmw_service_t * service)
{
  // Should have matching overide with rmw_service_allocate
  rmw_free(service);
}

rmw_wait_set_t *
rmw_wait_set_allocate()
{
  // Could be overridden with custom (maybe static) client struct allocator
  return (rmw_wait_set_t *)rmw_allocate(sizeof(rmw_wait_set_t));
}

void
rmw_wait_set_free(rmw_wait_set_t * wait_set)
{
  // Should have matching overide with rmw_wait_set_allocate
  rmw_free(wait_set);
}
