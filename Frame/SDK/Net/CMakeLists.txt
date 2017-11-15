file(GLOB AFNet_SRC *.h *.hpp *.cpp *.cc *.c)

file(GLOB RemoveItems Test*.cpp)
list(REMOVE_ITEM AFNet_SRC ${RemoveItems})

add_library(AFNet STATIC ${AFNet_SRC})

#if(UNIX)
#    include_directories(${ROOT_DIR}/Dep/glog/src/)    
#else()
#    include_directories(${ROOT_DIR}/Dep/glog/src/windows)
#endif()

set_target_properties(AFNet PROPERTIES PREFIX "")
set_target_properties(AFNet PROPERTIES OUTPUT_NAME_DEBUG "AFNet_d")
set_target_properties(AFNet PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

if(UNIX)
    target_link_libraries(AFNet proto protobuf event event_core)
    add_definitions(-D_USRDLL)
else()
    target_link_libraries(AFNet evpp_static protobuf libevent libevent_core)
    add_definitions(-D_LIB -DWIN32 -DWIN)
endif()
