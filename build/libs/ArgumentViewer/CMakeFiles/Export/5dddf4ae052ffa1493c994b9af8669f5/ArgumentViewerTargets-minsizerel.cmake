#----------------------------------------------------------------
# Generated CMake target import file for configuration "MinSizeRel".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ArgumentViewer::ArgumentViewer" for configuration "MinSizeRel"
set_property(TARGET ArgumentViewer::ArgumentViewer APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)
set_target_properties(ArgumentViewer::ArgumentViewer PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_MINSIZEREL "CXX"
  IMPORTED_LOCATION_MINSIZEREL "${_IMPORT_PREFIX}/lib/ArgumentViewers.lib"
  )

list(APPEND _cmake_import_check_targets ArgumentViewer::ArgumentViewer )
list(APPEND _cmake_import_check_files_for_ArgumentViewer::ArgumentViewer "${_IMPORT_PREFIX}/lib/ArgumentViewers.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
