file(GLOB AFKernel_SRC *.h *.hpp *.cpp *.cc *.c)

file(GLOB RemoveItems AFCAOIModule.h AFCAOIModule.cpp)
list(REMOVE_ITEM AFKernel_SRC ${RemoveItems})

add_library(AFKernelPlugin SHARED ${AFKernel_SRC})

add_dependencies(AFKernelPlugin AFCore)

set_target_properties(AFKernelPlugin PROPERTIES PREFIX "")
set_target_properties(AFKernelPlugin PROPERTIES OUTPUT_NAME_DEBUG "AFKernelPlugin_d")
set_target_properties(AFKernelPlugin PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

target_link_libraries(AFKernelPlugin AFCore)