#Define server plugin building macro
function(BUILD_PLUGIN_MACRO project_name)
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
            debug pthread
            debug dl
            
            optimized :AFProto.a
            optimized protobuf
            optimized pthread
            optimized dl)
    else(UNIX)
        add_definitions(-D_USRDLL)

        target_link_libraries(${project_name} 
            debug AFProto_d.lib
            debug libprotobufd.lib

            optimized AFProto.lib
            optimized libprotobuf.lib)
    endif(UNIX)

    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "samples")
endfunction(BUILD_PLUGIN_MACRO project_name)

###################################################################################################
add_subdirectory(sample1)
add_subdirectory(sample2)
add_subdirectory(sample3)
