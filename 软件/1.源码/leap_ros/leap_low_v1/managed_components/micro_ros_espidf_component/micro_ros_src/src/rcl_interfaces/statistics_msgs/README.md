# metrics_statistics_msgs

Package containing ROS 2 message definitions for reporting
statistics for topics and system resources.

## Messages defined in this package

1. StatisticDataType

   A message that represent the types of statistics reported.
   This defines the following data types that can be used to represent
   individual metric data points.
   * `average`
   * `minimum`
   * `maximum`
   * `stddev`
   * `sample_count`

1. StatisticDataPoint

   A message that represents a single statistic value.
   This includes:
   * `data_type`: Type of the metric data point, as defined in `StatisticDataType`.
   * `data`: Value of the metric data point.

1. MetricsMessage

   A message that represents statistic data measured from a source
   within a time window.
   This includes:
   * `measurement_source_name`: Source from where the measurement or statistic originates.
   * `metrics_source`: Name of the metric.
   * `unit`: Unit representing the metric.
   * `window_start`: Start time of the metric measurement window.
   * `window_stop`: End time of the metric measurement window.
   * `statistics`: A list of `StatisticDataPoint` representing the values
   of collected metrics.

## Quality Declaration
This package claims to be in the **Quality Level 1** category, see the [Quality Declaration](QUALITY_DECLARATION.md) for more details.
