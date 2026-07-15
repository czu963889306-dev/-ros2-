# rosidl_typesupport_introspection_cpp features

`rosidl_typesupport_introspection_cpp` provides a Python generator executable, `rosidl_typesupport_introspection_cpp`, based on Empy to create C++ source files that describes the type of a ROS 2 C++ message and provides interfaces to construct and interpret such messages.

The templates utilized by this generator executable are located in the `resource` directory and generate source files for messages, services and actions.

`rosidl_typesupport_introspection_cpp` defines a typesupport identifier, which is declared in `identifier.hpp`.

`rosidl_typesupport_introspection_cpp` provides the following functionality for incorporation into generated typesupport source files.

* `message_type_support_decl.hpp`: Look up message type support handle functions from available libraries.
* `service_type_support_decl.hpp`: Look up service type support handle functions from available libraries.
* `message_introspection.hpp`: Defines the representation of message members and functions to construct or destruct messages and members and to compute member addresses.
* `service_introspection.hpp`: Provides access to service name, namespace and message types.
* `field_types.hpp`: Defines the codes used to identify a members' types.
