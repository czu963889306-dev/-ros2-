# Describes a single Service endpoint, which may be a Server or Client

# Fully qualified name of the Service
string name

# Type and actual QoS of the request publisher (Client) or subscription (Server)
InterfaceType request_type
QoSProfile request_qos

# Type and actual QoS of the request subscription (Client) or publisher (Server)
InterfaceType response_type
QoSProfile response_qos
