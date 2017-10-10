file(GLOB MasterNetServerPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFMasterNetServerPlugin SHARED ${MasterNetServerPlugin_SRC})

set_target_properties(AFMasterNetServerPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFMasterNetServerPlugin_d")
set_target_properties(AFMasterNetServerPlugin PROPERTIES
    FOLDER "Server/MasterServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

LINK_SDK_MACRO("AFMasterNetServerPlugin")