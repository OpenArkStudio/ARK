set(project_name AFProto)

if (UNIX AND (NOT APPLE))
    #Set rpath
    set(CMAKE_INSTALL_RPATH ".")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif (UNIX AND (NOT APPLE))

file(GLOB PROTO_DEFINATION proto/*.proto)
MY_PROTOBUF_GENERATE_CPP(${CMAKE_CURRENT_SOURCE_DIR}/cpp PROTO_SRC PROTO_HEADER ${PROTO_DEFINATION})
add_library(${project_name} STATIC ${PROTO_HEADER} ${PROTO_SRC})

set_target_properties(${project_name} PROPERTIES PREFIX "")
set_target_properties(${project_name} PROPERTIES
        FOLDER "proto"
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})

target_link_libraries(${project_name} ${PROTOBUF_LIBRARIES})