file(GLOB AFConfig_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFConfigPlugin SHARED ${AFConfig_SRC})

add_dependencies(AFConfigPlugin AFCore)

set_target_properties(AFConfigPlugin PROPERTIES PREFIX "")
set_target_properties(AFConfigPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFConfigPlugin_d")
set_target_properties(AFConfigPlugin PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

target_link_libraries(AFConfigPlugin AFCore)