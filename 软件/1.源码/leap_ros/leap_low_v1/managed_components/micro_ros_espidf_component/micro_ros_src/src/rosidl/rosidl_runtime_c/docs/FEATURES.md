# rosidl_runtime_c Features

## Primitive sequence definitions initializations and finalizations

`rosidl_runtime_c` provides default definitions, as well as initialization and finalization functions for the supported rosidl primitive sequence types.
These functions are defined in the following headers:

* `primitives_sequence.h`: C struct definitions for all supported primitive sequence types.
* `primitive_sequence_functions.h`: initialization and finalization functionality for the above defined sequences.
* `sequence_bound.h`: Struct designed to track the upper bounds size of a sequence. Currently unused.

## String and u16string definitions, initializations and finalizations

Like the sequences described above, `rosidl_runtime_c` provides definitions, as well as initialization and finalization functions for 8-bit and 16-bit character strings.

* `string.h`: C struct definition for an 8-bit string.
* `string_functions.h`: initialization and finalization functionality for the rosidl 8-bit string.
* `string_bound.h`: Designed to track the upper bounds size of a string. Currently unused.

* `u16string.h`: C struct definition for a 16-bit string.
* `u16string_functions.h`: initialization and finalization functionality for the rosidl 16-bit string.

## Typesupport structs

This package defines several structs which are utilized for providing access to properties and functionality of different rosidl types.
They are defined for the three main ROS 2 rosidl types:
* Messages (`message_type_support_struct.h`)
* Services (`service_type_support_struct.h`)
* Actions (`action_type_support_struct.h`)
