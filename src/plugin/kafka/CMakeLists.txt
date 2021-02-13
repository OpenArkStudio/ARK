if (WIN32)
    include_directories(
    )

    BUILD_PLUGIN_MACRO(AFKafkaPlugin
            )
else ()
    find_package(librdkafka REQUIRED)
    find_package(OpenSSL REQUIRED)
    find_package(Zstd REQUIRED)
    find_package(LZ4 REQUIRED)
    find_package(ZLIB REQUIRED)


    include_directories(
            ${LIBRDKAFKA_INCLUDE_DIR}
            ${OPENSSL_INCLUDE_DIR}
            ${ZSTD_INCLUDE_DIRS}
            ${LZ4_INCLUDE_DIR}
            ${ZLIB_INCLUDE_DIRS}
    )

    BUILD_PLUGIN_MACRO(AFKafkaPlugin
            ${LIBRDKAFKA_CPP_LIBRARIES}
            ${LIBRDKAFKA_LIBRARIES}
            ${OPENSSL_LIBRARIES}
            ${ZSTD_LIBRARIES}
            ${LZ4_LIBRARY}
            ${ZLIB_LIBRARIES}
            )
endif ()
