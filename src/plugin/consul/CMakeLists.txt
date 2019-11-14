if(WIN32)
    BUILD_PLUGIN_MACRO(AFConsulPlugin 
        debug brynetd.lib
        debug AFProto_d.lib
        debug libprotobufd.lib

        optimized brynet.lib
        optimized AFProto.lib
        optimized libprotobuf.lib)
elseif(APPLE)
    BUILD_PLUGIN_MACRO(AFConsulPlugin 
        debug ${BIN_OUTPUT_DIR}/AFProto_d.a
        optimized ${BIN_OUTPUT_DIR}/AFProto.a
        brynet
        protobuf)
else(UNIX)
    BUILD_PLUGIN_MACRO(AFConsulPlugin 
        debug :AFProto_d.a
        optimized :AFProto.a
        brynet
        protobuf)
endif()
