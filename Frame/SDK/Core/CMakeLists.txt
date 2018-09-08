file(GLOB AFCore_SRC *.h *.hpp *.cpp *.cc *.c Common/*.h Common/*.hpp Common/*.cpp)

add_library(AFCore STATIC ${AFCore_SRC})

set_target_properties(AFCore PROPERTIES PREFIX "")
set_target_properties(AFCore PROPERTIES OUTPUT_NAME_DEBUG "AFCore_d")
set_target_properties(AFCore PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR})

target_link_libraries(AFCore)