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
