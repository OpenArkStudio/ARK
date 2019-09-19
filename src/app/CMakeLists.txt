set(project_name app)
file(GLOB_RECURSE app_SRC *.h *.hpp *.cpp *.cc *.c)
    
if(UNIX)
    #Set rpath
    set(CMAKE_INSTALL_RPATH ".")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

add_executable(app ${app_SRC})

if(UNIX)
    target_link_libraries(app pthread dl --coverage)
else(UNIX)
    target_link_libraries(app)
endif(UNIX)

set_target_properties(app PROPERTIES OUTPUT_NAME_DEBUG "app_d")
set_target_properties(app PROPERTIES
    FOLDER "app"
	ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
