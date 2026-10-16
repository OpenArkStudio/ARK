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
ark_database_test(ark_database_redis_tests redis_service.cpp)
target_link_libraries(ark_database_redis_tests PRIVATE arknet::arknet)
ark_database_test(ark_database_redis_stop_tests redis_stop_capture.cpp)
target_link_libraries(ark_database_redis_stop_tests PRIVATE arknet::arknet)
add_custom_target(check-database
    COMMAND $<TARGET_FILE:ark_database_queue_tests>
    COMMAND $<TARGET_FILE:ark_database_redis_stop_tests>
    COMMAND $<TARGET_FILE:ark_database_task_tests>
    COMMAND ${CMAKE_COMMAND} -DPROGRAM=$<TARGET_FILE:ark_database_task_contract_tests>
        -P "${CMAKE_CURRENT_SOURCE_DIR}/task_group_contract.cmake"
    DEPENDS ark_database_queue_tests ark_database_redis_stop_tests ark_database_task_tests ark_database_task_contract_tests
    USES_TERMINAL)

if(ARKNET_ENABLE_SSL)
    if(NOT EXISTS "${ARKNET_SOURCE_DIR}/include/arknet/mysql/pool.hpp")
        message(FATAL_ERROR "MySQL development tests require a local arknet checkout with the native MySQL pool")
    endif()
    ark_database_test(ark_database_mysql_tests mysql_service.cpp)
    target_link_libraries(ark_database_mysql_tests PRIVATE arknet::arknet)
else()
    message(STATUS "Native MySQL development test disabled: enable ARKNET_ENABLE_SSL for authentication and TLS")
endif()
