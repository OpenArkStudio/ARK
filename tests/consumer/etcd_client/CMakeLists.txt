cmake_minimum_required(VERSION 3.21)
project(ark_etcd_client_header_consumer LANGUAGES CXX)
get_filename_component(ARK_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
set(ARK_BUILD_APPLICATIONS OFF CACHE BOOL "Build applications" FORCE)
set(ARK_BUILD_PROTO OFF CACHE BOOL "Build protocol messages" FORCE)
set(ARK_BUILD_CONFIG OFF CACHE BOOL "Build configuration reader" FORCE)
set(ARK_BUILD_NETWORK_TESTS OFF CACHE BOOL "Build network tests" FORCE)
set(ARK_BUILD_CORE_TESTS OFF CACHE BOOL "Build core tests" FORCE)
set(ARK_BUILD_TESTS OFF CACHE BOOL "Build legacy tests" FORCE)
set(ARK_BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
set(ARK_BUILD_PLUGIN_ETCD_CLIENT OFF CACHE BOOL "Build Etcd Client runtime plugin" FORCE)
add_subdirectory("${ARK_SOURCE_DIR}" ark)

# Retain the entire existing public surface, including implementation and assembly.
set(etcd_client_headers
    etcd_client.hpp
    etcd_client_impl.hpp
    etcd_client_module.hpp
    etcd_client_module_impl.hpp
    etcd_client_plugin.hpp
)
set(header_checks)
foreach(header IN LISTS etcd_client_headers)
    string(MAKE_C_IDENTIFIER "${header}" identifier)
    set(source "${CMAKE_CURRENT_BINARY_DIR}/${identifier}.cpp")
    file(WRITE "${source}" "#include <ark/etcd_client/${header}>\n")
    list(APPEND header_checks "${source}")
endforeach()
add_executable(ark_etcd_client_header_consumer main.cpp ${header_checks})
target_link_libraries(ark_etcd_client_header_consumer PRIVATE ark::etcd_client)
add_custom_target(check-etcd-client-headers
    COMMAND $<TARGET_FILE:ark_etcd_client_header_consumer>
    DEPENDS ark_etcd_client_header_consumer USES_TERMINAL)
