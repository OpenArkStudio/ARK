set(project_name test)

file(GLOB_RECURSE TEST_SRC test.cpp)
    
if(UNIX)
    #Set rpath
    set(CMAKE_INSTALL_RPATH ".")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

add_executable(ark_test ${TEST_SRC})

if(UNIX)
    target_link_libraries(ark_test brynet ssl pthread dl --coverage)
else(WIN32)
    target_link_libraries(ark_test ws2_32.lib 
        debug brynetd.lib
        optimized brynet.lib)
endif()

set_target_properties(ark_test PROPERTIES
    FOLDER "test"
    ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
