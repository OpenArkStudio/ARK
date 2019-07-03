#Define sdk plugin building macro
macro(BUILD_SDK_PLUGIN_MACRO project_name)
    if(UNIX)
        #Set rpath
        SET(CMAKE_INSTALL_RPATH "./lib/" "../lib/")
        SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif(UNIX)
    
    aux_source_directory(. SDK_SRC)
    add_library(${project_name} SHARED ${SDK_SRC})
    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "sdk"
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
    )
    target_link_libraries(${project_name})
endmacro(BUILD_SDK_PLUGIN_MACRO)

###################################################################################################

#macro(BUILD_PLUGIN_MACRO project_name)
#    aux_source_directory(. PLUGIN_SRC)
#
#    if(UNIX)
#        #Set rpath
#        set(CMAKE_INSTALL_RPATH "./lib/" "../lib/")
#        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
#    endif()
#
#    add_library(${project_name} SHARED ${PLUGIN_SRC})
#    #add_dependencies(${project_name})
#
#    if(UNIX)
#        target_link_libraries(${project_name} 
#            debug protobuf
#            debug brynet
#            debug pthread
#            debug dl
#            
#            optimized protobuf
#            optimized brynet
#            optimized pthread
#            optimized dl)
#    else(WIN32)
#        add_definitions(-D_USRDLL)
#
#        target_link_libraries(${project_name} 
#            debug brynetd.lib
#            
#            optimized brynet.lib)
#    endif()
#
#    set_target_properties(${project_name} PROPERTIES PREFIX "")    
#    set_target_properties(${project_name} PROPERTIES
#        FOLDER "plugins"
#        ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
#        RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
#        LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})
#endmacro(BUILD_PLUGIN_MACRO project_name)

macro(BUILD_PLUGIN_MACRO project_name)
    aux_source_directory(. PLUGIN_SRC)

    if(UNIX)
        #Set rpath
        set(CMAKE_INSTALL_RPATH "./lib/" "../lib/")
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif()

    add_library(${project_name} SHARED ${PLUGIN_SRC})

    add_dependencies(${project_name} AFProto)

    if(UNIX)
        target_link_libraries(${project_name} 
            debug :AFProto_d.a
            debug protobuf
            debug brynet
            debug pthread
            debug dl
            
            optimized :AFProto.a
            optimized protobuf
            optimized brynet
            optimized pthread
            optimized dl)
    else(UNIX)
        add_definitions(-D_USRDLL)

        target_link_libraries(${project_name} 
            debug brynetd.lib
            debug AFProto_d.lib
            
            optimized brynet.lib
            optimized AFProto.lib)
    endif(UNIX)

    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "server"
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})
endmacro(BUILD_PLUGIN_MACRO project_name)

#---------------------------------------------------------------------------------------
# sub projects
#---------------------------------------------------------------------------------------
# core plugins
add_subdirectory(kernel)
add_subdirectory(bus)
add_subdirectory(net)
add_subdirectory(utility)

# server plugins
add_subdirectory(game)
add_subdirectory(login)
add_subdirectory(proxy)
add_subdirectory(world)
add_subdirectory(master)
add_subdirectory(dir)
add_subdirectory(router)

# 3rd plugins
