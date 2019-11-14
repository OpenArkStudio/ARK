if(WIN32)
    BUILD_PLUGIN_MACRO(AFKernelPlugin 
        debug AFProto_d.lib
        debug libprotobufd.lib

        optimized AFProto.lib
        optimized libprotobuf.lib)
elseif(APPLE)
    BUILD_PLUGIN_MACRO(AFKernelPlugin 
        debug ${BIN_OUTPUT_DIR}/AFProto_d.a
        optimized ${BIN_OUTPUT_DIR}/AFProto.a
        protobuf)
else(UNIX)
    BUILD_PLUGIN_MACRO(AFKernelPlugin 
        debug :AFProto_d.a
        optimized :AFProto.a
        protobuf)
endif()
