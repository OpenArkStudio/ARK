include_guard(GLOBAL)

# Discover the parser at the project root so sibling consumers can use its target.
find_package(tomlplusplus 3.4 CONFIG QUIET)
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
