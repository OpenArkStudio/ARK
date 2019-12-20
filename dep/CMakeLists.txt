

message(STATUS "Add dep projects...")

# Building protobuffer
if(WIN32)
    option(protobuf_BUILD_SHARED_LIBS "Disable protobuf shared lib" OFF)
    option(protobuf_MSVC_STATIC_RUNTIME "Disable MSVC static runtime" OFF)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
else(UNIX)
    option(protobuf_BUILD_SHARED_LIBS "Disable protobuf shared lib" ON)
endif()
option(protobuf_BUILD_TESTS "Disable protobuf tests" OFF)
add_subdirectory(protobuf/cmake)

# set protobuf folder
set(THIRD_PARTY_TARGET
    #dep_postevent
    libprotobuf
    libprotobuf-lite
    libprotoc
    protoc)

foreach(target_index ${THIRD_PARTY_TARGET})
    set_target_properties(${target_index} PROPERTIES
        FOLDER "dep"
        ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
endforeach(target_index)

# need add a custom target
if(WIN32)
    add_custom_target(dep_postevent ALL
        COMMAND echo "this is a custom target for pre-build dependencies"
        COMMAND ${CMAKE_COMMAND} -E copy ${BIN_OUTPUT_DIR}/protoc.exe ${ROOT_DIR}/src/proto/proto-gen/
        COMMAND cd "${ROOT_DIR}/src/proto/"
        COMMAND ./gen-proto.bat
        COMMAND cd "${ROOT_DIR}/build/"
        DEPENDS libprotobuf libprotobuf-lite libprotoc protoc)
else(UNIX)
    add_custom_target(dep_postevent ALL
        COMMAND echo "this is a custom target for pre-build dependencies"
        COMMAND ${CMAKE_COMMAND} -E copy ${BIN_OUTPUT_DIR}/*protoc* ${ROOT_DIR}/src/proto/proto-gen/
        COMMAND cd "${ROOT_DIR}/src/proto/"
        COMMAND ./gen-proto.bat
        COMMAND cd "${ROOT_DIR}/build/"
        DEPENDS libprotobuf libprotobuf-lite libprotoc protoc)
endif()

set_target_properties(dep_postevent PROPERTIES
    FOLDER "dep"
    ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})