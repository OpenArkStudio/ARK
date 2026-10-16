find_package(Zstd REQUIRED)
include_directories(${ZSTD_INCLUDE_DIRS})

# find_package(LZ4 REQUIRED)
# include_directories(${LZ4_INCLUDE_DIR})

find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    mark_as_advanced(ZLIB_FOUND ZLIB_LIBRARIES ZLIB_INCLUDE_DIR) 
    message(STATUS "Found ZLib add_library: ${ZLIB_LIBRARIES}")
    message(STATUS "Found ZLib headers: ${ZLIB_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "Zlib not found")
endif(ZLIB_FOUND)

include_directories(${ZLIB_INCLUDE_DIRS})

BUILD_PLUGIN_MACRO(AFUtilityPlugin ${ZSTD_LIBRARIES} ${ZLIB_LIBRARIES})
# BUILD_PLUGIN_MACRO(AFUtilityPlugin ${LZ4_LIBRARY})