function(ark_validate_plugin_name plugin_name)
    if(NOT plugin_name MATCHES "^[a-z][a-z0-9]*(_[a-z0-9]+)*$")
        message(FATAL_ERROR "Plugin library names must use lowercase snake_case: ${plugin_name}")
    endif()
endfunction()

function(BUILD_PLUGIN_MACRO project_name)
    ark_validate_plugin_name("${project_name}")
    if(UNIX AND NOT APPLE)
        set(CMAKE_INSTALL_RPATH ".")
        set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    endif()
    file(GLOB_RECURSE PLUGIN_SRC CONFIGURE_DEPENDS *.h *.hpp *.cpp *.cc *.c)
    get_filename_component(plugin_name "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
    if(plugin_name IN_LIST ark_colocated_plugin_names)
        set(PLUGIN_HEADERS ${ark_${plugin_name}_public_headers})
    else()
        file(GLOB_RECURSE PLUGIN_HEADERS CONFIGURE_DEPENDS
            "${ROOT_DIR}/include/ark/${plugin_name}/*.h"
            "${ROOT_DIR}/include/ark/${plugin_name}/*.hpp")
    endif()
    add_library(${project_name} SHARED ${PLUGIN_SRC} ${PLUGIN_HEADERS})
    set_target_properties(${project_name} PROPERTIES
        PREFIX ""
        FOLDER "plugins"
        ARCHIVE_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${BIN_OUTPUT_DIR}")
    add_dependencies(${project_name} AFProto)
    target_link_libraries(${project_name} ark::ark ${ARGN})
endfunction()

# Public plugin APIs can be consumed without building their shared libraries.
# These plugins keep their source headers beside their implementation.
# Only this explicit list enters the build-tree SDK; plugin assembly stays private.
set(ark_mysql_public_headers "${CMAKE_CURRENT_SOURCE_DIR}/mysql/mysql_module.hpp")
set(ark_redis_public_headers "${CMAKE_CURRENT_SOURCE_DIR}/redis/redis_module.hpp")
set(ark_utility_public_headers
    "${CMAKE_CURRENT_SOURCE_DIR}/utility/guid_module.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utility/timer_module.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utility/schedule_module.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utility/compress_module.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utility/scheduler.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/utility/ctpl.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/utility/ctpl_stl.h"
)
set(ark_colocated_plugin_names mysql redis utility)
set(ark_plugin_sdk_include_dir "${CMAKE_CURRENT_BINARY_DIR}/sdk-include")
foreach(plugin_name IN LISTS ark_colocated_plugin_names)
    set(staged_headers)
    set(staging_dir "${ark_plugin_sdk_include_dir}/ark/${plugin_name}")
    foreach(header IN LISTS ark_${plugin_name}_public_headers)
        get_filename_component(header_name "${header}" NAME)
        set(staged_header "${staging_dir}/${header_name}")
        configure_file("${header}" "${staged_header}" COPYONLY)
        list(APPEND staged_headers "${staged_header}")
    endforeach()
    # These managed directories are owned by the explicit mapping. This
    # also removes the last staged header when a plugin's list becomes empty.
    file(GLOB_RECURSE old_headers LIST_DIRECTORIES FALSE "${staging_dir}/*")
    foreach(header IN LISTS old_headers)
        if(NOT header IN_LIST staged_headers)
            file(REMOVE "${header}")
        endif()
    endforeach()
endforeach()
file(GLOB plugin_entries CONFIGURE_DEPENDS LIST_DIRECTORIES TRUE "${CMAKE_CURRENT_SOURCE_DIR}/*")
set(ark_plugin_names)
foreach(plugin_dir IN LISTS plugin_entries)
    if(IS_DIRECTORY "${plugin_dir}" AND EXISTS "${plugin_dir}/CMakeLists.txt")
        get_filename_component(plugin_name "${plugin_dir}" NAME)
        list(APPEND ark_plugin_names "${plugin_name}")
        add_library(ark_${plugin_name}_api INTERFACE)
        add_library(ark::${plugin_name} ALIAS ark_${plugin_name}_api)
        target_include_directories(ark_${plugin_name}_api INTERFACE
            "$<BUILD_INTERFACE:${ROOT_DIR}/include>")
        if(plugin_name IN_LIST ark_colocated_plugin_names)
            target_include_directories(ark_${plugin_name}_api BEFORE INTERFACE
                "$<BUILD_INTERFACE:${ark_plugin_sdk_include_dir}>")
        endif()
        target_link_libraries(ark_${plugin_name}_api INTERFACE ark::core)
        target_link_libraries(ark_sdk INTERFACE ark::${plugin_name})
    endif()
endforeach()

# Propagate API header dependencies, separately from runtime library selection.
foreach(plugin_name IN LISTS ark_plugin_names)
    if(plugin_name IN_LIST ark_colocated_plugin_names)
        set(api_headers ${ark_${plugin_name}_public_headers})
    else()
        file(GLOB_RECURSE api_headers CONFIGURE_DEPENDS "${ROOT_DIR}/include/ark/${plugin_name}/*.hpp" "${ROOT_DIR}/include/ark/${plugin_name}/*.h")
    endif()
    set(api_dependencies)
    foreach(header IN LISTS api_headers)
        file(STRINGS "${header}" includes REGEX "#[ \t]*include[ \t]*[<\"]ark/")
        if(ARK_BUILD_PROTO AND includes MATCHES "ark/proto\\.hpp")
            list(APPEND api_dependencies ark::proto)
        endif()
        foreach(include IN LISTS includes)
            if(include MATCHES "ark/([a-z0-9_]+)/" AND CMAKE_MATCH_1 IN_LIST ark_plugin_names AND NOT CMAKE_MATCH_1 STREQUAL plugin_name)
                list(APPEND api_dependencies "ark::${CMAKE_MATCH_1}")
            endif()
        endforeach()
    endforeach()
    if(api_dependencies)
        list(REMOVE_DUPLICATES api_dependencies)
        target_link_libraries(ark_${plugin_name}_api INTERFACE ${api_dependencies})
    endif()
    string(TOUPPER "${plugin_name}" plugin_option)
    set(plugin_default OFF)
    if(plugin_name MATCHES "^(kernel|net|bus|utility|etcd_client)$")
        set(plugin_default ON)
    endif()
    option(ARK_BUILD_PLUGIN_${plugin_option} "Build the ${plugin_name} plugin" ${plugin_default})
endforeach()

if(ARK_BUILD_APPLICATIONS)
    foreach(plugin_name IN LISTS ark_plugin_names)
        string(TOUPPER "${plugin_name}" plugin_option)
        if(ARK_BUILD_PLUGIN_${plugin_option})
            add_subdirectory("${plugin_name}")
        endif()
    endforeach()
endif()
