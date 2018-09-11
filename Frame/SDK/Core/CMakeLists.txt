set(project_name AFCore)
file(GLOB SDK_SRC *.h *.hpp *.cpp *.cc *.c Common/*.h Common/*.hpp Common/*.cpp)
add_library(${project_name} STATIC ${SDK_SRC})

set_target_properties(${project_name} PROPERTIES PREFIX "")
set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}_d")
set_target_properties(${project_name} PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR})

target_link_libraries(${project_name})