file(GLOB AFProto_SRC *.h *.hpp *.cpp *.cc *.c cpp/*.h cpp/*.hpp cpp/*.cpp cpp/*.cc cpp/*.c)

if(UNIX)
    #Set rpath
    set(CMAKE_INSTALL_RPATH "./" "../lib/")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

add_library(AFProto SHARED ${AFProto_SRC})

if(UNIX)
    target_link_libraries(AFProto protobuf)
    add_definitions(
        -DLIBPROTOC_EXPORTS
        -DPROTOBUF_USE_DLLS)
else()
    target_link_libraries(AFProto
        debug libprotobufd
        optimized libprotobuf
    )
    
    add_definitions(
        -D_USRDLL
        -D_LIB
        -DLIBPROTOC_EXPORTS
        -DPROTOBUF_USE_DLLS)
endif()

set_target_properties(AFProto PROPERTIES PREFIX "")
set_target_properties(AFProto PROPERTIES OUTPUT_NAME_DEBUG "AFProto_d")
set_target_properties(AFProto PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR})