# copied from ament_package/template/environment_hook/pkg_config_path.sh

ament_prepend_unique_value PKG_CONFIG_PATH "$AMENT_CURRENT_PREFIX/lib/pkgconfig"
