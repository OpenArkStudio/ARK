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

# Optional legacy entity workload: neither the application nor check-core runs it.
if(ARK_BUILD_BENCHMARKS AND ARK_BUILD_CONFIG AND TARGET ark_kernel_plugin AND TARGET ark_utility_plugin)
    add_executable(ark_entity_operations_benchmark EXCLUDE_FROM_ALL entity_operations.cpp)
    set_property(TARGET ark_entity_operations_benchmark PROPERTY LINK_LIBRARIES ark::core ark::kernel ark::utility)
    add_dependencies(ark_entity_operations_benchmark ark_kernel_plugin ark_utility_plugin)
    set_target_properties(ark_entity_operations_benchmark PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
        set_target_properties(ark_entity_operations_benchmark PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_CURRENT_BINARY_DIR}")
    endforeach()
    add_custom_target(check-entity-benchmark-smoke
        COMMAND ${CMAKE_COMMAND} "-DPROGRAM=$<TARGET_FILE:ark_entity_operations_benchmark>"
            "-DPLUGIN_DIR=$<TARGET_FILE_DIR:ark_kernel_plugin>" "-DSOURCE_ROOT=${PROJECT_SOURCE_DIR}"
            "-DRUNTIME_DIR=${CMAKE_CURRENT_BINARY_DIR}/entity workload smoke"
            -P "${PROJECT_SOURCE_DIR}/benchmarks/entity_operations_smoke.cmake"
        DEPENDS ark_entity_operations_benchmark USES_TERMINAL VERBATIM)
endif()
