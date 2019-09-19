if(WIN32)
    BUILD_PLUGIN_MACRO(AFLogPlugin
        debug AFProto_d.lib
        debug libprotobufd.lib
        
        optimized AFProto.lib
        optimized libprotobuf.lib)
else(UNIX)
    BUILD_PLUGIN_MACRO(AFLogPlugin protobuf)
endif()
