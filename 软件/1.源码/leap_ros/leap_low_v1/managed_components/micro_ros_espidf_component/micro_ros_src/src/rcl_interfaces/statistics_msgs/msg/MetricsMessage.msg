#############################################
# A generic metrics message providing statistics for measurements from different sources. For example,
# measure a system's CPU % for a given window yields the following data points over a window of time:
#
#   - average cpu %
#   - std deviation
#   - min
#   - max
#   - sample count
#
# These are all represented as different 'StatisticDataPoint's.
#############################################

# Name metric measurement source, e.g., node, topic, or process name
string measurement_source_name

# Name of the metric being measured, e.g. cpu_percentage, free_memory_mb, message_age, etc.
string metrics_source

# Unit of measure of the metric, e.g. percent, mb, seconds, etc.
string unit

# Measurement window start time
builtin_interfaces/Time window_start

# Measurement window end time
builtin_interfaces/Time window_stop

# A list of statistics data point, defined in StatisticDataPoint.msg
StatisticDataPoint[] statistics
