project(AFCore)

include_directories(${AFCore_SOURCE_DIR}/../../)
include_directories(${AFCore_SOURCE_DIR}/../../../Dep)

aux_source_directory(. DIR_SRCS)

add_library(AFCore SHARED ${DIR_SRCS})