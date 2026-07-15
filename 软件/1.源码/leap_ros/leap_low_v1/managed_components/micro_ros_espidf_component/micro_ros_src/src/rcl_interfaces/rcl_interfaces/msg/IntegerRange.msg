# Represents bounds and a step value for an integer typed parameter.

# Start value for valid values, inclusive.
int64 from_value

# End value for valid values, inclusive.
int64 to_value

# Size of valid steps between the from and to bound.
#
# A step value of zero implies a continuous range of values. Ideally, the step
# would be less than or equal to the distance between the bounds, as well as an
# even multiple of the distance between the bounds, but neither are required.
#
# If the absolute value of the step is larger than or equal to the distance
# between the two bounds, then the bounds will be the only valid values. e.g. if
# the range is defined as {from_value: 1, to_value: 2, step: 5} then the valid
# values will be 1 and 2.
# 
# If the step is less than the distance between the bounds, but the distance is
# not a multiple of the step, then the "to" bound will always be a valid value,
# e.g. if the range is defined as {from_value: 2, to_value: 5, step: 2} then
# the valid values will be 2, 4, and 5.
uint64 step
