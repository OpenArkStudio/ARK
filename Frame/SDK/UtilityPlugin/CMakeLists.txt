file(GLOB AFUtility_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFUtility SHARED ${AFUtility_SRC})

set_target_properties(AFUtility PROPERTIES OUTPUT_NAME_DEBUG "AFUtility_d")
set_target_properties(AFUtility PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

target_link_libraries(AFUtility)