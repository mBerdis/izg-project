# Install script for directory: D:/Projects/IZG/izg_project/libs/MealyMachine

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/izgProject")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Projects/IZG/izg_project/build/libs/MealyMachine/Debug/MealyMachined.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Projects/IZG/izg_project/build/libs/MealyMachine/Release/MealyMachine.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Projects/IZG/izg_project/build/libs/MealyMachine/MinSizeRel/MealyMachines.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Projects/IZG/izg_project/build/libs/MealyMachine/RelWithDebInfo/MealyMachinerd.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/MealyMachine" TYPE FILE FILES "D:/Projects/IZG/izg_project/build/libs/MealyMachine/MealyMachine/mealymachine_export.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/MealyMachine" TYPE FILE FILES
    "D:/Projects/IZG/izg_project/libs/MealyMachine/src/MealyMachine/Fwd.h"
    "D:/Projects/IZG/izg_project/libs/MealyMachine/src/MealyMachine/MapTransitionChooser.h"
    "D:/Projects/IZG/izg_project/libs/MealyMachine/src/MealyMachine/MealyMachine.h"
    "D:/Projects/IZG/izg_project/libs/MealyMachine/src/MealyMachine/TransitionChooser.h"
    "D:/Projects/IZG/izg_project/libs/MealyMachine/src/MealyMachine/Exception.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/MealyMachine" TYPE FILE FILES
    "D:/Projects/IZG/izg_project/build/libs/MealyMachine/MealyMachine/MealyMachineConfig.cmake"
    "D:/Projects/IZG/izg_project/build/libs/MealyMachine/MealyMachine/MealyMachineConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/MealyMachine/MealyMachineTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/MealyMachine/MealyMachineTargets.cmake"
         "D:/Projects/IZG/izg_project/build/libs/MealyMachine/CMakeFiles/Export/8a78f3cfc4e16014fa93791fc7915ae5/MealyMachineTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/MealyMachine/MealyMachineTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/MealyMachine/MealyMachineTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/MealyMachine" TYPE FILE FILES "D:/Projects/IZG/izg_project/build/libs/MealyMachine/CMakeFiles/Export/8a78f3cfc4e16014fa93791fc7915ae5/MealyMachineTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/MealyMachine" TYPE FILE FILES "D:/Projects/IZG/izg_project/build/libs/MealyMachine/CMakeFiles/Export/8a78f3cfc4e16014fa93791fc7915ae5/MealyMachineTargets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/MealyMachine" TYPE FILE FILES "D:/Projects/IZG/izg_project/build/libs/MealyMachine/CMakeFiles/Export/8a78f3cfc4e16014fa93791fc7915ae5/MealyMachineTargets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/MealyMachine" TYPE FILE FILES "D:/Projects/IZG/izg_project/build/libs/MealyMachine/CMakeFiles/Export/8a78f3cfc4e16014fa93791fc7915ae5/MealyMachineTargets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/MealyMachine" TYPE FILE FILES "D:/Projects/IZG/izg_project/build/libs/MealyMachine/CMakeFiles/Export/8a78f3cfc4e16014fa93791fc7915ae5/MealyMachineTargets-release.cmake")
  endif()
endif()

