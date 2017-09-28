file(GLOB AFNet_H *.h)
file(GLOB AFNet_HPP *.hpp)
file(GLOB AFNet_CPP *.cpp)

file(GLOB RemoveItems Test*.cpp)
list(REMOVE_ITEM AFNet_CPP ${RemoveItems})

add_library(AFNet SHARED 
    ${AFNet_H}
    ${AFNet_HPP}
    ${AFNet_CPP}
)

if(unix)
    include_directories(${ROOT_DIR}/Dep/glog/src/)    
else()
    include_directories(${ROOT_DIR}/Dep/glog/src/windows)
endif()

set_target_properties(AFNet PROPERTIES OUTPUT_NAME_DEBUG "AFNet_d")
set_target_properties(AFNet PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

if(unix)
    target_link_libraries(AFNet libevent.a libevent_core.a libprotobuf.so)
    add_definitions(-D_USRDLL)
else()
    target_link_libraries(AFNet libprotobuf libevent libevent_core)
    add_definitions(-D_LIB -DWIN32 -DWIN)
endif()