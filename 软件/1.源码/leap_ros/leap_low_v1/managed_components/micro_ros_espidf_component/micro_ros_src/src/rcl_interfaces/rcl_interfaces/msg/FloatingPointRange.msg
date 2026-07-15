# Represents bounds and a step value for a floating point typed parameter.

# Start value for valid values, inclusive.
float64 from_value

# End value for valid values, inclusive.
float64 to_value

# Size of valid steps between the from and to bound.
# 
# Step is considered to be a magnitude, therefore negative values are treated
# the same as positive values, and a step value of zero implies a continuous
# range of values.
#
# Ideally, the step would be less than or equal to the distance between the
# bounds, as well as an even multiple of the distance between the bounds, but
# neither are required.
#
# If the absolute value of the step is larger than or equal to the distance
# between the two bounds, then the bounds will be the only valid values. e.g. if
# the range is defined as {from_value: 1.0, to_value: 2.0, step: 5.0} then the
# valid values will be 1.0 and 2.0.
#
# If the step is less than the distance between the bounds, but the distance is
# not a multiple of the step, then the "to" bound will always be a valid value,
# e.g. if the range is defined as {from_value: 2.0, to_value: 5.0, step: 2.0}
# then the valid values will be 2.0, 4.0, and 5.0.
float64 step
