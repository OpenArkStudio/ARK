set(project_name AFProto)

set(ark_proto_generated "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(GLOB PROTO_DEFINITION CONFIGURE_DEPENDS proto/*.proto)
MY_PROTOBUF_GENERATE_CPP("${ark_proto_generated}/ark/proto" PROTO_SRC PROTO_HEADER ${PROTO_DEFINITION})
add_library(${project_name} STATIC ${PROTO_HEADER} ${PROTO_SRC})
add_library(ark::proto ALIAS ${project_name})
target_include_directories(${project_name} PUBLIC
    "$<BUILD_INTERFACE:${ark_proto_generated}>")
target_compile_definitions(${project_name} PUBLIC ARK_HAS_PROTOBUF=1)
target_link_libraries(${project_name} PUBLIC ${PROTOBUF_LIBRARIES})
set_target_properties(${project_name} PROPERTIES
    PREFIX ""
    FOLDER "proto"
    ARCHIVE_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}")
