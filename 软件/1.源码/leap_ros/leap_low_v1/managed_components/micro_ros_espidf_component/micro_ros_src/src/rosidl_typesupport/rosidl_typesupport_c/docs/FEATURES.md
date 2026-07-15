# rosidl_typesupport_c features

`rosidl_typesupport_c` provides a Python generator executable, `rosidl_typesupport_c`, based on Empy to create rosidl C source files.

The templates utilized by this generator executable are located in the `resource` directory and generate source files for messages, services and actions.

The generator also generates a visibility_control header based on https://gcc.gnu.org/wiki/Visibility.

`rosidl_typesupport_c` defines a typesupport identifier, which is declared in `identifier.h`.

`rosidl_typesupport_c` provides the following functionality for incorporation into generated typesupport source files.

* `message_type_support_dispatch.h`: Look up message type support handle functions from available libraries.
* `service_type_support_dispatch.h`: Look up service type support handle functions from available libraries.
* `type_support_map_t.h`: This defines the lookup of C typesupport handlers.
