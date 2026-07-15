# Copyright 2021 Christophe Bedard
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Tracepoint names."""

rcl_init = 'ros2:rcl_init'
rcl_node_init = 'ros2:rcl_node_init'
rmw_publisher_init = 'ros2:rmw_publisher_init'
rcl_publisher_init = 'ros2:rcl_publisher_init'
rclcpp_publish = 'ros2:rclcpp_publish'
rcl_publish = 'ros2:rcl_publish'
rmw_publish = 'ros2:rmw_publish'
rmw_subscription_init = 'ros2:rmw_subscription_init'
rcl_subscription_init = 'ros2:rcl_subscription_init'
rclcpp_subscription_init = 'ros2:rclcpp_subscription_init'
rclcpp_subscription_callback_added = 'ros2:rclcpp_subscription_callback_added'
rmw_take = 'ros2:rmw_take'
rcl_take = 'ros2:rcl_take'
rclcpp_take = 'ros2:rclcpp_take'
rcl_service_init = 'ros2:rcl_service_init'
rclcpp_service_callback_added = 'ros2:rclcpp_service_callback_added'
rcl_client_init = 'ros2:rcl_client_init'
rcl_timer_init = 'ros2:rcl_timer_init'
rclcpp_timer_callback_added = 'ros2:rclcpp_timer_callback_added'
rclcpp_timer_link_node = 'ros2:rclcpp_timer_link_node'
rclcpp_callback_register = 'ros2:rclcpp_callback_register'
callback_start = 'ros2:callback_start'
callback_end = 'ros2:callback_end'
rcl_lifecycle_state_machine_init = 'ros2:rcl_lifecycle_state_machine_init'
rcl_lifecycle_transition = 'ros2:rcl_lifecycle_transition'
rclcpp_executor_get_next_ready = 'ros2:rclcpp_executor_get_next_ready'
rclcpp_executor_wait_for_work = 'ros2:rclcpp_executor_wait_for_work'
rclcpp_executor_execute = 'ros2:rclcpp_executor_execute'
