#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "MealyMachine::MealyMachine" for configuration "Debug"
set_property(TARGET MealyMachine::MealyMachine APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(MealyMachine::MealyMachine PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/MealyMachined.lib"
  )

list(APPEND _cmake_import_check_targets MealyMachine::MealyMachine )
list(APPEND _cmake_import_check_files_for_MealyMachine::MealyMachine "${_IMPORT_PREFIX}/lib/MealyMachined.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
