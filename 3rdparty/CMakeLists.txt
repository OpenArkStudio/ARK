message(STATUS "Add 3rdparty projects...")

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Set dependencies as release mode" FORCE)

# zephyr
add_subdirectory(zephyr)

#------------------------------------------------------------------------------
# set output folder
#------------------------------------------------------------------------------
#set(THIRD_PARTY_TARGET some_project_name)


#foreach (target_index ${THIRD_PARTY_TARGET})
#    set_target_properties(${target_index} PROPERTIES
#            FOLDER "3rdparty"
#            ARCHIVE_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
#            RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR}
#            LIBRARY_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})
#endforeach (target_index)
