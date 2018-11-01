#Define sdk plugin building macro
macro(BUILD_SDK_PLUGIN_MACRO project_name)
    if(UNIX)
        #Set rpath
        SET(CMAKE_INSTALL_RPATH "./" "../lib/")
        SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif()
    
    aux_source_directory(. SDK_SRC)
    add_library(${project_name} SHARED ${SDK_SRC})
    add_dependencies(${project_name} AFCore)
    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}_d")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "SDK"
        ARCHIVE_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    )
    target_link_libraries(${project_name} AFCore)
endmacro(BUILD_SDK_PLUGIN_MACRO project_name)

#macro(DEP_EXTRA_LIBS LIBRARY_NAMES_LIST)
#    set(NEW_LIBRARY_NAMES_LIST ${LIBRARY_NAMES_LIST} ${ARGN} ) # Merge them together
#    message( "inside ${_LIBRARY_NAMES_LIST}" )
#endmacro()

macro(BUILD_SDK_PLUGIN_MACRO_EX project_name dep_libs)
    if(UNIX)
        #Set rpath
        SET(CMAKE_INSTALL_RPATH "./" "../lib/")
        SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif()
    
    aux_source_directory(. SDK_SRC)
    add_library(${project_name} SHARED ${SDK_SRC})
    add_dependencies(${project_name} AFCore)
    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}_d")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "SDK"
        ARCHIVE_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    )
    set(new_dep_libs ${dep_libs} ${ARGN})
    target_link_libraries(${project_name} AFCore ${new_dep_libs})
endmacro(BUILD_SDK_PLUGIN_MACRO_EX)

###################################################################################################

add_subdirectory(Core)
add_subdirectory(KernelPlugin)
add_subdirectory(BusPlugin)
add_subdirectory(NetPlugin)
add_subdirectory(ProtoPlugin)
add_subdirectory(UtilityPlugin)
