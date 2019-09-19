if(WIN32)
    BUILD_PLUGIN_MACRO(AFConsulPlugin 
        debug brynetd.lib
        debug AFProto_d.lib
        debug libprotobufd.lib

        optimized brynet.lib
        optimized AFProto.lib
        optimized libprotobuf.lib)
else(UNIX)
    BUILD_PLUGIN_MACRO(AFConsulPlugin 
        debug brynet    
        debug :AFProto_d.a
        debug protobuf

        optimized brynet
        optimized :AFProto.a
        optimized protobuf)
endif()
