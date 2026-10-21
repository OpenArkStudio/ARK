BUILD_PLUGIN_MACRO(ark_redis_plugin arknet::arknet)
target_include_directories(ark_redis_plugin PRIVATE "${ROOT_DIR}/src/plugins")
set_target_properties(ark_redis_plugin PROPERTIES
    CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN YES)
