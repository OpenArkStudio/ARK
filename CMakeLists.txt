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
if (UNIX)
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
    endif ()
endif ()

#---------------------------------------------------------------------------------------
# Set CMAKE_MACOSX_RPATH
#---------------------------------------------------------------------------------------
if (APPLE)
    set(CMAKE_MACOSX_RPATH 1)
endif ()

# CMake 3.9 and newer remove any effect the following settings may have on the install_name of a target on macOS:
# 
# BUILD_WITH_INSTALL_RPATH target property
# SKIP_BUILD_RPATH target property
# CMAKE_SKIP_RPATH variable
# CMAKE_SKIP_INSTALL_RPATH variable
if (APPLE AND (${CMAKE_MAJOR_VERSION} GREATER_EQUAL 3 AND ${CMAKE_MINOR_VERSION} GREATER_EQUAL 9))
    #message("cmake version is ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")
    option(MACOS_NO_RPATH "CMake 3.9 and newer remove any effect of RPATH" ON)
endif ()

#---------------------------------------------------------------------------------------
# Compiler config
#---------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
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
# CMake switches
#---------------------------------------------------------------------------------------
option(ASIO_STANDALONE "Enable asio standalone" OFF) # used by zephyr
option(ZEPHYR_USE_XD_KCP "Enable XD KCP" ON) # used by zephyr

if (ZEPHYR_USE_XD_KCP)
    add_definitions(-DXD_KCP) # XD KCP
endif ()

option(ARK_BUILD_TESTS "Enable build tests" ON)
option(ARK_BUILD_EXAMPLES "Enable build examples" ON)

#---------------------------------------------------------------------------------------
# project switches
#---------------------------------------------------------------------------------------
option(ARK_USE_GUID_32 "use 32 bits GUID" OFF)
if (ARK_USE_GUID_32)
    add_definitions(-DARK_USE_GUID_32)
endif (ARK_USE_GUID_32)

option(ARK_DOCKER_ALPINE "build docker image by alpine" OFF)
if (ARK_DOCKER_ALPINE)
    add_definitions(-DARK_DOCKER_ALPINE)
endif (ARK_DOCKER_ALPINE)

option(ARK_OPEN_FILE_LOG "write log into file" ON)
if (ARK_OPEN_FILE_LOG)
    add_definitions(-DOPEN_FILE_LOG)
endif (ARK_OPEN_FILE_LOG)

#---------------------------------------------------------------------------------------
# Get version number.
#---------------------------------------------------------------------------------------
include(cmake/utils.cmake)
ark_extract_version()

#---------------------------------------------------------------------------------------
# ARK project
#---------------------------------------------------------------------------------------
project(ark VERSION ${ARK_VERSION} LANGUAGES CXX C)
message(STATUS "Build ark: v${ARK_VERSION}")

set(ROOT_DIR ${PROJECT_SOURCE_DIR})
set(BIN_OUTPUT_DIR ${ROOT_DIR}/build/bin)
set(3RD_PARTY_DIR ${ROOT_DIR}/3rdparty)
set(3RD_PARTY_LIB_DIR ${BIN_OUTPUT_DIR})

#---------------------------------------------------------------------------------------
# 3rdparty
#---------------------------------------------------------------------------------------
set(SELF_BUILD_TYPE ${CMAKE_BUILD_TYPE})
add_subdirectory(3rdparty)
set(CMAKE_BUILD_TYPE ${SELF_BUILD_TYPE})

if (WIN32)
    if (MSVC_VERSION LESS 1911)
        message(FATAL_ERROR "need msvc++ 14.1 Visual Studio 2017+")
    endif (MSVC_VERSION LESS 1911)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /bigobj -D_CRT_SECURE_NO_WARNINGS -D_SILENCE_CXX17_C_HEADER_DEPRECATION_WARNING")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
	
	add_definitions(-DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=0x0601)
else (UNIX)
    if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
        message(FATAL_ERROR "need gcc7.0+ to support C++17")
    endif ()

    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb -Wall -Wextra -D_DEBUG -fPIC")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -DNDEBUG -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    #add_definitions(-Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-strict-aliasing -Wno-pmf-conversions -Wignored-qualifiers)
    add_definitions(-Wno-unused-parameter -Wno-unused-variable -Wno-unused-function)

    if (XCODE)
        # set the output dir for xcode
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")

        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
    endif (XCODE)

endif ()

# protobuf
find_package(Protobuf 3.0.0 REQUIRED)
if (PROTOBUF_FOUND)
    message(STATUS "protobuf library found")
    include_directories(${Protobuf_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "protobuf library is needed but can't be found")
endif ()

if (ASIO_STANDALONE)
    # standalone asio
    add_definitions(-DASIO_STANDALONE)
    #include_directories(${3rdparty}/zephyr/asio/asio/include)
else ()
    # boost.asio
    find_package(Boost 1.72.0 REQUIRED)
    if (Boost_FOUND)
        message(STATUS "boost library found")
    else ()
        message(FATAL_ERROR "boost library is needed but can‘t be found")
    endif ()
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif ()

# include and link directories
include_directories(
        ${3RD_PARTY_DIR}
        ${3RD_PARTY_DIR}/zephyr/include
        ${3RD_PARTY_DIR}/spdlog/include
        ${ROOT_DIR}
        ${ROOT_DIR}/src
        ${ROOT_DIR}/src/plugin
        ${ROOT_DIR}/src/server
        ${ROOT_DIR}/src/escort)

link_directories(${3RD_PARTY_LIB_DIR})

#---------------------------------------------------------------------------------------
# log
#---------------------------------------------------------------------------------------
message(STATUS "Current system is [${CMAKE_SYSTEM}]")
message(STATUS "ROOT_DIR=${ROOT_DIR}")
message(STATUS "3RD_PARTY_LIB_DIR=${3RD_PARTY_LIB_DIR}")
message(STATUS "BIN_OUTPUT_DIR=${BIN_OUTPUT_DIR}")
message(STATUS "CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")

#---------------------------------------------------------------------------------------
# codecov
#---------------------------------------------------------------------------------------
#find_package(codecov)

#---------------------------------------------------------------------------------------
# sub projects
#---------------------------------------------------------------------------------------
message(STATUS "Start to build all ...")

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
if (ARK_BUILD_EXAMPLES)
    message(STATUS "Building samples ...")
    add_subdirectory(examples)
endif (ARK_BUILD_EXAMPLES)

#---------------------------------------------------------------------------------------
# Building tests
#---------------------------------------------------------------------------------------
if (ARK_BUILD_TESTS)
    message(STATUS "Building tests ...")
    enable_testing()
    add_subdirectory(test)
endif (ARK_BUILD_TESTS)
