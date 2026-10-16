find_package(tomlplusplus 3.4 CONFIG QUIET)
if(NOT TARGET tomlplusplus::tomlplusplus)
    include(FetchContent)
    FetchContent_Declare(tomlplusplus
        GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
        GIT_TAG 30172438cee64926dc41fdd9c11fb3ba5b2ba9de
        GIT_SHALLOW FALSE)
    FetchContent_MakeAvailable(tomlplusplus)
endif()

add_library(ark_config STATIC application_manifest.cpp)
add_library(ark::config ALIAS ark_config)
target_compile_features(ark_config PUBLIC cxx_std_20)
target_include_directories(ark_config PUBLIC "$<BUILD_INTERFACE:${ROOT_DIR}/include>")
target_include_directories(ark_config PRIVATE "${RAPIDXML_INCLUDE_DIR}")
target_link_libraries(ark_config PRIVATE tomlplusplus::tomlplusplus)
