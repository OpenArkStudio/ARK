if(ARK_BUILD_BENCHMARKS)
    add_executable(ark_network_buffer_benchmark network_buffers.cpp)
    target_link_libraries(ark_network_buffer_benchmark PRIVATE ark::core ark::net)
    set_target_properties(ark_network_buffer_benchmark PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_BINARY_DIR}")
endif()

if(ARK_BUILD_DATABASE_BENCHMARKS)
    if(NOT EXISTS "${ARKNET_SOURCE_DIR}/include/arknet/base/manual_runtime.hpp" OR
       NOT EXISTS "${ARKNET_SOURCE_DIR}/include/arknet/redis/pool.hpp")
        message(FATAL_ERROR "Database benchmark requires a local arknet source with manual_runtime and native pools")
    endif()
    find_package(Threads REQUIRED)
    add_executable(ark_database_benchmark database.cpp)
    target_include_directories(ark_database_benchmark PRIVATE "${ROOT_DIR}/src/plugins")
    target_link_libraries(ark_database_benchmark PRIVATE arknet::arknet Threads::Threads)
    if(WIN32)
        target_link_libraries(ark_database_benchmark PRIVATE psapi)
    endif()
    set_target_properties(ark_database_benchmark PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
        set_target_properties(ark_database_benchmark PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_CURRENT_BINARY_DIR}")
    endforeach()
    add_custom_target(check-database-benchmark
        COMMAND $<TARGET_FILE:ark_database_benchmark> --self-test
        DEPENDS ark_database_benchmark USES_TERMINAL)
endif()
