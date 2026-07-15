# composition_interfaces
This is a package containing message and service definitions for managing composable nodes in a container process.
Generally these services are used by the ROS 2 [`roslaunch`](https://design.ros2.org/articles/roslaunch.html) system.

For more information about ROS 2 interfaces, see [docs.ros.org](https://docs.ros.org/en/rolling/Concepts/About-ROS-Interfaces.html).

## Services (.srv)
* [ListNodes](srv/ListNodes.srv): Retrieve a list of running composable nodes, including their names and ids.
* [LoadNodes](srv/LoadNode.srv): Load a composable node.
* [UnloadNode](srv/UnloadNode.srv): Unload a specified node by its id.

## Quality Declaration
This package claims to be in the **Quality Level 1** category, see the [Quality Declaration](QUALITY_DECLARATION.md) for more details.
