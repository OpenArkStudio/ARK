file(GLOB GameLogicPlugin_SRC *.h *.hpp *.cpp *.cc *.c)

add_library(AFGameLogicPlugin SHARED ${GameLogicPlugin_SRC})

set_target_properties(AFGameLogicPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFGameLogicPlugin_d")
set_target_properties(AFGameLogicPlugin PROPERTIES
    FOLDER "Server/GameServer"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

if(UNIX)
    target_link_libraries(AFGameLogicPlugin AFCore libprotobuf)
else()
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        target_link_libraries(AFGameLogicPlugin AFCore libprotobufd)
    else()
        target_link_libraries(AFGameLogicPlugin AFCore libprotobuf)
    endif()

    add_definitions(
        -D_USRDLL
        -D_LIB
        -DLIBPROTOC_EXPORTS
        -DPROTOBUF_USE_DLLS
        -D_SCL_SECURE_NO_WARNINGS
        )
endif()