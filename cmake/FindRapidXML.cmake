# RapidXML is header-only and does not consistently provide a CMake config.
include(FindPackageHandleStandardArgs)
if(RAPIDXML_INCLUDE_DIR AND NOT RapidXML_INCLUDE_DIR)
    set(RapidXML_INCLUDE_DIR "${RAPIDXML_INCLUDE_DIR}")
endif()
find_path(RapidXML_INCLUDE_DIR rapidxml.hpp PATH_SUFFIXES rapidxml)
find_package_handle_standard_args(RapidXML REQUIRED_VARS RapidXML_INCLUDE_DIR)
if(RapidXML_FOUND AND NOT TARGET RapidXML::RapidXML)
    add_library(RapidXML::RapidXML INTERFACE IMPORTED)
    set_target_properties(RapidXML::RapidXML PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${RapidXML_INCLUDE_DIR}")
endif()
mark_as_advanced(RapidXML_INCLUDE_DIR)
