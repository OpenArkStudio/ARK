file(GLOB LoginLogicPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFLoginLogicPlugin SHARED ${LoginLogicPlugin_SRC})

set_target_properties(AFLoginLogicPlugin PREFIX "")
set_target_properties(AFLoginLogicPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFLoginLogicPlugin_d")
set_target_properties(AFLoginLogicPlugin PROPERTIES
    FOLDER "Server/LoginServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

LINK_SDK_MACRO("AFLoginLogicPlugin")