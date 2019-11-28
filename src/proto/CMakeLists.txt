set(project_name AFProto)
file(GLOB_RECURSE PROTO_SRC *.h *.hpp *.cpp *.cc *.c)

if(UNIX AND (NOT APPLE))
    #Set rpath
    set(CMAKE_INSTALL_RPATH ".")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif(UNIX AND (NOT APPLE))

add_library(${project_name} STATIC ${PROTO_SRC})

set_target_properties(${project_name} PROPERTIES PREFIX "")
set_target_properties(${project_name} PROPERTIES
    FOLDER "proto"
	ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})

if(WIN32)
    target_link_libraries(${project_name} 
        debug libprotobufd
        optimized libprotobuf)
else(UNIX)
    target_link_libraries(${project_name})
endif()  