# Copyright (c) 2026 OpenArkStudio. SPDX-License-Identifier: Apache-2.0
add_library(http_client_sample_plugin SHARED http_client_sample_module.cpp http_client_sample_plugin.cpp)
set_property(TARGET http_client_sample_plugin PROPERTY LINK_LIBRARIES ark::net ark::etcd_client)
set_target_properties(http_client_sample_plugin PROPERTIES PREFIX ""
    ARCHIVE_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}")
