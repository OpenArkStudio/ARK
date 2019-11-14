if(WIN32)
    BUILD_PLUGIN_MACRO(AFBusPlugin
        debug AFProto_d.lib
        debug brynetd.lib
        debug libprotobufd.lib
        
        optimized AFProto.lib
        optimized brynet.lib
        optimized libprotobuf.lib)
elseif(APPLE)
    BUILD_PLUGIN_MACRO(AFBusPlugin 
        debug ${BIN_OUTPUT_DIR}/AFProto_d.a
        optimized ${BIN_OUTPUT_DIR}/AFProto.a
        brynet
        protobuf)
else(UNIX)
    BUILD_PLUGIN_MACRO(AFBusPlugin 
        debug :AFProto_d.a
        optimized :AFProto.a
        brynet
        protobuf)
endif()
