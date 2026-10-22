add_executable(ark_network_tests
    main.cpp
    framing.cpp
    logging.cpp
    endpoint_lifetime.cpp
    tcp_websocket.cpp
    rudp.cpp
    http.cpp
    ../../src/plugins/net/tcp_client.cpp
    ../../src/plugins/net/tcp_server.cpp
    ../../src/plugins/net/websocket_client.cpp
    ../../src/plugins/net/websocket_server.cpp
    ../../src/plugins/net/rudp_client.cpp
    ../../src/plugins/net/rudp_server.cpp
    ../../src/plugins/net/http_client_module_impl.cpp
    ../../src/plugins/net/http_server.cpp
    ../../src/plugins/http/http_server_module_impl.cpp
    ../../src/plugins/http/http_plugin.cpp)
target_link_libraries(ark_network_tests PRIVATE tomlplusplus::tomlplusplus)
target_include_directories(ark_network_tests PRIVATE "${PROJECT_SOURCE_DIR}/src/plugins")
add_custom_target(check-network
    COMMAND $<TARGET_FILE:ark_network_tests>
    DEPENDS ark_network_tests
    USES_TERMINAL)
