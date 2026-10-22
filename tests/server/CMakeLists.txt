# Copyright (c) 2026 OpenArkStudio. SPDX-License-Identifier: Apache-2.0
add_custom_target(check-server-plugins)
foreach(server game login proxy world master dir router)
    set(source "${PROJECT_SOURCE_DIR}/src/server/${server}")
    configure_file(types.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/${server}/server_test_types.hpp" @ONLY)
    set(test_target ark_${server}_construction_tests)
    # Compile the real private factory and assembly into the test as well: Windows
    # does not export those C++ internals from the runtime DLL.
    add_executable(${test_target} construction.cpp
        "${source}/${server}_plugin.cpp" "${source}/${server}_net_module_impl.cpp")
    set_property(TARGET ${test_target} PROPERTY LINK_LIBRARIES ark::ark ark_server_compat_headers)
    target_include_directories(${test_target} PRIVATE "${source}" "${CMAKE_CURRENT_BINARY_DIR}/${server}")
    add_dependencies(${test_target} ark_${server}_plugin)
    add_custom_target(check-server-${server}
        COMMAND $<TARGET_FILE:${test_target}> "$<TARGET_FILE_DIR:ark_${server}_plugin>/" "ark_${server}_plugin"
        DEPENDS ${test_target} ark_${server}_plugin USES_TERMINAL VERBATIM)
    add_dependencies(check-server-plugins check-server-${server})
endforeach()

# A counted substitute verifies rejected-factory ownership through the actual
# Login assembly. This does not replace the real-factory tests above.
add_executable(ark_server_registration_tests registration.cpp
    "${PROJECT_SOURCE_DIR}/src/server/login/login_plugin.cpp")
set_property(TARGET ark_server_registration_tests PROPERTY LINK_LIBRARIES ark::ark ark_server_compat_headers)
target_include_directories(ark_server_registration_tests PRIVATE "${PROJECT_SOURCE_DIR}/src/server/login")
add_custom_target(check-server-registration
    COMMAND $<TARGET_FILE:ark_server_registration_tests>
    DEPENDS ark_server_registration_tests USES_TERMINAL)
add_dependencies(check-server-plugins check-server-registration)

# Exercise the actual Login entry/exit functions through the real manager loader
# with a controlled module, without invoking business network initialization.
add_library(ark_server_entry_probe SHARED entry_factory.cpp
    "${PROJECT_SOURCE_DIR}/src/server/login/login_plugin.cpp")
set_property(TARGET ark_server_entry_probe PROPERTY LINK_LIBRARIES ark::ark)
target_include_directories(ark_server_entry_probe PRIVATE "${PROJECT_SOURCE_DIR}/src/server/login")
set_target_properties(ark_server_entry_probe PROPERTIES PREFIX "")
add_executable(ark_server_entry_tests entry.cpp)
set_property(TARGET ark_server_entry_tests PROPERTY LINK_LIBRARIES ark::ark ark_server_compat_headers)
add_custom_target(check-server-entry
    COMMAND $<TARGET_FILE:ark_server_entry_tests> "$<TARGET_FILE_DIR:ark_server_entry_probe>/" "ark_server_entry_probe"
    DEPENDS ark_server_entry_tests ark_server_entry_probe USES_TERMINAL VERBATIM)
add_dependencies(check-server-plugins check-server-entry)
