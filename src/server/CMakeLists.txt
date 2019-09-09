macro(BUILD_SERVER_MACRO project_name)
    if(UNIX)
        #Set rpath
        set(CMAKE_INSTALL_RPATH ".")
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif()

    file(GLOB_RECURSE PLUGIN_SRC *.h *.hpp *.cpp *.cc *.c)
    add_library(${project_name} SHARED ${PLUGIN_SRC})

    add_dependencies(${project_name} AFProto)

    if(UNIX)
        target_link_libraries(${project_name} 
            debug :AFProto_d.a
            debug protobuf
            #debug brynet
            #debug pthread
            #debug dl
            
            optimized :AFProto.a
            optimized protobuf
            #optimized brynet
            #optimized pthread
            #optimized dl
            )
    else(WIN32)
        add_definitions(-D_USRDLL)

        target_link_libraries(${project_name} 
            debug brynetd.lib
            debug AFProto_d.lib
            
            #optimized brynet.lib
            optimized AFProto.lib
            )
    endif(UNIX)

    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "server"
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})
endmacro(BUILD_SERVER_MACRO project_name)

#---------------------------------------------------------------------------------------
# sub projects
#---------------------------------------------------------------------------------------
# server plugins
add_subdirectory(game)
add_subdirectory(login)
add_subdirectory(proxy)
add_subdirectory(world)
add_subdirectory(master)
add_subdirectory(dir)
add_subdirectory(router)
