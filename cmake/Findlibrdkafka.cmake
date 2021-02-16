MESSAGE(STATUS "Using bundled Findlibrdkafka.cmake...")
FIND_PATH(
        LIBRDKAFKA_INCLUDE_DIR
        librdkafka/rdkafka.h
        /usr/include/
        /usr/local/include/
)

if (UNIX)
    if (APPLE)
        FIND_LIBRARY(
                LIBRDKAFKA_LIBRARIES NAMES librdkafka.dylib
                PATHS /usr/lib/ /usr/local/lib/
        )
        FIND_LIBRARY(
                LIBRDKAFKA_CPP_LIBRARIES NAMES librdkafka++.dylib
                PATHS /usr/lib/ /usr/local/lib/
        )
    else ()
        FIND_LIBRARY(
                LIBRDKAFKA_LIBRARIES NAMES librdkafka.so
                PATHS /usr/lib/ /usr/local/lib/
        )
        FIND_LIBRARY(
                LIBRDKAFKA_CPP_LIBRARIES NAMES librdkafka++.so
                PATHS /usr/lib/ /usr/local/lib/
        )
    endif ()
else ()
    FIND_LIBRARY(
            LIBRDKAFKA_LIBRARIES NAMES librdkafka.dylib
            PATHS .
    )
    FIND_LIBRARY(
            LIBRDKAFKA_CPP_LIBRARIES NAMES librdkafka++.dylib
            PATHS .
    )
endif ()


mark_as_advanced(LIBRDKAFKA_INCLUDE_DIR LIBRDKAFKA_LIBRARIES LIBRDKAFKA_CPP_LIBRARIES)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(librdkafka LIBRDKAFKA_INCLUDE_DIR LIBRDKAFKA_LIBRARIES LIBRDKAFKA_CPP_LIBRARIES)

if (LIBRDKAFKA_FOUND)
    message(STATUS "Found librdkafka")
    message(STATUS "LIBRDKAFKA_INCLUDE_DIR: ${LIBRDKAFKA_INCLUDE_DIR}")
    message(STATUS "LIBRDKAFKA_LIBRARIES: ${LIBRDKAFKA_LIBRARIES}")
    message(STATUS "LIBRDKAFKA_CPP_LIBRARIES: ${LIBRDKAFKA_CPP_LIBRARIES}")
else ()
    message(FATAL_ERROR "Cannot found librdkafka, please install manually.")
endif ()