function(BUILD_PLUGIN_MACRO project_name)
    if(UNIX AND (NOT APPLE))
        #Set rpath
        set(CMAKE_INSTALL_RPATH ".")
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif(UNIX AND (NOT APPLE))
    
    file(GLOB_RECURSE PLUGIN_SRC *.h *.hpp *.cpp *.cc *.c)
    add_library(${project_name} SHARED ${PLUGIN_SRC})
    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "plugins"
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})

    add_dependencies(${project_name} AFProto)
    target_link_libraries(${project_name} ${ARGN})
endfunction(BUILD_PLUGIN_MACRO)

#---------------------------------------------------------------------------------------
# sub projects
#---------------------------------------------------------------------------------------
# Main plugins
add_subdirectory(kernel)
add_subdirectory(bus)
add_subdirectory(net)
add_subdirectory(utility)
add_subdirectory(log)
add_subdirectory(consul)

