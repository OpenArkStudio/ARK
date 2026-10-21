if(NOT ARKNET_ENABLE_SSL)
    message(FATAL_ERROR "The native MySQL plugin requires ARKNET_ENABLE_SSL for authentication and TLS")
endif()
BUILD_PLUGIN_MACRO(ark_mysql_plugin arknet::arknet)
target_include_directories(ark_mysql_plugin PRIVATE "${ROOT_DIR}/src/plugins")
set_target_properties(ark_mysql_plugin PROPERTIES
    CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN YES)
