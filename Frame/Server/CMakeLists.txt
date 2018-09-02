#Define server plugin building macro
MACRO(BUILD_PLUGIN_MACRO project_name)
    aux_source_directory(. PLUGIN_SRC)

    if(UNIX)
        #Set rpath
        SET(CMAKE_INSTALL_RPATH "./" "../lib/")
        SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif()

    add_library(${project_name} SHARED ${PLUGIN_SRC})

    add_dependencies(${project_name} AFKernelPlugin AFUtilityPlugin AFProto AFNet AFCore)
    if(UNIX)
        add_definitions(
            -DLIBPROTOC_EXPORTS
            -DPROTOBUF_USE_DLLS)

        target_link_libraries(${project_name} 
            debug :AFNet_d.a
            debug :AFProto_d.so
            debug :AFCore_d.a
            debug protobuf
            debug brynet.a
            debug pthread
            debug dl
            
            optimized :AFNet.a
            optimized :AFProto.so
			optimized :AFCore.a
            optimized protobuf
            optimized brynet.a
            optimized pthread
            optimized dl)
    else(UNIX)
        add_definitions(
            -D_USRDLL
            -D_LIB
            -DLIBPROTOC_EXPORTS
            -DPROTOBUF_USE_DLLS)

        target_link_libraries(${project_name} 
            debug brynet.lib
            debug AFCore_d.lib
            debug AFNet_d.lib
            debug AFConfigPlugin_d.lib
            debug AFKernelPlugin_d.lib
            debug AFUtilityPlugin_d.lib
            debug AFProto_d.lib
            
            optimized brynet.lib
            optimized AFCore.lib
            optimized AFNet.lib
            optimized AFConfigPlugin_d.lib
            optimized AFKernelPlugin.lib
            optimized AFUtilityPlugin.lib
            optimized AFProto.lib)
    endif(UNIX)

    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}_d")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "Server"
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
ENDMACRO(BUILD_PLUGIN_MACRO project_name)

###################################################################################################
add_subdirectory(BusPlugin)
add_subdirectory(NetPlugin)
add_subdirectory(GameLogicPlugin)
add_subdirectory(LoginLogicPlugin)
add_subdirectory(ProxyLogicPlugin)
add_subdirectory(WorldLogicPlugin)
add_subdirectory(MasterLogicPlugin)
