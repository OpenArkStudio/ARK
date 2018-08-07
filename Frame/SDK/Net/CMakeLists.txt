file(GLOB AFNet_SRC *.h *.hpp *.cpp *.cc *.c)

file(GLOB RemoveItems Test*.cpp)
list(REMOVE_ITEM AFNet_SRC ${RemoveItems})

add_library(AFNet STATIC ${AFNet_SRC})

set_target_properties(AFNet PROPERTIES PREFIX "")
set_target_properties(AFNet PROPERTIES OUTPUT_NAME_DEBUG "AFNet_d")
set_target_properties(AFNet PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

if(UNIX)
    target_link_libraries(AFNet brynet.a)
else()
    target_link_libraries(AFNet brynet)
    add_definitions(-D_LIB -DWIN32 -DWIN)
endif()
