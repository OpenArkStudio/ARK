function(TEST_FUNCTION project_name)
    file(GLOB_RECURSE TEST_SRC ${project_name}.cpp)
    
    if(UNIX AND (NOT APPLE))
        #Set rpath
        set(CMAKE_INSTALL_RPATH ".")
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif(UNIX AND (NOT APPLE))

    add_executable(${project_name} ${TEST_SRC})

    # test_zstd
    if(${project_name} STREQUAL "test_zstd")
        find_package(Zstd REQUIRED)
        include_directories(${ZSTD_INCLUDE_DIRS})
        target_link_libraries(${project_name} ${ZSTD_LIBRARIES})
    endif()

    # # test_lz4
    # if(${project_name} STREQUAL "test_lz4")
    #     find_package(LZ4 REQUIRED)
    #     include_directories(${LZ4_INCLUDE_DIR})
    #     target_link_libraries(${project_name} ${LZ4_LIBRARY})
    # endif()

    # test zlib
    if(${project_name} STREQUAL "test_zlib")
        find_package(ZLIB REQUIRED)
        if (ZLIB_FOUND)
            mark_as_advanced(ZLIB_FOUND ZLIB_LIBRARIES ZLIB_INCLUDE_DIR) 
            message(STATUS "Found ZLib library: ${ZLIB_LIBRARIES}")
            message(STATUS "Found ZLib headers: ${ZLIB_INCLUDE_DIR}")
        else()
            message(FATAL_ERROR "Zlib not found")
        endif(ZLIB_FOUND)
        include_directories(${ZLIB_INCLUDE_DIRS})
        target_link_libraries(${project_name} ${ZLIB_LIBRARIES})
    endif()

    set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}_d")
    set_target_properties(${project_name} PROPERTIES
    FOLDER "test"
    ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
    add_test(NAME ${project_name} COMMAND ${project_name} WORKING_DIRECTORY ${BIN_OUTPUT_DIR})
endfunction(TEST_FUNCTION)

set(UNIT_TESTS
    test_datetime
    test_base64
    test_xxtea
    test_random
    test_singleton
    test_hex
    test_zstd
    #test_lz4
    test_zlib)

foreach(test_index ${UNIT_TESTS})
    TEST_FUNCTION(${test_index})
endforeach()