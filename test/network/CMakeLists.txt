add_executable(ark_network_tests
    main.cpp
    framing.cpp
    logging.cpp
    tcp_websocket.cpp
    rudp.cpp
    http.cpp
    ../../src/plugin/net/src/AFCTCPClient.cpp
    ../../src/plugin/net/src/AFCTCPServer.cpp
    ../../src/plugin/net/src/AFCWebSocketClient.cpp
    ../../src/plugin/net/src/AFCWebSocketServer.cpp
    ../../src/plugin/net/src/AFCRDPClient.cpp
    ../../src/plugin/net/src/AFCRDPServer.cpp
    ../../src/plugin/net/src/AFCHttpClientModule.cpp
    ../../src/plugin/net/src/AFCHttpServer.cpp
    ../../src/plugin/http/src/AFCHttpServerModule.cpp)
target_include_directories(ark_network_tests PRIVATE ${ROOT_DIR}/src/base)
add_custom_target(check-network
    COMMAND $<TARGET_FILE:ark_network_tests>
    DEPENDS ark_network_tests
    USES_TERMINAL)
