cmake_minimum_required(VERSION 3.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(ARK)

#3rd cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

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

set(ROOT_DIR ${PROJECT_SOURCE_DIR})

include_directories(
    ${ROOT_DIR}
    ${ROOT_DIR}/Dep
    ${ROOT_DIR}/Dep/protobuf/src
    ${ROOT_DIR}/Dep/brynet/src
    ${ROOT_DIR}/Frame
)

if(UNIX)
    #linux
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g -ggdb -fPIC")
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -D_DEBUG ")
    else()
        set(CMAKE_BUILD_TYPE "Release")
        set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fPIC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DNDEBUG")
    endif()
    set(LDFLAGS "$ENV{LDFLAGS} -stdlib=libc++")
    set(DEP_LIB_DIR ${ROOT_DIR}/Dep/lib/ ${ROOT_DIR}/Bin/bin)
    set(BIN_OUTPUT_DIR ${ROOT_DIR}/Bin/bin/)
    set(PLUGIN_OUTPUT_DIR ${ROOT_DIR}/Bin/lib)
    add_definitions(-Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-strict-aliasing -Wno-pmf-conversions)
    
    if(ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    endif()
    
else()
    #windows
    add_compile_options("/std:c++latest")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
    set(DEP_LIB_DIR ${ROOT_DIR}/Dep/lib ${ROOT_DIR}/Bin/bin/)
    set(BIN_OUTPUT_DIR ${ROOT_DIR}/Bin/bin/)
    set(PLUGIN_OUTPUT_DIR ${ROOT_DIR}/Bin/lib)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${BIN_OUTPUT_DIR}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${BIN_OUTPUT_DIR}")
endif()

link_directories(${DEP_LIB_DIR})

#log
message("ROOT dir=${ROOT_DIR}")
message("Bin Output dir=${BIN_OUTPUT_DIR}")
message("DEP lib dir=${DEP_LIB_DIR}")
message("Plugin Output=${PLUGIN_OUTPUT_DIR}")
message("CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")

#codecov
find_package(codecov)
##################################################################
message("Start to build all...")
add_subdirectory(Frame/SDK)
add_subdirectory(Frame/Server)
#add_subdirectory(Frame/Examples)
#add_subdirectory(Frame/Tools)