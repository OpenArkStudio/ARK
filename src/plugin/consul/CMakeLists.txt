if(WIN32)
    BUILD_PLUGIN_MACRO(AFConsulPlugin
        debug AFProto_d.lib
        optimized AFProto.lib
        ${PROTOBUF_LIBRARIES})
elseif(APPLE)
    BUILD_PLUGIN_MACRO(AFConsulPlugin 
        debug ${BIN_OUTPUT_DIR}/AFProto_d.a
        optimized ${BIN_OUTPUT_DIR}/AFProto.a
        ${PROTOBUF_LIBRARIES})
else(UNIX)
    BUILD_PLUGIN_MACRO(AFConsulPlugin 
        debug :AFProto_d.a
        optimized :AFProto.a
        ${PROTOBUF_LIBRARIES})
endif()
