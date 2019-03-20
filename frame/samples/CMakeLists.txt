#Define server plugin building macro
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
            debug brynet.a
            debug pthread
            debug dl
            
            optimized :AFProto.a
            optimized protobuf
            optimized brynet.a
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
    set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}_d")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "samples"
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})
endmacro(BUILD_PLUGIN_MACRO project_name)

###################################################################################################
add_subdirectory(sample1)
add_subdirectory(sample2)
add_subdirectory(sample3)
