set(project_name "AFNetPlugin")

if(UNIX)
    #Set rpath
    SET(CMAKE_INSTALL_RPATH "./lib/" "../lib/")
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

file(GLOB_RECURSE SDK_SRC *.h *.hpp *.cpp *.cc *.c)
add_library(${project_name} SHARED ${SDK_SRC})
add_dependencies(${project_name} AFProto)
set_target_properties(${project_name} PROPERTIES PREFIX "")
set_target_properties(${project_name} PROPERTIES
        FOLDER "plugins"
        ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})

if(UNIX)
    target_link_libraries(${project_name} brynet protobuf)
else(WIN32)
    target_link_libraries(${project_name} AFProto 
    debug brynetd.lib
    debug libprotobufd.lib

    optimized brynet.lib
    optimized libprotobuf.lib)
endif()