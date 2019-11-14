function(TEST_FUNCION project_name)
    file(GLOB_RECURSE TEST_SRC ${project_name}.cpp)
    
    if(UNIX AND (NOT APPLE))
        #Set rpath
        set(CMAKE_INSTALL_RPATH ".")
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif(UNIX AND (NOT APPLE))
    
    add_executable(${project_name} ${TEST_SRC})
    set_target_properties(${project_name} PROPERTIES
    FOLDER "test"
    ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
    add_test(NAME ${project_name} COMMAND ${project_name} WORKING_DIRECTORY ${ROOT_DIR}/bin)
endfunction(TEST_FUNCION)

set(UNIT_TESTS
    test_datetime
    test_base64
    test_xxtea
    test_random
    test_singleton)

foreach(test_index ${UNIT_TESTS})
  TEST_FUNCION(${test_index})
endforeach()