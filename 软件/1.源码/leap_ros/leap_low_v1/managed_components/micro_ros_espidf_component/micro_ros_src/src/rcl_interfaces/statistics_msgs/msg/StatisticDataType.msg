#############################################
# This file contains the commonly used constants for the statistics data type.
#
# The value 0 is reserved for unitialized statistic message data type.
# Range of values: [0, 255].
# Unallowed values: any value that is not specified in this file.
#
#############################################

# Constant for uninitialized
uint8 STATISTICS_DATA_TYPE_UNINITIALIZED = 0

# Allowed values
uint8 STATISTICS_DATA_TYPE_AVERAGE = 1
uint8 STATISTICS_DATA_TYPE_MINIMUM = 2
uint8 STATISTICS_DATA_TYPE_MAXIMUM = 3
uint8 STATISTICS_DATA_TYPE_STDDEV = 4
uint8 STATISTICS_DATA_TYPE_SAMPLE_COUNT = 5
