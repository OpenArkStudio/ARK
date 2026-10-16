# Copyright (c) 2026 OpenArkStudio. SPDX-License-Identifier: Apache-2.0

foreach(argument PROGRAM SOURCE_ROOT RUNTIME_DIR)
    if(NOT DEFINED ${argument})
        message(FATAL_ERROR "${argument} is required")
    endif()
endforeach()

file(MAKE_DIRECTORY "${RUNTIME_DIR}")
file(WRITE "${RUNTIME_DIR}/invalid.toml" "[application\n")
file(WRITE "${RUNTIME_DIR}/invalid.app.conf" "<xml><plugins></xml>")
file(WRITE "${RUNTIME_DIR}/wrong-type.toml" "[application]\nplugin_library_dir = 42\n")

foreach(manifest missing.toml invalid.toml invalid.app.conf wrong-type.toml)
    execute_process(
        COMMAND "${PROGRAM}" --busid=1.0.1.1 --name=manifest-error-test
            "--app_conf=${RUNTIME_DIR}/${manifest}"
            "--log_conf=${SOURCE_ROOT}/build/conf/log.conf"
        WORKING_DIRECTORY "${RUNTIME_DIR}"
        RESULT_VARIABLE result
        OUTPUT_VARIABLE output
        ERROR_VARIABLE errors
        TIMEOUT 15)
    if(NOT result STREQUAL "1")
        message(FATAL_ERROR "${manifest}: expected normal exit 1, got ${result}\n${output}\n${errors}")
    endif()
    if(NOT errors MATCHES "Application manifest loading failed:")
        message(FATAL_ERROR "${manifest}: missing manifest diagnostic\n${errors}")
    endif()
    message(STATUS "${manifest}: normal exit 1 with manifest diagnostic")
endforeach()
