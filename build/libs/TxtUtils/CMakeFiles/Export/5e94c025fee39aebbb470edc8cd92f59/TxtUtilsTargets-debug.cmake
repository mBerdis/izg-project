#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "TxtUtils::TxtUtils" for configuration "Debug"
set_property(TARGET TxtUtils::TxtUtils APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(TxtUtils::TxtUtils PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/TxtUtilsd.lib"
  )

list(APPEND _cmake_import_check_targets TxtUtils::TxtUtils )
list(APPEND _cmake_import_check_files_for_TxtUtils::TxtUtils "${_IMPORT_PREFIX}/lib/TxtUtilsd.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
