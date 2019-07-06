cmake_minimum_required(VERSION 3.1)


#---------------------------------------------------------------------------------------
# Set default build to release
#---------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

#---------------------------------------------------------------------------------------
# Compiler config
#---------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


#---------------------------------------------------------------------------------------
# 3rd cmake modules
#---------------------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#---------------------------------------------------------------------------------------
# Self defined postfix
#---------------------------------------------------------------------------------------
set(CMAKE_DEBUG_POSTFIX "_d" CACHE STRING "Set debug library postfix")

#---------------------------------------------------------------------------------------
# ARK project
#---------------------------------------------------------------------------------------
include(cmake/utils.cmake)
ark_extract_version()

project(ark VERSION ${ARK_VERSION} LANGUAGES CXX)
message(STATUS "Build ark: v${ARK_VERSION}")

set(ROOT_DIR ${PROJECT_SOURCE_DIR})

include_directories(
    ${ROOT_DIR}
    ${ROOT_DIR}/dep
    ${ROOT_DIR}/dep/common
    ${ROOT_DIR}/dep/protobuf/src
    ${ROOT_DIR}/dep/brynet/src
    ${ROOT_DIR}/dep/spdlog/include
    ${ROOT_DIR}/src
    ${ROOT_DIR}/src/plugin
    ${ROOT_DIR}/src/server)

if(UNIX)
    #linux
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g -ggdb -fPIC")
    if(CMAKE_BUILD_TYPE MATCHES "debug")
        set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -D_DEBUG ")
    else()
        set(CMAKE_BUILD_TYPE "Release")
        set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG")
    endif()
    set(LDFLAGS "$ENV{LDFLAGS} -stdlib=libc++")
    set(DEP_LIB_DIR ${ROOT_DIR}/dep/lib/ ${ROOT_DIR}/bin/lib)
    set(BIN_OUTPUT_DIR ${ROOT_DIR}/bin/bin)
    set(LIB_OUTPUT_DIR ${ROOT_DIR}/bin/lib)
    add_definitions(-Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-strict-aliasing -Wno-pmf-conversions)
    
    option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)
    if(ENABLE_COVERAGE)
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
        message("Build with coverage")
        add_compile_options(--coverage -O0)
    endif()
    
else(WIN32)
    #windows
    add_compile_options("/std:c++latest")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    set(DEP_LIB_DIR ${ROOT_DIR}/dep/lib ${ROOT_DIR}/bin/lib)
    set(BIN_OUTPUT_DIR ${ROOT_DIR}/bin/bin)
    set(LIB_OUTPUT_DIR ${ROOT_DIR}/bin/lib)
    add_definitions(
        -DWIN
        -D_WINDOWS
        -D_CRT_SECURE_NO_WARNINGS)
    if(CMAKE_BUILD_TYPE MATCHES "debug")
        add_definitions(-D_DEBUG)
    else()
        add_definitions(-DNDEBUG)
    endif()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${LIB_OUTPUT_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${LIB_OUTPUT_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${LIB_OUTPUT_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${LIB_OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${LIB_OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${LIB_OUTPUT_DIR}")
endif()

link_directories(${DEP_LIB_DIR})

#---------------------------------------------------------------------------------------
# log
#---------------------------------------------------------------------------------------
message("ROOT dir=${ROOT_DIR}")
message("Bin Output dir=${BIN_OUTPUT_DIR}")
message("Dep lib dir=${DEP_LIB_DIR}")
message("Plugin Output=${LIB_OUTPUT_DIR}")
message("CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")

#---------------------------------------------------------------------------------------
# codecov
#---------------------------------------------------------------------------------------
find_package(codecov)

#---------------------------------------------------------------------------------------
# sub projects
#---------------------------------------------------------------------------------------
message("Start to build all...")

add_subdirectory(src/app)
add_subdirectory(src/proto)

add_subdirectory(src/plugin)
add_subdirectory(src/server)

add_subdirectory(samples)

add_subdirectory(test)
