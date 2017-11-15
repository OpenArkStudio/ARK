file(GLOB MasterLogicPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFMasterLogicPlugin SHARED ${MasterLogicPlugin_SRC})

set_target_properties(AFMasterLogicPlugin PREFIX "")
set_target_properties(AFMasterLogicPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFMasterLogicPlugin_d")
set_target_properties(AFMasterLogicPlugin PROPERTIES
    FOLDER "Server/MasterServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

LINK_SDK_MACRO("AFMasterLogicPlugin")