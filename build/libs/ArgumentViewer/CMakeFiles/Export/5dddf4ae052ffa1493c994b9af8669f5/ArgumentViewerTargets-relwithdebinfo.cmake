#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "ArgumentViewer::ArgumentViewer" for configuration "RelWithDebInfo"
set_property(TARGET ArgumentViewer::ArgumentViewer APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(ArgumentViewer::ArgumentViewer PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/ArgumentViewerrd.lib"
  )

list(APPEND _cmake_import_check_targets ArgumentViewer::ArgumentViewer )
list(APPEND _cmake_import_check_files_for_ArgumentViewer::ArgumentViewer "${_IMPORT_PREFIX}/lib/ArgumentViewerrd.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
