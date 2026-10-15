add_executable(ark_network_tests
    main.cpp
    framing.cpp
    logging.cpp
    endpoint_lifetime.cpp
    tcp_websocket.cpp
    rudp.cpp
    http.cpp
    ../../plugins/net/src/tcp_client.cpp
    ../../plugins/net/src/tcp_server.cpp
    ../../plugins/net/src/websocket_client.cpp
    ../../plugins/net/src/websocket_server.cpp
    ../../plugins/net/src/rudp_client.cpp
    ../../plugins/net/src/rudp_server.cpp
    ../../plugins/net/src/http_client_module_impl.cpp
    ../../plugins/net/src/http_server.cpp
    ../../plugins/http/src/http_server_module_impl.cpp)
add_custom_target(check-network
    COMMAND $<TARGET_FILE:ark_network_tests>
    DEPENDS ark_network_tests
    USES_TERMINAL)
