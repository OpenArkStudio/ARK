add_library(ark_core_header_checks OBJECT headers/queue.cpp headers/guid128.cpp headers/rw_lock.cpp)
target_link_libraries(ark_core_header_checks PRIVATE ark::core)

add_executable(ark_core_tests main.cpp strings.cpp containers.cpp templates.cpp callbacks.cpp
    header_regressions.cpp concurrency.cpp plugin_manager_startup.cpp)
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

add_executable(ark_logging_tests main.cpp logging_paths.cpp)
target_link_libraries(ark_logging_tests PRIVATE ark::core)
add_custom_target(check-logging
    COMMAND $<TARGET_FILE:ark_logging_tests>
    DEPENDS ark_logging_tests
    USES_TERMINAL)
add_dependencies(check-core check-logging)

find_package(Threads REQUIRED)
add_library(ark_task_header_checks OBJECT headers/task.cpp)
# This SDK consumer uses the public core target without aggregate plugin APIs.
set_property(TARGET ark_task_header_checks PROPERTY LINK_LIBRARIES ark::core)

add_executable(ark_task_tests main.cpp tasks.cpp)
set_property(TARGET ark_task_tests PROPERTY LINK_LIBRARIES ark::core Threads::Threads)
target_include_directories(ark_task_tests PRIVATE "${PROJECT_SOURCE_DIR}/src/plugins")
add_dependencies(ark_task_tests ark_task_header_checks)

add_executable(ark_task_contract_tests ../database/task_group_contract.cpp)
set_property(TARGET ark_task_contract_tests PROPERTY LINK_LIBRARIES ark::core Threads::Threads)
add_custom_target(check-task-sdk
    COMMAND $<TARGET_FILE:ark_task_tests>
    COMMAND ${CMAKE_COMMAND} "-DPROGRAM=$<TARGET_FILE:ark_task_contract_tests>"
        -P "${PROJECT_SOURCE_DIR}/tests/database/task_group_contract.cmake"
    DEPENDS ark_task_tests ark_task_contract_tests
    USES_TERMINAL)
add_dependencies(check-core check-task-sdk)

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

add_executable(ark_xml_tests main.cpp xml.cpp)
target_link_libraries(ark_xml_tests PRIVATE ark::core)
target_compile_definitions(ark_xml_tests PRIVATE ARK_TEST_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
add_custom_target(check-xml
    COMMAND $<TARGET_FILE:ark_xml_tests>
    DEPENDS ark_xml_tests
    USES_TERMINAL)
add_dependencies(check-core check-xml)

if(ARK_BUILD_CONFIG)
    add_executable(ark_application_manifest_tests main.cpp application_manifest.cpp)
    # This configuration-only consumer deliberately excludes the inherited network SDK.
    set_property(TARGET ark_application_manifest_tests PROPERTY LINK_LIBRARIES ark::config)
    target_compile_definitions(ark_application_manifest_tests PRIVATE ARK_TEST_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
    add_custom_target(check-application-manifest
        COMMAND $<TARGET_FILE:ark_application_manifest_tests>
        DEPENDS ark_application_manifest_tests
        USES_TERMINAL)
    add_dependencies(check-core check-application-manifest)

    if(TARGET app)
        add_custom_target(check-app-manifest-failures
            COMMAND ${CMAKE_COMMAND} "-DPROGRAM=$<TARGET_FILE:app>"
                "-DSOURCE_ROOT=${PROJECT_SOURCE_DIR}"
                "-DRUNTIME_DIR=${CMAKE_CURRENT_BINARY_DIR}/manifest failure runtime"
                -P "${PROJECT_SOURCE_DIR}/tests/app/manifest_failures.cmake"
            DEPENDS app
            USES_TERMINAL)
        add_dependencies(check-core check-app-manifest-failures)
    endif()
endif()

if(ARK_BUILD_PROTO)
    add_executable(ark_kernel_initialization_tests main.cpp module_initialization.cpp entity_config_copy.cpp
        ../../src/plugins/kernel/class_call_back_manager.cpp
        ../../src/plugins/kernel/class_meta.cpp
        ../../src/plugins/kernel/class_meta_module_impl.cpp
        ../../src/plugins/kernel/config_module_impl.cpp
        ../../src/plugins/kernel/container_impl.cpp
        ../../src/plugins/kernel/container_manager_impl.cpp
        ../../src/plugins/kernel/entity_impl.cpp
        ../../src/plugins/kernel/kernel_module_impl.cpp
        ../../src/plugins/kernel/map_module_impl.cpp
        ../../src/plugins/kernel/node_manager.cpp
        ../../src/plugins/kernel/row.cpp
        ../../src/plugins/kernel/static_entity_impl.cpp
        ../../src/plugins/kernel/table_impl.cpp
        ../../src/plugins/kernel/table_meta.cpp)
    target_link_libraries(ark_kernel_initialization_tests PRIVATE ark::kernel ark::utility ark::proto)
    add_custom_target(check-kernel-initialization
        COMMAND $<TARGET_FILE:ark_kernel_initialization_tests>
        DEPENDS ark_kernel_initialization_tests
        USES_TERMINAL)
    add_dependencies(check-core check-kernel-initialization)
endif()
