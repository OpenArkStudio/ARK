file(GLOB WorldNetClientPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFWorldNetClientPlugin SHARED ${WorldNetClientPlugin_SRC})

set_target_properties(AFWorldNetClientPlugin PREFIX "")
set_target_properties(AFWorldNetClientPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFWorldNetClientPlugin_d")
set_target_properties(AFWorldNetClientPlugin PROPERTIES
    FOLDER "Server/WorldServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

LINK_SDK_MACRO("AFWorldNetClientPlugin")