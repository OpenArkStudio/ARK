file(GLOB AFCore_H *.h)
file(GLOB AFCore_HPP *.hpp)
file(GLOB AFCore_CPP *.cpp)

add_library(AFCore SHARED 
    ${AFCore_H}
    ${AFCore_HPP}
    ${AFCore_CPP}
)

set_target_properties(AFCore PROPERTIES OUTPUT_NAME_DEBUG "AFCore_d")
set_target_properties(AFCore PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

target_link_libraries(AFCore)