# Copyright (c) 2026 OpenArkStudio. SPDX-License-Identifier: Apache-2.0

if(NOT DEFINED PROGRAM)
    message(FATAL_ERROR "PROGRAM is required")
endif()
foreach(mode signal-int signal-term concurrent worker-signal command)
    message(STATUS "Application stop-request check: ${mode}")
    execute_process(COMMAND "${PROGRAM}" "${mode}"
        RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors TIMEOUT 15)
    if(NOT result STREQUAL "0")
        message(FATAL_ERROR "${mode} failed: ${result}\n${output}\n${errors}")
    endif()
    message(STATUS "${output}")
endforeach()
