file(GLOB WorldLogicPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFWorldLogicPlugin SHARED ${WorldLogicPlugin_SRC})

set_target_properties(AFWorldLogicPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFWorldLogicPlugin_d")
set_target_properties(AFWorldLogicPlugin PROPERTIES
    FOLDER "Server/WorldServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

LINK_SDK_MACRO("AFWorldLogicPlugin")