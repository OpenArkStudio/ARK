function(BUILD_SERVER_MACRO project_name)
    if(UNIX AND (NOT APPLE))
        #Set rpath
        set(CMAKE_INSTALL_RPATH ".")
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif(UNIX AND (NOT APPLE))

    file(GLOB_RECURSE PLUGIN_SRC *.h *.hpp *.cpp *.cc *.c)
    add_library(${project_name} SHARED ${PLUGIN_SRC})

    add_dependencies(${project_name} AFProto)

    if(WIN32)
        add_definitions(-D_USRDLL)

        target_link_libraries(${project_name} 
            debug AFProto_d.lib
            debug libprotobufd.lib
            
            optimized AFProto.lib
            optimized libprotobuf.lib)
    elseif(APPLE)
        target_link_libraries(${project_name} 
            debug ${BIN_OUTPUT_DIR}/AFProto_d.a
            optimized ${BIN_OUTPUT_DIR}/AFProto.a
            protobuf)
    else()
        target_link_libraries(${project_name} 
            debug :AFProto_d.a
            optimized :AFProto.a
            protobuf)
    endif()

    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "server"
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
endfunction(BUILD_SERVER_MACRO)

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
