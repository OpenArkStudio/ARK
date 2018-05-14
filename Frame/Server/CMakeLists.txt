if(UNIX)
    add_definitions(
        -DELPP_NO_DEFAULT_LOG_FILE)
else()
    add_definitions(
        -D_USRDLL
        -D_LIB
        -DELPP_NO_DEFAULT_LOG_FILE
        -DPROTOBUF_USE_DLLS)
endif()

if(unix)
    include_directories(${ROOT_DIR}/Dep/glog/src/)    
else()
    include_directories(${ROOT_DIR}/Dep/glog/src/windows)
endif()


########################################################################
macro(LINK_SDK_MACRO project_name)
add_dependencies(${project_name} AFCore AFNet AFKernelPlugin AFUtilityPlugin AFProto)
if(UNIX)
    target_link_libraries(${project_name} AFKernelPlugin AFUtilityPlugin AFCore AFNet AFProto protobuf brynet)
else(UNIX)
    target_link_libraries(${project_name} 
        debug brynet
        debug AFCore_d.lib
        debug AFNet_d.lib
        debug AFKernelPlugin_d.lib
        debug AFUtilityPlugin_d.lib
        debug AFProto_d.lib 
		debug brynet.lib 
        
        optimized brynet
        optimized AFCore.lib
        optimized AFNet.lib
        optimized AFKernelPlugin.lib
        optimized AFUtilityPlugin.lib
        optimized AFProto.lib)
endif(UNIX)
endmacro(LINK_SDK_MACRO project_name)
########################################################################
add_subdirectory(GameServer/GameLogicPlugin)
add_subdirectory(GameServer/GameNetClientPlugin)
add_subdirectory(GameServer/GameNetServerPlugin)

add_subdirectory(LoginServer/LoginLogicPlugin)
add_subdirectory(LoginServer/LoginNetClientPlugin)
add_subdirectory(LoginServer/LoginNetServerPlugin)

add_subdirectory(MasterServer/MasterLogicPlugin)
add_subdirectory(MasterServer/MasterNetServerPlugin)

add_subdirectory(ProxyServer/ProxyLogicPlugin)
add_subdirectory(ProxyServer/ProxyNetClientPlugin)
add_subdirectory(ProxyServer/ProxyNetServerPlugin)

add_subdirectory(WorldServer/WorldLogicPlugin)
add_subdirectory(WorldServer/WorldNetClientPlugin)
add_subdirectory(WorldServer/WorldNetServerPlugin)