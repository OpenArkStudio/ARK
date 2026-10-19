if(NOT ARKNET_ENABLE_SSL)
    message(FATAL_ERROR "The native MySQL plugin requires ARKNET_ENABLE_SSL for authentication and TLS")
endif()
BUILD_PLUGIN_MACRO(AFMysqlPlugin arknet::arknet)
