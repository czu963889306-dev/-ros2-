# lifecycle_msgs
This package contains message and service definitions for managing lifecycle nodes.
These messages and services form a standardized interface for transitioning these
managed nodes through a known state-machine.

For more information about life cycle nodes see: [design.ros2.org](http://design.ros2.org/articles/node_lifecycle.html).

For more information about ROS 2 interfaces, see [docs.ros.org](https://docs.ros.org/en/rolling/Concepts/About-ROS-Interfaces.html).

## Messages (.msg)
* [State](msg/State.msg): A lifecycle node's state-machine state.
* [Transition](msg/Transition.msg): A state transition with specific id and label.
* [TransitionDescription](msg/TransitionDescription.msg): A description of a transition from one state-machine state to another.
* [TransitionEvent](msg/TransitionEvent.msg): A timestamped state transition.

## Services (.srv)
* [ChangeState](srv/ChangeState.srv): Request a node change states with a specific transition.
* [GetAvailableStates](srv/GetAvailableStates.srv): Request an array of states that this node can transition to.
* [GetAvailableTransitions](srv/GetAvailableTransitions.srv): Request an array of lifecycle state transitions available for this node.
* [GetState](srv/GetState.srv): Request the current lifecycle state of this node.

## Quality Declaration
This package claims to be in the **Quality Level 1** category, see the [Quality Declaration](QUALITY_DECLARATION.md) for more details.
