file(GLOB LoginNetClientPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFLoginNetClientPlugin SHARED ${LoginNetClientPlugin_SRC})

set_target_properties(AFLoginNetClientPlugin PROPERTIES PREFIX "")
set_target_properties(AFLoginNetClientPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFLoginNetClientPlugin_d")
set_target_properties(AFLoginNetClientPlugin PROPERTIES
    FOLDER "Server/LoginServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

LINK_SDK_MACRO("AFLoginNetClientPlugin")