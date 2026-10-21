find_package(nlohmann_json CONFIG REQUIRED)

BUILD_PLUGIN_MACRO(ark_etcd_client_plugin nlohmann_json::nlohmann_json)
