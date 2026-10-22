# Copyright (c) 2026 OpenArkStudio. SPDX-License-Identifier: Apache-2.0
add_library(http_server_plugin SHARED http_server_module.cpp http_server_plugin.cpp)
set_property(TARGET http_server_plugin PROPERTY LINK_LIBRARIES ark::http)
set_target_properties(http_server_plugin PROPERTIES PREFIX ""
    ARCHIVE_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}")
