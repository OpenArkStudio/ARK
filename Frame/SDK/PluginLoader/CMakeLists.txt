file(GLOB PluginLoader_SRC *.h *.hpp *.cpp *.cc *.c)

#add_library(PluginLoader SHARED ${PluginLoader_SRC})
add_executable(PluginLoader ${PluginLoader_SRC})

set_target_properties(PluginLoader PROPERTIES OUTPUT_NAME_DEBUG "PluginLoader_d")
set_target_properties(PluginLoader PROPERTIES
    FOLDER "PluginLoader"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR})

target_link_libraries(PluginLoader)

add_definitions(
    -D_CONSOLE
    -DAF_DYNAMIC_PLUGIN
    -DELPP_NO_DEFAULT_LOG_FILE)