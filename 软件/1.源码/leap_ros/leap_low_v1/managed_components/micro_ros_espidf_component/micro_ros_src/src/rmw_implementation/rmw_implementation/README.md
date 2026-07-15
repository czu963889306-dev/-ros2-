# rmw_implementation

Proxy implementation of the ROS 2 Middleware Interface, forwarding calls to the chosen (and dynamically loaded) `rmw` implementation.
`RMW_IMPLEMENTATION` environment variable, if set, allows the user to select one of the available `rmw` implementations.
Otherwise, the default `rmw` implementation will be used.
Refer to `rmw_implementation_cmake` package to learn about this default.


## Quality Declaration

This package claims to be in the **Quality Level 1** category, see the [Quality Declaration](./QUALITY_DECLARATION.md) for more details.
