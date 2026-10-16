add_library(ark_core_header_checks OBJECT headers/queue.cpp headers/guid128.cpp headers/rw_lock.cpp)
target_link_libraries(ark_core_header_checks PRIVATE ark::core)

add_executable(ark_core_tests main.cpp strings.cpp containers.cpp templates.cpp callbacks.cpp
    header_regressions.cpp concurrency.cpp)
target_link_libraries(ark_core_tests PRIVATE ark::core ark::utility)
add_dependencies(ark_core_tests ark_core_header_checks)
if(ARK_BUILD_PROTO)
    target_sources(ark_core_tests PRIVATE protobuf_messages.cpp)
    target_link_libraries(ark_core_tests PRIVATE ark::bus ark::proto)
endif()
add_custom_target(check-core
    COMMAND $<TARGET_FILE:ark_core_tests>
    DEPENDS ark_core_tests
    USES_TERMINAL)

find_package(Threads REQUIRED)
add_executable(ark_app_shutdown_tests ../app/shutdown_request.cpp)
target_compile_features(ark_app_shutdown_tests PRIVATE cxx_std_20)
target_include_directories(ark_app_shutdown_tests PRIVATE "${PROJECT_SOURCE_DIR}/src/app")
# This app-private state test does not consume the directory's inherited ark SDK.
set_property(TARGET ark_app_shutdown_tests PROPERTY LINK_LIBRARIES Threads::Threads)
add_custom_target(check-app-shutdown
    COMMAND ${CMAKE_COMMAND} "-DPROGRAM=$<TARGET_FILE:ark_app_shutdown_tests>"
        -P "${PROJECT_SOURCE_DIR}/tests/app/shutdown_request.cmake"
    DEPENDS ark_app_shutdown_tests
    USES_TERMINAL)
add_dependencies(check-core check-app-shutdown)

if(ARK_BUILD_PROTO)
    add_executable(ark_kernel_initialization_tests main.cpp module_initialization.cpp entity_config_copy.cpp
        ../../plugins/kernel/src/class_call_back_manager.cpp
        ../../plugins/kernel/src/class_meta.cpp
        ../../plugins/kernel/src/class_meta_module_impl.cpp
        ../../plugins/kernel/src/config_module_impl.cpp
        ../../plugins/kernel/src/container_impl.cpp
        ../../plugins/kernel/src/container_manager_impl.cpp
        ../../plugins/kernel/src/entity_impl.cpp
        ../../plugins/kernel/src/kernel_module_impl.cpp
        ../../plugins/kernel/src/map_module_impl.cpp
        ../../plugins/kernel/src/node_manager.cpp
        ../../plugins/kernel/src/row.cpp
        ../../plugins/kernel/src/static_entity_impl.cpp
        ../../plugins/kernel/src/table_impl.cpp
        ../../plugins/kernel/src/table_meta.cpp)
    target_link_libraries(ark_kernel_initialization_tests PRIVATE ark::kernel ark::utility ark::proto)
    add_custom_target(check-kernel-initialization
        COMMAND $<TARGET_FILE:ark_kernel_initialization_tests>
        DEPENDS ark_kernel_initialization_tests
        USES_TERMINAL)
    add_dependencies(check-core check-kernel-initialization)
endif()
