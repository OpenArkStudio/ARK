file(GLOB GameNetClientPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFGameNetClientPlugin SHARED ${GameNetClientPlugin_SRC})

set_target_properties(AFGameNetClientPlugin PROPERTIES PREFIX "")
set_target_properties(AFGameNetClientPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFGameNetClientPlugin_d")
set_target_properties(AFGameNetClientPlugin PROPERTIES
    FOLDER "Server/GameServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

LINK_SDK_MACRO("AFGameNetClientPlugin")