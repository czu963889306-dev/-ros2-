# This message contains a parameter event.
# Because the parameter event was an atomic update, a specific parameter name
# can only be in one of the three sets.

# The time stamp when this parameter event occurred.
builtin_interfaces/Time stamp

# Fully qualified ROS path to node.
string node

# New parameters that have been set for this node.
Parameter[] new_parameters

# Parameters that have been changed during this event.
Parameter[] changed_parameters

# Parameters that have been deleted during this event.
Parameter[] deleted_parameters
