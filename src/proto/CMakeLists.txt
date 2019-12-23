set(project_name AFProto)

if(UNIX AND (NOT APPLE))
    #Set rpath
    set(CMAKE_INSTALL_RPATH ".")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif(UNIX AND (NOT APPLE))

file(GLOB PROTO_FILES "${ROOT_DIR}/src/proto/proto/*.proto")
foreach(PROTO_FILE in ${PROTO_FILES})
    string(REGEX REPLACE "[.]proto$" ".pb.cc" OUTPUT_SOURCE ${PROTO_FILE})
    string(REGEX REPLACE "proto/proto" "proto/cpp" OUTPUT_SOURCE ${OUTPUT_SOURCE})
    list(APPEND OUTPUT_SOURCES ${OUTPUT_SOURCE})
endforeach()

add_custom_command(OUTPUT ${OUTPUT_SOURCES}
    COMMAND ${CMAKE_COMMAND} -E copy ${BIN_OUTPUT_DIR}/*protoc* ${ROOT_DIR}/src/proto/proto-gen/
    COMMAND cd ${ROOT_DIR}/src/proto/ && bash gen-proto.sh
    DEPENDS ${PROTO_FILES}
    COMMENT "generate proto files in CMake...")

add_library(${project_name} STATIC ${OUTPUT_SOURCES})
add_dependencies(${project_name} protoc libprotobuf libprotobuf-lite libprotoc)

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
    target_link_libraries(${project_name} ${PROTO_PRE_EVENT})
endif()  