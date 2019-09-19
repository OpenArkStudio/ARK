if(WIN32)
    BUILD_PLUGIN_MACRO(AFKernelPlugin 
        debug AFProto_d.lib
        debug libprotobufd.lib

        optimized AFProto.lib
        optimized libprotobuf.lib)
else(UNIX)
    BUILD_PLUGIN_MACRO(AFKernelPlugin 
        debug :AFProto_d.a
        debug protobuf

        optimized :AFProto.a
        optimized protobuf)
endif()
