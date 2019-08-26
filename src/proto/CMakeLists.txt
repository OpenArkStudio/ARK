if(UNIX)
    #Set rpath
    SET(CMAKE_INSTALL_RPATH ".")
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

set(project_name AFProto)
file(GLOB_RECURSE SDK_SRC *.h *.hpp *.cpp *.cc *.c)
add_library(${project_name} STATIC ${SDK_SRC})
set_target_properties(${project_name} PROPERTIES PREFIX "")
set_target_properties(${project_name} PROPERTIES
    FOLDER "proto"
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})
if(UNIX)
    target_link_libraries(${project_name})
else(UNIX)
    target_link_libraries(${project_name} 
        debug libprotobufd
        optimized libprotobuf)
endif(UNIX)
