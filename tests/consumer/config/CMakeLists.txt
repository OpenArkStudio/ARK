cmake_minimum_required(VERSION 3.21)
project(ark_configuration_consumer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(MSVC)
    add_compile_options(/utf-8 /Zc:__cplusplus)
endif()

# Deliberately configure only the configuration module: no arknet/core/runtime.
get_filename_component(ARK_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
option(ARK_BUILD_CONFIG "Build and test the compiled configuration reader" ON)
set(ARK_BUILD_NETWORK_TESTS OFF)
add_subdirectory("${ARK_SOURCE_DIR}/src/config" config)

add_executable(ark_config_header_consumer headers.cpp)
target_link_libraries(ark_config_header_consumer PRIVATE ark::config_headers)
add_custom_target(check-config-headers COMMAND $<TARGET_FILE:ark_config_header_consumer>
    DEPENDS ark_config_header_consumer USES_TERMINAL)

if(ARK_BUILD_CONFIG)
    add_library(ark_config_shared_probe SHARED shared.cpp)
    target_link_libraries(ark_config_shared_probe PRIVATE ark::config tomlplusplus::tomlplusplus)
    target_include_directories(ark_config_shared_probe PRIVATE "${ARK_SOURCE_DIR}/src/plugins")
    target_compile_definitions(ark_config_shared_probe PRIVATE ARK_CONFIG_PROBE_BUILD)

    add_executable(ark_config_consumer "${ARK_SOURCE_DIR}/tests/core/main.cpp"
        "${ARK_SOURCE_DIR}/tests/core/application_manifest.cpp"
        "${ARK_SOURCE_DIR}/tests/core/plugin_configuration.cpp" shared_library.cpp)
    target_link_libraries(ark_config_consumer PRIVATE ark::config ark_config_shared_probe tomlplusplus::tomlplusplus)
    target_include_directories(ark_config_consumer PRIVATE "${ARK_SOURCE_DIR}/src/plugins")
    target_compile_definitions(ark_config_consumer PRIVATE ARK_TEST_SOURCE_DIR="${ARK_SOURCE_DIR}")
    add_custom_target(check-config COMMAND $<TARGET_FILE:ark_config_consumer>
        DEPENDS ark_config_consumer check-config-headers USES_TERMINAL)
elseif(TARGET ark_config OR TARGET tomlplusplus::tomlplusplus OR TARGET arknet::arknet)
    message(FATAL_ERROR "Declaration-only configuration consumer acquired implementation dependencies")
endif()
