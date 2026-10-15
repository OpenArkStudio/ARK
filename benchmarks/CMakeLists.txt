add_executable(ark_network_buffer_benchmark network_buffers.cpp)
target_link_libraries(ark_network_buffer_benchmark PRIVATE ark::core ark::net)
set_target_properties(ark_network_buffer_benchmark PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}")
