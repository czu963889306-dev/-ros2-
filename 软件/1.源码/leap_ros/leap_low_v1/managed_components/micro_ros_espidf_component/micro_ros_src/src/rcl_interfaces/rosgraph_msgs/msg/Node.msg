# Represents the observable runtime state of a ROS Node
# Therefore, does not perfectly align with the abstract specification which created it.

# Fully qualified node name (FQN)
string name

# Parameter specifications for the node
rcl_interfaces/ParameterDescriptor[] parameters

# Current values of the node's parameters
# NOTE:
#   parameter_values[] must be empty, or the same size as parameters[]
#   When set, parameter_values[] match 1:1 with the same index in parameters[]
rcl_interfaces/ParameterValue[] parameter_values

# Communications endpoints - Topics, Services, and Actions
Topic[] publishers
Topic[] subscriptions

Service[] service_clients
Service[] service_servers

Action[] action_clients
Action[] action_servers
