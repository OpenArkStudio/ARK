# Copyright (c) 2026 OpenArkStudio. SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20)
foreach(argument PROGRAM PLUGIN_DIR SOURCE_ROOT RUNTIME_DIR)
    if(NOT DEFINED ${argument})
        message(FATAL_ERROR "${argument} is required")
    endif()
endforeach()
foreach(sample sample1 sample2)
    set(directory "${RUNTIME_DIR}/${sample}")
    file(MAKE_DIRECTORY "${directory}/conf" "${directory}/resource/meta")
    file(COPY "${SOURCE_ROOT}/config/kernel.plugin.conf" DESTINATION "${directory}/conf")
    # Sample2 needs external business metadata. Reuse the minimal schema shape
    # from the kernel initialization regression, without changing exported data.
    file(WRITE "${directory}/resource/meta/config_class.config"
        "<xml><config id=\"attribute\" meta=\"meta/attribute.config\" res=\"meta/attribute.data\"/></xml>")
    file(WRITE "${directory}/resource/meta/entity_class.config" "<xml><config meta=\"meta/player.config\"/></xml>")
    file(WRITE "${directory}/resource/meta/attribute.config"
        "<xml><meta field=\"id\" type=\"uint64\"/><meta field=\"level\" type=\"int\"/></xml>")
    file(WRITE "${directory}/resource/meta/attribute.data" "<xml><data id=\"42\" level=\"7\"/></xml>")
    file(WRITE "${directory}/resource/meta/player.config" "<xml><meta class=\"Player\" type=\"class\"/></xml>")
    execute_process(COMMAND "${PROGRAM}" "${SOURCE_ROOT}" "${PLUGIN_DIR}" "${sample}"
        WORKING_DIRECTORY "${directory}" RESULT_VARIABLE result
        OUTPUT_VARIABLE output ERROR_VARIABLE errors TIMEOUT 15)
    file(WRITE "${directory}/output.log" "${output}\n${errors}")
    if(NOT "${result}" STREQUAL "0")
        message(FATAL_ERROR "${sample} actual DSO regression failed: ${result}\n${output}\n${errors}")
    endif()
    message(STATUS "${sample}: actual DSOs, five manager-thread updates, startup and shutdown passed")
endforeach()
