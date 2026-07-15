# This is the message to communicate a parameter's descriptor.

# The name of the parameter.
string name

# Enum values are defined in the `ParameterType.msg` message.
uint8 type

# Description of the parameter, visible from introspection tools.
string description

# Parameter constraints

# Plain English description of additional constraints which cannot be expressed
# with the available constraints, e.g. "only prime numbers".
#
# By convention, this should only be used to clarify constraints which cannot
# be completely expressed with the parameter constraints below.
string additional_constraints

# If 'true' then the value cannot change after it has been initialized.
bool read_only false

# If true, the parameter is allowed to change type.
bool dynamic_typing false

# If any of the following sequences are not empty, then the constraint inside of
# them apply to this parameter.
#
# FloatingPointRange and IntegerRange are mutually exclusive.

# FloatingPointRange consists of a from_value, a to_value, and a step.
FloatingPointRange[<=1] floating_point_range

# IntegerRange consists of a from_value, a to_value, and a step.
IntegerRange[<=1] integer_range
