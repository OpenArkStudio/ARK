include_guard(GLOBAL)

# The compiled reader can be configured on its own. Make a newly discovered
# imported parser visible to sibling private consumers as well.
if(NOT TARGET tomlplusplus::tomlplusplus)
    find_package(tomlplusplus 3.4 CONFIG QUIET)
    if(TARGET tomlplusplus::tomlplusplus)
        set_property(TARGET tomlplusplus::tomlplusplus PROPERTY IMPORTED_GLOBAL TRUE)
    endif()
endif()
if(NOT TARGET tomlplusplus::tomlplusplus)
    include(FetchContent)
    FetchContent_Declare(tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG 30172438cee64926dc41fdd9c11fb3ba5b2ba9de
        GIT_SHALLOW FALSE)
    FetchContent_GetProperties(tomlplusplus)
    if(NOT tomlplusplus_POPULATED)
        FetchContent_Populate(tomlplusplus)
        add_subdirectory("${tomlplusplus_SOURCE_DIR}" "${tomlplusplus_BINARY_DIR}" EXCLUDE_FROM_ALL)
    endif()
endif()
