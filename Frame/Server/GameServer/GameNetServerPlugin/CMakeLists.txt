file(GLOB GameNetServerPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFGameNetServerPlugin SHARED ${GameNetServerPlugin_SRC})

set_target_properties(AFGameNetServerPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFGameNetServerPlugin_d")
set_target_properties(AFGameNetServerPlugin PROPERTIES
    FOLDER "Server/GameServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

LINK_SDK_MACRO("AFGameNetServerPlugin")