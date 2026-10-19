# Copyright (c) 2026 OpenArkStudio. SPDX-License-Identifier: Apache-2.0

foreach(argument PROGRAM PLUGIN_DIR SOURCE_ROOT RUNTIME_DIR)
    if(NOT DEFINED ${argument})
        message(FATAL_ERROR "${argument} is required")
    endif()
endforeach()

file(MAKE_DIRECTORY "${RUNTIME_DIR}")
file(TO_CMAKE_PATH "${PLUGIN_DIR}" plugin_directory)
string(APPEND plugin_directory "/")
string(REPLACE "&" "&amp;" xml_plugin_directory "${plugin_directory}")
string(REPLACE "\"" "&quot;" xml_plugin_directory "${xml_plugin_directory}")
string(REPLACE "<" "&lt;" xml_plugin_directory "${xml_plugin_directory}")
string(REPLACE "\"" "\\\"" toml_plugin_directory "${plugin_directory}")
set(plugin_name ark_app_startup_failure_plugin)

foreach(failed_phase init post_init check_config pre_update)
    foreach(format toml app.conf)
        set(manifest "${RUNTIME_DIR}/${failed_phase}.${format}")
        if(format STREQUAL "toml")
            file(WRITE "${manifest}"
                "[application]\nplugin_library_dir = \"${toml_plugin_directory}\"\n"
                "[[plugins]]\nname = \"${plugin_name}\"\nconfig = \"${failed_phase}\"\n")
        else()
            file(WRITE "${manifest}"
                "<xml><plugins plugin_lib_dir=\"${xml_plugin_directory}\">"
                "<plugin name=\"${plugin_name}\" conf=\"${failed_phase}\"/></plugins></xml>\n")
        endif()
        execute_process(
            COMMAND "${PROGRAM}" --busid=1.0.1.1 --name=startup-error-test
                "--app_conf=${manifest}"
                "--log_conf=${SOURCE_ROOT}/build/conf/log.conf"
            WORKING_DIRECTORY "${RUNTIME_DIR}"
            RESULT_VARIABLE result
            OUTPUT_VARIABLE output
            ERROR_VARIABLE errors
            TIMEOUT 15)
        file(WRITE "${manifest}.stdout.log" "${output}")
        file(WRITE "${manifest}.stderr.log" "${errors}")
        file(WRITE "${manifest}.result.txt" "${result}\n")
        if(NOT result STREQUAL "1")
            message(FATAL_ERROR "${failed_phase}/${format}: expected normal exit 1, got ${result}\n${output}\n${errors}")
        endif()
        if(NOT errors MATCHES "Application startup failed")
            message(FATAL_ERROR "${failed_phase}/${format}: missing startup diagnostic\n${errors}")
        endif()
        set(expected)
        foreach(phase init post_init check_config pre_update)
            list(APPEND expected "app-startup-test:${phase}")
            if(phase STREQUAL failed_phase)
                break()
            endif()
        endforeach()
        string(REGEX MATCHALL "app-startup-test:[a-z_]+" observed "${errors}")
        if(NOT observed STREQUAL expected)
            message(FATAL_ERROR "${failed_phase}/${format}: unexpected lifecycle calls\nexpected=${expected}\nobserved=${observed}\n${errors}")
        endif()
        message(STATUS "${failed_phase}/${format}: normal exit 1; no later phase, update or shutdown call")
    endforeach()
endforeach()
