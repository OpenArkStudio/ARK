BUILD_PLUGIN_MACRO(ark_http_plugin)
set_property(TARGET ark_http_plugin APPEND PROPERTY LINK_LIBRARIES ark::config tomlplusplus::tomlplusplus)
