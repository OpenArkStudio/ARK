#Define sdk plugin building macro
macro(BUILD_PLUGIN_MACRO project_name)
    if(UNIX)
        #Set rpath
        SET(CMAKE_INSTALL_RPATH "./lib/" "../lib/")
        SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif(UNIX)
    
    file(GLOB_RECURSE PLUGIN_SRC *.h *.hpp *.cpp *.cc *.c)
    add_library(${project_name} SHARED ${PLUGIN_SRC})
    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "plugins"
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
    )
    target_link_libraries(${project_name})
endmacro(BUILD_PLUGIN_MACRO)

macro(BUILD_PLUGIN_MACRO_EX project_name extra_lib)
    if(UNIX)
        #Set rpath
        SET(CMAKE_INSTALL_RPATH "./")
        SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif(UNIX)
    
    file(GLOB_RECURSE PLUGIN_SRC *.h *.hpp *.cpp *.cc *.c)
    add_library(${project_name} SHARED ${PLUGIN_SRC})
    set_target_properties(${project_name} PROPERTIES PREFIX "")
    set_target_properties(${project_name} PROPERTIES
        FOLDER "plugins"
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})
    target_link_libraries(${project_name} ${extra_lib})
endmacro(BUILD_PLUGIN_MACRO_EX)

#---------------------------------------------------------------------------------------
# sub projects
#---------------------------------------------------------------------------------------
# 3rd plugins
add_subdirectory(kernel)
add_subdirectory(bus)
add_subdirectory(net)
add_subdirectory(utility)
add_subdirectory(log)
#add_subdirectory(consul)

