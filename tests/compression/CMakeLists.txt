find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Zstd REQUIRED)
find_package(LZ4 REQUIRED)

add_library(ark_compression_header_checks OBJECT headers/zlib.cpp headers/zstd.cpp headers/lz4.cpp)
set_property(TARGET ark_compression_header_checks PROPERTY LINK_LIBRARIES ark::core)
target_include_directories(ark_compression_header_checks PRIVATE
    ${ZLIB_INCLUDE_DIRS} ${ZSTD_INCLUDE_DIRS} ${LZ4_INCLUDE_DIR})

add_executable(ark_compression_tests ../core/main.cpp compression.cpp)
set_property(TARGET ark_compression_tests PROPERTY LINK_LIBRARIES
    ark::core Threads::Threads ZLIB::ZLIB ${ZSTD_LIBRARIES} ${LZ4_LIBRARY})
target_include_directories(ark_compression_tests PRIVATE ${ZSTD_INCLUDE_DIRS} ${LZ4_INCLUDE_DIR})
add_dependencies(ark_compression_tests ark_compression_header_checks)
add_custom_target(check-compression
    COMMAND $<TARGET_FILE:ark_compression_tests>
    DEPENDS ark_compression_tests
    USES_TERMINAL)
