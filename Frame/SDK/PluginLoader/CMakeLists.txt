file(GLOB AFNet_H *.h)
file(GLOB AFNet_HPP *.hpp)
file(GLOB AFNet_CPP *.cpp)

add_library(AFNet SHARED 
    ${AFNet_H}
    ${AFNet_HPP}
    ${AFNet_CPP}
)

set_target_properties(AFNet PROPERTIES OUTPUT_NAME_DEBUG "AFNet_d")
set_target_properties(AFNet PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

target_link_libraries(AFNet)