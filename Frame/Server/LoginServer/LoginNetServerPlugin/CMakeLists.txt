file(GLOB LoginNetServerPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFLoginNetServerPlugin SHARED ${LoginNetServerPlugin_SRC})

set_target_properties(AFLoginNetServerPlugin PREFIX "")
set_target_properties(AFLoginNetServerPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFLoginNetServerPlugin_d")
set_target_properties(AFLoginNetServerPlugin PROPERTIES
    FOLDER "Server/LoginServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

LINK_SDK_MACRO("AFLoginNetServerPlugin")