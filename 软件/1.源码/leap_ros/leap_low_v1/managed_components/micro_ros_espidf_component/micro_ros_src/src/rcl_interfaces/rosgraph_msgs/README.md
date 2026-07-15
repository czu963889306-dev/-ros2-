# rosgraph_msgs

This is a package containing message definitions relating to the ROS Computation Graph.
These are generally considered to be low-level messages that end users do not interact with.

For more information about ROS 2 clock, see [design.ros2.org](https://design.ros2.org/articles/clock_and_time.html).

For more information about ROS 2 interfaces, see [docs.ros.org](https://docs.ros.org/en/rolling/Concepts/About-ROS-Interfaces.html).

## Messages (.msg)

* [Action](msg/Action.msg): Describes an Action endpoint (server or client).
* [Clock](msg/Clock.msg): Communicates the current ROS time.
* [Graph](msg/Graph.msg): A list of Nodes, describing a running ROS graph.
* [InterfaceType](msg/InterfaceType.msg): Describes the name and hash of an interface's type.
* [Node](msg/Node.msg): Describes a running Node with all realized names and values.
* [QoSProfile](msg/QoSProfile.msg): Represents the actual Quality of Service profile of an interface endpoint.
* [Service](msg/Service.msg): Represents a Service endpoint (server or client).
* [Topic](msg/Topic.msg): Represents a Topic endpoint (publisher or subscription).
* [TypeHash](msg/TypeHash.msg): Encodes the hash of a ROS interface type, uniquely identifying it when definitions may change over time.

## Quality Declaration

This package claims to be in the **Quality Level 1** category, see the [Quality Declaration](QUALITY_DECLARATION.md) for more details.
