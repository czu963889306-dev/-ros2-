# rcl_interfaces
This package contains the messages and services which ROS client libraries will use under the hood to communicate higher level concepts such as parameters.

For more information about ROS 2 interfaces, see [docs.ros.org](https://docs.ros.org/en/rolling/Concepts/About-ROS-Interfaces.html)

## Parameter Groups
Parameters are contained in groups.
The default group is '/'.
It behaves like a filepath, where you can nest sub-groups within groups.

For more information about parameters, see:
[design.ros2.org](https://design.ros2.org/articles/ros_parameters.html)

## Standard topics for parameters

The ROS API for a node will be as follows inside the node's namespace.

### Topics:
 * `parameter_events`: `ParameterEvent`
  * This topic provides a way to subscribe to all parameter updates occurring on the node, including addition removal and changes in value. Every atomic change will be published separately.
 * `parameter_event_descriptors`: `ParameterEventDescriptors`
  * This topic provides a way to subscribe to all parameter updates occurring on the node, including addition removal and changes in value.
    Every atomic change will be published separately. This is provided if large parameter values are expected to slow down the system.

### Services:

 * `get_parameters`: `GetParameters`
  * The service to get the value of parameters which are set on this node.
 * `has_parameters`: `HasParameters`
  * Query this node if specific parameters are set.
 * `list_parameters`: `ListParameters`
  * List the parameters on this node matching the filters.
 * `set_parameters`: `SetParameters`
  * Set parameters on this node.

## Messages (.msg)
* [FloatingPointRange](msg/FloatingPointRange.msg): Represents bounds and a step value for a floating point typed parameter
* [IntegerRange](msg/IntegerRange.msg): Represents bounds and a step value for an integer typed parameter
* [IntraProcessMessage](msg/IntraProcessMessage.msg): Demonstration message for passing around a pointer to shared memory.
* [ListParameterResult](msg/ListParameterResult.msg): This is the returned result of ListParameters service
* [Log](msg/Log.msg): A message for communicating log messages and their levels
* [Parameter](msg/Parameter.msg): A message for setting and getting parameter values
* [ParameterDescriptor](msg/ParameterDescriptor.msg): A more informational message about parameters and their values
* [ParameterEvent](msg/ParameterEvent.msg): For information regarding setting, changing or removing parameter events
* [ParameterEventDescriptors](msg/ParameterEventDescriptors.msg): Message describing parameter updates occurring on the node, including addition removal and changes in value
* [ParameterType](msg/ParameterType.msg): Enum definitions for denoting a parameter value's type
* [ParameterValue](msg/ParameterValue.msg): The associated value and type of a parameter
* [SetParameterResult](msg/SetParametersResult.msg): Result message indicating whether a set parameters event succeeded

## Services (.srv)
* [DescribeParameters](srv/DescribeParameters.srv): Request a list of descriptions for a specified list of parameters.
* [GetParameters](srv/GetParameters.srv): Get the values of a specific list of parameters
* [GetParameterTypes](srv/GetParametersTypes.srv): Get the enum type of a list of parameters
* [ListParameters](srv/ListParameters.srv): Get the list of parameters given a list of prefixes
* [SetParameters](srv/SetParameters.srv): Add or change a list of parameters individually
* [SetParametersAtomically](srv/SetParametersAtomically.srv): Add or change all parameters in a list or none at all.

## Quality Declaration
This package claims to be in the **Quality Level 1** category, see the [Quality Declaration](QUALITY_DECLARATION.md) for more details.
