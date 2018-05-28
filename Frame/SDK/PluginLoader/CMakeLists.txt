file(GLOB PluginLoader_SRC *.h *.hpp *.cpp *.cc *.c)

add_definitions(-DELPP_NO_DEFAULT_LOG_FILE)
add_executable(PluginLoader ${PluginLoader_SRC})

if(UNIX)
    target_link_libraries(PluginLoader AFCore AFNet)
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
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

if(UNIX)
target_link_libraries(PluginLoader dl pthread)
else()
target_link_libraries(PluginLoader)
endif()

add_definitions(
    -D_CONSOLE
    -DAF_DYNAMIC_PLUGIN
    -DELPP_NO_DEFAULT_LOG_FILE)