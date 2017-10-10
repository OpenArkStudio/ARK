file(GLOB GameLogicPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFGameLogicPlugin SHARED ${GameLogicPlugin_SRC})

set_target_properties(AFGameLogicPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFGameLogicPlugin_d")
set_target_properties(AFGameLogicPlugin PROPERTIES
    FOLDER "Server/GameServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

LINK_SDK_MACRO("AFGameLogicPlugin")