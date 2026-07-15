# This is the message to communicate a parameter. It is an open struct with an enum in
# the descriptor to select which value is active.

# The full name of the parameter.
string name

# The parameter's value which can be one of several types, see
# `ParameterValue.msg` and `ParameterType.msg`.
ParameterValue value
