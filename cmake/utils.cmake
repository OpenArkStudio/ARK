# Get ARK version from frame/version.h and put it in ARK_VERSION
function(ark_extract_version)
	file(READ "${CMAKE_CURRENT_LIST_DIR}/frame/version.h" file_contents)
	string(REGEX MATCH "ARK_VER_MAJOR ([0-9]+)" _  "${file_contents}")
	if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract major version number from ARK/frame/version.h")
	endif()
	set(ver_major ${CMAKE_MATCH_1})

	string(REGEX MATCH "ARK_VER_MINOR ([0-9]+)" _  "${file_contents}")
	if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract minor version number from ARK/frame/version.h")
	endif()

	set(ver_minor ${CMAKE_MATCH_1})
	string(REGEX MATCH "ARK_VER_PATCH ([0-9]+)" _  "${file_contents}")
	if(NOT CMAKE_MATCH_COUNT EQUAL 1)
        message(FATAL_ERROR "Could not extract patch version number from ARK/frame/version.h")
	endif()
	set(ver_patch ${CMAKE_MATCH_1})

	set (ARK_VERSION "${ver_major}.${ver_minor}.${ver_patch}" PARENT_SCOPE)
endfunction()