#Define sdk plugin building macro
MACRO(BUILD_SDK_PLUGIN_MACRO project_name)
    if(UNIX)
        #Set rpath
        SET(CMAKE_INSTALL_RPATH "../lib/" "./")
        SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif()
    
    aux_source_directory(. SDK_SRC)
    add_library(${project_name} SHARED ${SDK_SRC})
    add_dependencies(${project_name} AFCore)
    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}_d")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "SDK"
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    )
    target_link_libraries(${project_name} AFCore)
ENDMACRO(BUILD_SDK_PLUGIN_MACRO project_name)

###################################################################################################

add_subdirectory(Core)
add_subdirectory(KernelPlugin)
add_subdirectory(ConfigPlugin)
add_subdirectory(Net)
add_subdirectory(PluginLoader)
add_subdirectory(Proto)
add_subdirectory(UtilityPlugin)