file(GLOB AFProto_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFProto SHARED ${AFProto_SRC})

set_target_properties(AFProto PROPERTIES OUTPUT_NAME_DEBUG "AFProto_d")
set_target_properties(AFProto PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

if(UNIX)
    target_link_libraries(AFProto libprotobuf)
else()
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        target_link_libraries(AFProto libprotobufd)
    else()
        target_link_libraries(AFProto libprotobuf)
    endif()

    add_definitions(
        -D_USRDLL
        -D_LIB
        -DLIBPROTOC_EXPORTS
        -DPROTOBUF_USE_DLLS
        -D_SCL_SECURE_NO_WARNINGS
        )
endif()