cmake_minimum_required(VERSION 3.0)

# UNIX : is TRUE on all UNIX-like OS's, including Apple OS X and CygWin
# WIN32 : is TRUE on Windows. Prior to 2.8.4 this included CygWin
# APPLE : is TRUE on Apple systems. Note this does not imply the system is Mac OS X, only that APPLE is #defined in C/C++ header files.
# MINGW : is TRUE when using the MinGW compiler in Windows
# MSYS : is TRUE when using the MSYS developer environment in Windows
# CYGWIN : is TRUE on Windows when using the CygWin version of cmake

#---------------------------------------------------------------------------------------
# Set default build type to release
#---------------------------------------------------------------------------------------
if(UNIX)
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
    endif()
endif()

#---------------------------------------------------------------------------------------
# Set CMAKE_MACOSX_RPATH
#---------------------------------------------------------------------------------------
if(APPLE)
    set(CMAKE_MACOSX_RPATH 1)
endif()

# CMake 3.9 and newer remove any effect the following settings may have on the install_name of a target on macOS:
# 
# BUILD_WITH_INSTALL_RPATH target property
# SKIP_BUILD_RPATH target property
# CMAKE_SKIP_RPATH variable
# CMAKE_SKIP_INSTALL_RPATH variable
if(APPLE AND (${CMAKE_MAJOR_VERSION} GREATER_EQUAL 3 AND ${CMAKE_MINOR_VERSION} GREATER_EQUAL 9))
    #message("cmake version is ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")
    option(MACOS_NO_RPATH "CMake 3.9 and newer remove any effect of RPATH" ON)
endif()

#---------------------------------------------------------------------------------------
# Compiler config
#---------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#---------------------------------------------------------------------------------------
# 3rd cmake modules
#---------------------------------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

#---------------------------------------------------------------------------------------
# Coverage reporting
#---------------------------------------------------------------------------------------
option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)

#---------------------------------------------------------------------------------------
# Building tests and examples
#---------------------------------------------------------------------------------------
option(BUILD_TESTS "Enable build tests" ON)
option(BUILD_SAMPLES "Enable build examples" ON) # TODO: will change to examples

#---------------------------------------------------------------------------------------
# Get version number.
#---------------------------------------------------------------------------------------
include(cmake/utils.cmake)
ark_extract_version()

#---------------------------------------------------------------------------------------
# ARK project
#---------------------------------------------------------------------------------------
project(ark VERSION ${ARK_VERSION} LANGUAGES CXX)
message(STATUS "Build ark: v${ARK_VERSION}")

set(ROOT_DIR ${PROJECT_SOURCE_DIR})

include_directories(
    ${ROOT_DIR}
    ${ROOT_DIR}/dep
    ${ROOT_DIR}/dep/common
    ${ROOT_DIR}/dep/protobuf/src
    ${ROOT_DIR}/dep/brynet/include
    ${ROOT_DIR}/dep/spdlog/include
    ${ROOT_DIR}/src
    ${ROOT_DIR}/src/plugin
    ${ROOT_DIR}/src/server)

set(BIN_OUTPUT_DIR ${ROOT_DIR}/bin/bin)
set(DEP_LIB_DIR ${BIN_OUTPUT_DIR} ${ROOT_DIR}/dep/lib/)

#---------------------------------------------------------------------------------------
# Dependencies
#include(cmake/deps.cmake)
add_subdirectory(dep)
#---------------------------------------------------------------------------------------

if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest -D_CRT_SECURE_NO_WARNINGS -D_SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
else(UNIX)
    SET(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -ggdb -Wall -Wextra -D_DEBUG -fPIC")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -ggdb -Wall -Wextra -DNDEBUG -fPIC")
    
    if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        set(CMAKE_CXX_STANDARD 11)
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
        set(CMAKE_CXX_STANDARD 17)
    endif()
    #add_definitions(-Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-strict-aliasing -Wno-pmf-conversions -Wignored-qualifiers)
    add_definitions(-Wno-unused-parameter -Wno-unused-variable -Wno-unused-function)
    if(ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
        SET(CMAKE_CXX_FLAGS_DEBUG "-lgcov --coverage")
        SET(CMAKE_CXX_FLAGS_RELEASE "-lgcov --coverage")
        message("Build with coverage")
        add_compile_options(--coverage -O0)
    endif()

    if(XCODE)
        # set the output dir for xcode
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")

        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
    endif(XCODE)
    
endif()

link_directories(${DEP_LIB_DIR})

#---------------------------------------------------------------------------------------
# log
#---------------------------------------------------------------------------------------
message(STATUS "Current system is [${CMAKE_SYSTEM}]")
message(STATUS "C++ Standard is ${CMAKE_CXX_STANDARD}")
message(STATUS "ROOT_DIR=${ROOT_DIR}")
message(STATUS "DEP_LIB_DIR=${DEP_LIB_DIR}")
message(STATUS "BIN_OUTPUT_DIR=${BIN_OUTPUT_DIR}")
message(STATUS "CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")

#---------------------------------------------------------------------------------------
# codecov
#---------------------------------------------------------------------------------------
find_package(codecov)

#---------------------------------------------------------------------------------------
# sub projects
#---------------------------------------------------------------------------------------
message(STATUS "Start to build all ...")

#---------------------------------------------------------------------------------------
# Building 3rd party dependencies.
#---------------------------------------------------------------------------------------
# message(STATUS "Start to build all 3rd party dependencies...")
# add_subdirectory(dep)

#---------------------------------------------------------------------------------------
# Self defined postfix
#---------------------------------------------------------------------------------------
set(CMAKE_DEBUG_POSTFIX "_d" CACHE STRING "Set debug library postfix")

#---------------------------------------------------------------------------------------
# Building ARK source.
#---------------------------------------------------------------------------------------
message(STATUS "Start to build all source...")
add_subdirectory(src/app)
add_subdirectory(src/proto)
add_subdirectory(src/plugin)
add_subdirectory(src/server)

#---------------------------------------------------------------------------------------
# Building samples
#---------------------------------------------------------------------------------------
if(BUILD_SAMPLES)
    message(STATUS "Buiding samples ...")
    add_subdirectory(samples)    
endif(BUILD_SAMPLES)

#---------------------------------------------------------------------------------------
# Building tests
#---------------------------------------------------------------------------------------
if(BUILD_TESTS)
    message(STATUS "Buiding tests ...")
    enable_testing()
    add_subdirectory(test)
endif(BUILD_TESTS)
