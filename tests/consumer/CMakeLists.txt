cmake_minimum_required(VERSION 3.21)
project(ark_sdk_consumer LANGUAGES CXX)

if(NOT ARK_SOURCE_DIR)
    get_filename_component(ARK_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
endif()
set(ARK_BUILD_APPLICATIONS OFF CACHE BOOL "Build applications" FORCE)
set(ARK_BUILD_PROTO OFF CACHE BOOL "Build protocol messages" FORCE)
set(ARK_BUILD_NETWORK_TESTS OFF CACHE BOOL "Build network tests" FORCE)
set(ARK_BUILD_TESTS OFF CACHE BOOL "Build legacy tests" FORCE)
set(ARK_BUILD_EXAMPLES OFF CACHE BOOL "Build examples" FORCE)
add_subdirectory("${ARK_SOURCE_DIR}" ark)

add_executable(ark_sdk_consumer main.cpp headers.cpp dependencies.cpp database_headers.cpp utility_headers.cpp)
target_link_libraries(ark_sdk_consumer PRIVATE ark::core ark::net ark::kernel ark::mysql ark::redis ark::utility)
add_custom_target(check-sdk
    COMMAND $<TARGET_FILE:ark_sdk_consumer>
    DEPENDS ark_sdk_consumer
    USES_TERMINAL)
