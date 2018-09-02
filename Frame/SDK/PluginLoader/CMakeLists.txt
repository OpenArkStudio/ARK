file(GLOB PluginLoader_SRC *.h *.hpp *.cpp *.cc *.c)

add_definitions(
    -D_CONSOLE
    -DAF_DYNAMIC_PLUGIN)
    
if(UNIX)
    #Set rpath
    SET(CMAKE_INSTALL_RPATH "../bin" "../lib/")
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

add_dependencies(PluginLoader AFNet AFCore)
add_executable(PluginLoader ${PluginLoader_SRC})

if(UNIX)
    target_link_libraries(PluginLoader AFCore AFNet pthread dl)
else(UNIX)
    target_link_libraries(PluginLoader
        debug AFCore_d.lib
        debug AFNet_d.lib
        
        optimized AFCore.lib
        optimized AFNet.lib)
endif(UNIX)

set_target_properties(PluginLoader PROPERTIES OUTPUT_NAME_DEBUG "PluginLoader_d")
set_target_properties(PluginLoader PROPERTIES
    FOLDER "PluginLoader"
    ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})