if(WIN32)
    BUILD_PLUGIN_MACRO(AFBusPlugin
        debug AFProto_d.lib
        debug brynetd.lib
        debug libprotobufd.lib
        
        optimized AFProto.lib
        optimized brynet.lib
        optimized libprotobuf.lib)
else(UNIX)
    BUILD_PLUGIN_MACRO(AFBusPlugin 
        debug :AFProto_d.a
        debug protobuf
        debug brynet

        optimized :AFProto.a
        optimized protobuf
        optimized brynet)
endif()
