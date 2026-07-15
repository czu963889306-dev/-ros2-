# ROS2 rcl YAML parameter parser

Parse a YAML parameter file and populate the C data structure.

Features are described in detail at [http://docs.ros2.org](http://docs.ros2.org/latest/api/rcl_yaml_param_parser/index.html)

The data structure params_st will then be used during node initialization

YAML parameter file should follow the yaml syntax shown below

NOTE: It only supports canonical int and float types

```
<node_namespace_string>:  # optional
  <node1_name>:
    ros__parameters:
      <field_name>: <field_value>
      <parameter_namespace_string>:   # optional
        <field1_name>: <field1_value>
        <field2_name>: <field2_value>
  <node2_name>:
    ros__parameters:
      <field_name>: <field_value>
      <parameter_namespace_string>:   # optional
        <field1_name>: <field1_value>
        <field2_name>: <field2_value>
```

This package depends on C libyaml.

## Quality Declaration

This package claims to be in the **Quality Level 1** category, see the [Quality Declaration](./QUALITY_DECLARATION.md) for more details.
