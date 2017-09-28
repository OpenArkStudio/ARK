cmake_minimum_required(VERSION 2.8)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(GLOBAL PROPERTY USE_FOLDERS ON)
set(ROOT_DIR ${PROJECT_SOURCE_DIR})

#check cxx flag in linux system
if(UNIX)
    #linux
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
    if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif()
endif()

########################################################################
macro(SDK_LINK_MACRO project_name)
    add_dependencies(${project_name} AFCore AFNet)
    target_link_libraries(${project_name} event event_core AFCore AFNet)
endmacro(SDK_LINK_MACRO project_name)

########################################################################

project(ArkFameFrame)

include_directories(
    ${ROOT_DIR}/
    ${ROOT_DIR}/Dep/
    ${ROOT_DIR}/Dep/libevent/include/
    ${ROOT_DIR}/Dep/libevent/compat/
    ${ROOT_DIR}/Dep/evpp/
    ${ROOT_DIR}/Dep/protobuf/src
)

if(UNIX)
    #linux
    include_directories()(${ROOT_DIR}/Dep/libevent/include/)

    if(CMAKE_BUILD_TYPE STREQUAL "Debug" or CMAKE_BUILD_TYPE STREQUAL "DEBUG" or CMAKE_BUILD_TYPE STREQUAL "debug")
        set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -Wall -g -ggdb -DDEBUG -fPIC")
        set(DEP_LIB_DIR ${ROOT_DIR}/Dep/lib/ ${ROOT_DIR}/Dep/lib/Debug/ ${ROOT_DIR}/Bin/Debug/)
    else()
        set(CMAKE_BUILD_TYPE "Release")
        set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -Wall -fPIC")
        set(DEP_LIB_DIR ${ROOT_DIR}/Dep/lib/ ${ROOT_DIR}/Dep/lib/Release/ ${ROOT_DIR}/Bin/Release/)
    endif()
    set(LDFLAGS "$ENV{LDFLAGS} -stdlib=libc++")
    ADD_DEFINITIONS(-std=c++11)
    set(SDK_OUTPUT_DIR ${ROOT_DIR}/Bin/Comm/${CMAKE_BUILD_TYPE})
    #if(CMAKE_BUILD_TYPE STREQUAL "Debug" or CMAKE_BUILD_TYPE STREQUAL "DEBUG" or CMAKE_BUILD_TYPE STREQUAL "debug")
    #    set(SDK_OUTPUT_DIR ${ROOT_DIR}/Bin/Comm/Debug)
    #else()
    #    set(SDK_OUTPUT_DIR ${ROOT_DIR}/Bin/Release)
    #endif()
else()
    #windows
    include_directories(${ROOT_DIR}/Dep/libevent/WIN32-code/nmake/)
    set(CMAKE_CXX_FLAGS "-D_X64 -D_WINDOWS /EHsc")
    set(DEP_LIB_DIR ${ROOT_DIR}/Dep/lib ${ROOT_DIR}/Bin/)
    set(SDK_OUTPUT_DIR ${ROOT_DIR}/Bin/Comm)
endif()

#log SDK_OUTPUT_DIR
message("SDK Output dir=${SDK_OUTPUT_DIR}")

link_directories(${DEP_LIB_DIR})
##################################################################
message("Start to build all...")
add_subdirectory(${ROOT_DIR}/Frame/SDK)
add_subdirectory(${ROOT_DIR}/Frame/Server)
add_subdirectory(${ROOT_DIR}/Frame/Examples)
#add_subdirectory(${ROOT_DIR}/Frame/Tools)