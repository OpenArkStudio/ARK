file(GLOB ProxyNetClientPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFProxyNetClientPlugin SHARED ${ProxyNetClientPlugin_SRC})

set_target_properties(AFProxyNetClientPlugin PROPERTIES PREFIX "")
set_target_properties(AFProxyNetClientPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFProxyNetClientPlugin_d")
set_target_properties(AFProxyNetClientPlugin PROPERTIES
    FOLDER "Server/ProxyServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

LINK_SDK_MACRO("AFProxyNetClientPlugin")