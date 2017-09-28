file(GLOB AFProto_H *.h)
file(GLOB AFProto_HPP *.hpp)
file(GLOB AFProto_CPP *.cc)

add_library(AFProto SHARED 
    ${AFNET_H}
    ${AFNET_HPP}
    ${AFNET_CPP}
)

set_target_properties(AFProto PROPERTIES OUTPUT_NAME_DEBUG "AFProto_d")
set_target_properties(AFProto PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${SDK_OUTPUT_DIR}
)

target_link_libraries(AFProto)