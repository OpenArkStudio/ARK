file(GLOB ProxyNetServerPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFProxyNetServerPlugin SHARED ${ProxyNetServerPlugin_SRC})

set_target_properties(AFProxyNetServerPlugin PREFIX "")
set_target_properties(AFProxyNetServerPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFProxyNetServerPlugin_d")
set_target_properties(AFProxyNetServerPlugin PROPERTIES
    FOLDER "Server/ProxyServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

LINK_SDK_MACRO("AFProxyNetServerPlugin")