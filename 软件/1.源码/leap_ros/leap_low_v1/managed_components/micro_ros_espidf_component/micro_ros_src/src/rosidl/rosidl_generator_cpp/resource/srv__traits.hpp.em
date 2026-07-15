@# Included from rosidl_generator_cpp/resource/idl__traits.hpp.em
@{
TEMPLATE(
    'msg__traits.hpp.em',
    package_name=package_name, interface_path=interface_path,
    message=service.request_message, include_directives=include_directives)
}@

@{
TEMPLATE(
    'msg__traits.hpp.em',
    package_name=package_name, interface_path=interface_path,
    message=service.response_message, include_directives=include_directives)
}@

@{
service_typename = '::'.join(service.namespaced_type.namespaced_name())
service_fully_qualified_name = '/'.join(service.namespaced_type.namespaced_name())
}@
@
namespace rosidl_generator_traits
{

template<>
inline const char * data_type<@(service_typename)>()
{
  return "@(service_typename)";
}

template<>
inline const char * name<@(service_typename)>()
{
  return "@(service_fully_qualified_name)";
}

template<>
struct has_fixed_size<@(service_typename)>
  : std::integral_constant<
    bool,
    has_fixed_size<@(service_typename)_Request>::value &&
    has_fixed_size<@(service_typename)_Response>::value
  >
{
};

template<>
struct has_bounded_size<@(service_typename)>
  : std::integral_constant<
    bool,
    has_bounded_size<@(service_typename)_Request>::value &&
    has_bounded_size<@(service_typename)_Response>::value
  >
{
};

template<>
struct is_service<@(service_typename)>
  : std::true_type
{
};

template<>
struct is_service_request<@(service_typename)_Request>
  : std::true_type
{
};

template<>
struct is_service_response<@(service_typename)_Response>
  : std::true_type
{
};

}  // namespace rosidl_generator_traits
