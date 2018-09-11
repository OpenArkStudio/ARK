#Define server plugin building macro
macro(BUILD_PLUGIN_MACRO project_name)
    aux_source_directory(. PLUGIN_SRC)

    if(UNIX)
        #Set rpath
        set(CMAKE_INSTALL_RPATH "./lib/" "../lib/")
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif()

    add_library(${project_name} SHARED ${PLUGIN_SRC})

    add_dependencies(${project_name} AFKernelPlugin AFUtilityPlugin AFProtoPlugin AFCore)
    if(UNIX)
        add_definitions(
            -DLIBPROTOC_EXPORTS
            -DPROTOBUF_USE_DLLS)

        target_link_libraries(${project_name} 
            debug :AFProtoPlugin_d.so
            debug :AFCore_d.a
            debug protobuf
            debug brynet.a
            debug pthread
            debug dl
            
            optimized :AFProtoPlugin.so
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
            debug AFConfigPlugin_d.lib
            debug AFKernelPlugin_d.lib
            debug AFUtilityPlugin_d.lib
            debug AFProtoPlugin_d.lib
            
            optimized brynet.lib
            optimized AFCore.lib
            optimized AFConfigPlugin.lib
            optimized AFKernelPlugin.lib
            optimized AFUtilityPlugin.lib
            optimized AFProtoPlugin.lib)
    endif(UNIX)

    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}_d")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "Server"
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
endmacro(BUILD_PLUGIN_MACRO project_name)

###################################################################################################
add_subdirectory(GameLogicPlugin)
add_subdirectory(LoginLogicPlugin)
add_subdirectory(ProxyLogicPlugin)
add_subdirectory(WorldLogicPlugin)
add_subdirectory(MasterLogicPlugin)
