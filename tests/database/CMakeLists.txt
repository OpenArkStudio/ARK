# Development-only native database bridges; no runtime plugin is enabled here.
find_package(Threads REQUIRED)
if(NOT EXISTS "${ARKNET_SOURCE_DIR}/include/arknet/redis/pool.hpp")
    message(FATAL_ERROR "Database development tests require a local arknet checkout with the native Redis pool")
endif()

function(ark_database_test target)
    add_executable(${target} ${ARGN})
    target_include_directories(${target} PRIVATE "${ROOT_DIR}/src/plugins")
    target_link_libraries(${target} PRIVATE Threads::Threads)
    set_target_properties(${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
    foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
        set_target_properties(${target} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_CURRENT_BINARY_DIR}")
    endforeach()
endfunction()

ark_database_test(ark_database_queue_tests main.cpp completion_queue.cpp request_control.cpp)
if(NOT EXISTS "${ARKNET_SOURCE_DIR}/include/arknet/base/manual_runtime.hpp")
    message(FATAL_ERROR "Database coroutine development tests require arknet manual_runtime")
endif()
ark_database_test(ark_database_task_tests main.cpp task_group.cpp awaitable_lifetime.cpp)
target_link_libraries(ark_database_task_tests PRIVATE arknet::arknet)
ark_database_test(ark_database_task_contract_tests task_group_contract.cpp)
target_link_libraries(ark_database_task_contract_tests PRIVATE arknet::arknet)
ark_database_test(ark_database_service_awaitable_tests service_awaitable.cpp)
target_link_libraries(ark_database_service_awaitable_tests PRIVATE arknet::arknet)
ark_database_test(ark_database_operations_tests main.cpp operations_awaitable.cpp)
target_link_libraries(ark_database_operations_tests PRIVATE arknet::arknet)
ark_database_test(ark_database_service_retirement_tests main.cpp service_retirement.cpp)
target_link_libraries(ark_database_service_retirement_tests PRIVATE arknet::arknet)
ark_database_test(ark_database_redis_tests redis_service.cpp)
target_link_libraries(ark_database_redis_tests PRIVATE arknet::arknet)
ark_database_test(ark_database_redis_stop_tests redis_stop_capture.cpp)
target_link_libraries(ark_database_redis_stop_tests PRIVATE arknet::arknet)
ark_database_test(ark_database_public_redis_tests main.cpp public_redis_module.cpp
    "${ROOT_DIR}/src/plugins/redis/redis_module_impl.cpp")
target_link_libraries(ark_database_public_redis_tests PRIVATE ark::core arknet::arknet)
add_library(ark_database_redis_plugin SHARED
    "${ROOT_DIR}/src/plugins/redis/redis_plugin.cpp" "${ROOT_DIR}/src/plugins/redis/redis_module_impl.cpp")
target_include_directories(ark_database_redis_plugin PRIVATE "${ROOT_DIR}/src/plugins")
target_link_libraries(ark_database_redis_plugin PRIVATE ark::core arknet::arknet)
# Keep implementation symbols private so this exercises separate Asio executor
# type-erasure tables in the executable and dynamically loaded plugin.
set_target_properties(ark_database_redis_plugin PROPERTIES PREFIX ""
    CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN YES)
ark_database_test(ark_database_public_redis_plugin_tests public_redis_plugin.cpp)
target_link_libraries(ark_database_public_redis_plugin_tests PRIVATE ark::core arknet::arknet)
target_compile_definitions(ark_database_public_redis_plugin_tests PRIVATE
    ARK_DATABASE_PLUGIN_DIR="$<TARGET_FILE_DIR:ark_database_redis_plugin>/")
add_dependencies(ark_database_public_redis_plugin_tests ark_database_redis_plugin)
add_library(ark_database_redis_sample_check OBJECT "${ROOT_DIR}/examples/redis_sample/redis_sample_module.cpp")
target_link_libraries(ark_database_redis_sample_check PRIVATE ark::core arknet::arknet)
add_custom_target(check-database
    COMMAND $<TARGET_FILE:ark_database_queue_tests>
    COMMAND $<TARGET_FILE:ark_database_redis_stop_tests>
    COMMAND $<TARGET_FILE:ark_database_public_redis_tests>
    COMMAND $<TARGET_FILE:ark_database_public_redis_plugin_tests>
    COMMAND $<TARGET_FILE:ark_database_task_tests>
    COMMAND $<TARGET_FILE:ark_database_operations_tests>
    COMMAND $<TARGET_FILE:ark_database_service_retirement_tests>
    COMMAND ${CMAKE_COMMAND} -DPROGRAM=$<TARGET_FILE:ark_database_task_contract_tests>
        -P "${CMAKE_CURRENT_SOURCE_DIR}/task_group_contract.cmake"
    DEPENDS ark_database_queue_tests ark_database_redis_stop_tests ark_database_public_redis_tests
        ark_database_public_redis_plugin_tests ark_database_task_tests ark_database_operations_tests
        ark_database_service_retirement_tests
        ark_database_task_contract_tests
    USES_TERMINAL)

add_dependencies(check-database ark_database_redis_sample_check)

if(ARKNET_ENABLE_SSL)
    target_compile_definitions(ark_database_public_redis_tests PRIVATE
        ARK_DATABASE_REDIS_CERT_DIR="${ARKNET_SOURCE_DIR}/tests/certs")
    ark_database_test(ark_database_public_mysql_tests main.cpp public_mysql_module.cpp
        "${ROOT_DIR}/src/plugins/mysql/mysql_module_impl.cpp")
    target_include_directories(ark_database_public_mysql_tests PRIVATE "${ARKNET_SOURCE_DIR}/tests")
    target_compile_definitions(ark_database_public_mysql_tests PRIVATE
        ARKNET_TEST_CERT_DIR="${ARKNET_SOURCE_DIR}/tests/certs")
    target_link_libraries(ark_database_public_mysql_tests PRIVATE ark::core arknet::arknet)
    add_library(ark_database_mysql_plugin SHARED
        "${ROOT_DIR}/src/plugins/mysql/mysql_plugin.cpp" "${ROOT_DIR}/src/plugins/mysql/mysql_module_impl.cpp")
    target_include_directories(ark_database_mysql_plugin PRIVATE "${ROOT_DIR}/src/plugins")
    target_link_libraries(ark_database_mysql_plugin PRIVATE ark::core arknet::arknet)
    set_target_properties(ark_database_mysql_plugin PROPERTIES PREFIX ""
        CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN YES)
    ark_database_test(ark_database_public_mysql_plugin_tests public_mysql_plugin.cpp)
    target_link_libraries(ark_database_public_mysql_plugin_tests PRIVATE ark::core arknet::arknet)
    target_compile_definitions(ark_database_public_mysql_plugin_tests PRIVATE
        ARK_DATABASE_PLUGIN_DIR="$<TARGET_FILE_DIR:ark_database_mysql_plugin>/")
    add_dependencies(ark_database_public_mysql_plugin_tests ark_database_mysql_plugin)
    add_library(ark_database_mysql_sample_check OBJECT "${ROOT_DIR}/examples/mysql_sample/mysql_sample_module.cpp")
    target_link_libraries(ark_database_mysql_sample_check PRIVATE ark::core arknet::arknet)
    add_custom_target(check-database-public-mysql
        COMMAND $<TARGET_FILE:ark_database_public_mysql_tests>
        COMMAND $<TARGET_FILE:ark_database_public_mysql_plugin_tests>
        DEPENDS ark_database_public_mysql_tests ark_database_public_mysql_plugin_tests ark_database_mysql_sample_check USES_TERMINAL)
    add_dependencies(check-database check-database-public-mysql)
    ark_database_test(ark_database_mysql_transaction_awaitable_tests main.cpp transactions_awaitable.cpp)
    target_link_libraries(ark_database_mysql_transaction_awaitable_tests PRIVATE arknet::arknet)
    add_custom_target(check-database-mysql-transaction-awaitable
        COMMAND $<TARGET_FILE:ark_database_mysql_transaction_awaitable_tests>
        DEPENDS ark_database_mysql_transaction_awaitable_tests USES_TERMINAL)
    add_dependencies(check-database check-database-mysql-transaction-awaitable)
    ark_database_test(ark_database_mysql_transaction_state_tests main.cpp mysql_transaction_state.cpp)
    target_link_libraries(ark_database_mysql_transaction_state_tests PRIVATE arknet::arknet)
    ark_database_test(ark_database_mysql_transaction_tests mysql_transactions.cpp)
    target_link_libraries(ark_database_mysql_transaction_tests PRIVATE arknet::arknet)
    add_custom_target(check-database-mysql-transaction-state
        COMMAND $<TARGET_FILE:ark_database_mysql_transaction_state_tests>
        DEPENDS ark_database_mysql_transaction_state_tests USES_TERMINAL)
    add_dependencies(check-database check-database-mysql-transaction-state)

    set(ark_database_redis_cert_dir "${ARKNET_SOURCE_DIR}/tests/certs")
    foreach(certificate ca.pem server.pem server-key.pem wrong.pem ipv6/ca.pem)
        if(NOT EXISTS "${ark_database_redis_cert_dir}/${certificate}")
            message(FATAL_ERROR "Redis TLS regression requires arknet public test fixtures: ${certificate}")
        endif()
    endforeach()
    ark_database_test(ark_database_redis_tls_tests main.cpp redis_service_tls.cpp)
    target_link_libraries(ark_database_redis_tls_tests PRIVATE arknet::arknet)
    target_compile_definitions(ark_database_redis_tls_tests PRIVATE
        ARK_DATABASE_REDIS_CERT_DIR="${ark_database_redis_cert_dir}")
    add_custom_target(check-database-redis-tls
        COMMAND $<TARGET_FILE:ark_database_redis_tls_tests>
        DEPENDS ark_database_redis_tls_tests USES_TERMINAL)
    add_dependencies(check-database check-database-redis-tls)
    if(NOT EXISTS "${ARKNET_SOURCE_DIR}/include/arknet/mysql/pool.hpp")
        message(FATAL_ERROR "MySQL development tests require a local arknet checkout with the native MySQL pool")
    endif()
    ark_database_test(ark_database_mysql_tests mysql_service.cpp)
    target_link_libraries(ark_database_mysql_tests PRIVATE arknet::arknet)
else()
    message(STATUS "Native MySQL development test disabled: enable ARKNET_ENABLE_SSL for authentication and TLS")
endif()
