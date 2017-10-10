file(GLOB WorldNetServerPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFWorldNetServerPlugin SHARED ${WorldNetServerPlugin_SRC})

set_target_properties(AFWorldNetServerPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFWorldNetServerPlugin_d")
set_target_properties(AFWorldNetServerPlugin PROPERTIES
    FOLDER "Server/WorldServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

LINK_SDK_MACRO("AFWorldNetServerPlugin")