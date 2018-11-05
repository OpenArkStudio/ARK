set(project_name "AFBusPlugin")

if(UNIX)
    #Set rpath
    SET(CMAKE_INSTALL_RPATH "./lib/" "../lib/")
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif(UNIX)

aux_source_directory(. SDK_SRC)
add_library(${project_name} SHARED ${SDK_SRC})
add_dependencies(${project_name} AFProto AFCore)
set_target_properties(${project_name} PROPERTIES PREFIX "")
set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}_d")
set_target_properties(${project_name} PROPERTIES
        FOLDER "SDK"
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})

if(UNIX)
    target_link_libraries(${project_name} AFProto AFCore brynet.a protobuf)
else(UNIX)
    target_link_libraries(${project_name} AFProto AFCore 
    debug brynetd.lib
    debug libprotobufd.lib

    optimized brynet.lib
    optimized libprotobuf.lib)
endif(UNIX)
