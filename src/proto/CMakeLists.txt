set(project_name AFProto)
file(GLOB_RECURSE SDK_SRC *.h *.hpp *.cpp *.cc *.c)

if(UNIX)
    #Set rpath
    SET(CMAKE_INSTALL_RPATH ".")
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

add_library(${project_name} STATIC ${SDK_SRC})
set_target_properties(${project_name} PROPERTIES PREFIX "")
set_target_properties(${project_name} PROPERTIES
    FOLDER "proto")
if(UNIX)
    target_link_libraries(${project_name})
else(UNIX)
    target_link_libraries(${project_name} 
        debug libprotobufd
        optimized libprotobuf)
endif(UNIX)
