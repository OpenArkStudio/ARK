file(GLOB AFUtilityPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFUtilityPlugin SHARED ${AFUtilityPlugin_SRC})

set_target_properties(AFUtilityPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFUtilityPlugin_d")
set_target_properties(AFUtilityPlugin PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

target_link_libraries(AFUtilityPlugin)