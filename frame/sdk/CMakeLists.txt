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
    set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}_d")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "sdk"
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
    )
    target_link_libraries(${project_name})
endmacro(BUILD_SDK_PLUGIN_MACRO)

###################################################################################################

add_subdirectory(kernel)
add_subdirectory(bus)
add_subdirectory(net)
add_subdirectory(utility)
