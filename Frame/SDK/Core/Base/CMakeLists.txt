file(GLOB AFBase_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFBase_SRC STATIC ${AFBase_SRC})

set_target_properties(AFBase PROPERTIES OUTPUT_NAME_DEBUG "AFBase_d")
set_target_properties(AFBase PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

target_link_libraries(AFBase)