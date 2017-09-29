file(GLOB AFKernel_H *.h)
file(GLOB AFKernel_HPP *.hpp)
file(GLOB AFKernel_CPP *.cpp)

file(GLOB Remove_item_cpp AFCAOIModule.cpp)
file(GLOB Remove_item_h AFCAOIModule.h)
list(REMOVE_ITEM AFKernel_CPP ${Remove_item_cpp})
list(REMOVE_ITEM AFKernel_H ${Remove_item_h})

add_library(AFKernelPlugin SHARED 
    ${AFKernel_H}
    ${AFKernel_HPP}
    ${AFKernel_CPP}
)

add_dependencies(AFKernelPlugin AFCore)

set_target_properties(AFKernelPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFKernelPlugin_d")
set_target_properties(AFKernelPlugin PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

target_link_libraries(AFKernelPlugin AFCore)