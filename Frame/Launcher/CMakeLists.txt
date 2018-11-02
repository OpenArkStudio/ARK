set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")

file(GLOB Launcher_SRC *.h *.hpp *.cpp *.cc *.c)
    
if(UNIX)
    #Set rpath
    set(CMAKE_INSTALL_RPATH "./lib/" "../lib/")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()



add_executable(Launcher ${Launcher_SRC})
add_dependencies(Launcher AFCore)

if(UNIX)
    target_link_libraries(Launcher AFCore pthread dl)
else(UNIX)
    target_link_libraries(Launcher
        debug AFCore_d.lib
        
        optimized AFCore.lib)
endif(UNIX)

set_target_properties(Launcher PROPERTIES OUTPUT_NAME_DEBUG "Launcher_d")
set_target_properties(Launcher PROPERTIES
    FOLDER "Launcher"
    ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
