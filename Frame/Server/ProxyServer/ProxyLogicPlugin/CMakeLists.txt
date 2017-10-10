file(GLOB ProxyLogicPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFProxyLogicPlugin SHARED ${ProxyLogicPlugin_SRC})

set_target_properties(AFProxyLogicPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFProxyLogicPlugin_d")
set_target_properties(AFProxyLogicPlugin PROPERTIES
    FOLDER "Server/ProxyServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

LINK_SDK_MACRO("AFProxyLogicPlugin")