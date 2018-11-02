if(UNIX)
    #Set rpath
    SET(CMAKE_INSTALL_RPATH "./" "../lib/")
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

set(project_name AFProto)
file(GLOB SDK_SRC *.h *.hpp *.cpp *.cc *.c cpp/*.h cpp/*.hpp cpp/*.cpp cpp/*.cc)
add_library(${project_name} STATIC ${SDK_SRC})
add_dependencies(${project_name} AFCore)
set_target_properties(${project_name} PROPERTIES PREFIX "")
set_target_properties(${project_name} PROPERTIES OUTPUT_NAME_DEBUG "${project_name}_d")
set_target_properties(${project_name} PROPERTIES
    FOLDER "SDK"
    ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})
if(UNIX)
    target_link_libraries(${project_name})
else(UNIX)
    target_link_libraries(${project_name} AFCore debug libprotobufd optimized libprotobuf)
endif(UNIX)
