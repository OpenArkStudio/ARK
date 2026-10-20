find_package(spdlog 1.17 CONFIG REQUIRED)
find_package(readerwriterqueue CONFIG REQUIRED)

add_library(ark_core INTERFACE)
add_library(ark::core ALIAS ark_core)
target_compile_features(ark_core INTERFACE cxx_std_20)
target_include_directories(ark_core INTERFACE
    "$<BUILD_INTERFACE:${ROOT_DIR}/include>"
    "${RAPIDXML_INCLUDE_DIR}")
target_link_libraries(ark_core INTERFACE arknet::arknet spdlog::spdlog_header_only
    readerwriterqueue::readerwriterqueue ${CMAKE_DL_LIBS})
target_compile_definitions(ark_core INTERFACE
    $<$<BOOL:${ARK_USE_GUID_32}>:ARK_USE_GUID_32>
    $<$<BOOL:${ARK_DOCKER_ALPINE}>:ARK_DOCKER_ALPINE>
    $<$<BOOL:${ARK_OPEN_FILE_LOG}>:OPEN_FILE_LOG>
    $<$<CONFIG:Debug>:_DEBUG>)
if(MSVC)
    target_compile_options(ark_core INTERFACE /bigobj /utf-8 /Zc:__cplusplus)
    target_compile_definitions(ark_core INTERFACE WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0601)
endif()
