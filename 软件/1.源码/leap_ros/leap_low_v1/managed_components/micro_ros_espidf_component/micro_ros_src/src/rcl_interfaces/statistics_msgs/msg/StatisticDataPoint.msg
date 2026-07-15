#############################################
# This holds the structure of a single data point of a StatisticDataType.
#
# This message is used in MetricsStatisticsMessage, defined in MetricsStatisticsMessage.msg.
#
# Examples of the value of data point are
# - average size of messages received
# - standard deviation of the period of messages published
# - maximum age of messages published
#
# A value of nan represents no data is available.
# One example is that standard deviation is only available when there are two or more data points but there is only one,
# and in this case the value would be nan.
# +inf and -inf are not allowed.
#
#############################################

# The statistic type of this data point, defined in StatisticDataType.msg
# Default value should be StatisticDataType.STATISTICS_DATA_TYPE_UNINITIALIZED (0).
uint8 data_type

# The value of the data point
float64 data
