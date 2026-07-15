# Used to determine which of the next *_value fields are set.
# ParameterType.PARAMETER_NOT_SET indicates that the parameter was not set
# (if gotten) or is uninitialized.
# Values are enumerated in `ParameterType.msg`.

# The type of this parameter, which corresponds to the appropriate field below.
uint8 type

# "Variant" style storage of the parameter value. Only the value corresponding
# the type field will have valid information.

# Boolean value, can be either true or false.
bool bool_value

# Integer value ranging from -9,223,372,036,854,775,808 to
# 9,223,372,036,854,775,807.
int64 integer_value

# A double precision floating point value following IEEE 754.
float64 double_value

# A textual value with no practical length limit.
string string_value

# An array of bytes, used for non-textual information.
byte[] byte_array_value

# An array of boolean values.
bool[] bool_array_value

# An array of 64-bit integer values.
int64[] integer_array_value

# An array of 64-bit floating point values.
float64[] double_array_value

# An array of string values.
string[] string_array_value
