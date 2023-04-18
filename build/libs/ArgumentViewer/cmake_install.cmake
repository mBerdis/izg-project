# Install script for directory: D:/Projects/IZG/izg_project/libs/ArgumentViewer

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
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Projects/IZG/izg_project/build/libs/ArgumentViewer/Debug/ArgumentViewerd.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Projects/IZG/izg_project/build/libs/ArgumentViewer/Release/ArgumentViewer.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Projects/IZG/izg_project/build/libs/ArgumentViewer/MinSizeRel/ArgumentViewers.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/Projects/IZG/izg_project/build/libs/ArgumentViewer/RelWithDebInfo/ArgumentViewerrd.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ArgumentViewer" TYPE FILE FILES "D:/Projects/IZG/izg_project/build/libs/ArgumentViewer/ArgumentViewer/argumentviewer_export.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ArgumentViewer" TYPE FILE FILES
    "D:/Projects/IZG/izg_project/libs/ArgumentViewer/src/ArgumentViewer/ArgumentViewer.h"
    "D:/Projects/IZG/izg_project/libs/ArgumentViewer/src/ArgumentViewer/Fwd.h"
    "D:/Projects/IZG/izg_project/libs/ArgumentViewer/src/ArgumentViewer/Exception.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Devel" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ArgumentViewer" TYPE FILE FILES
    "D:/Projects/IZG/izg_project/build/libs/ArgumentViewer/ArgumentViewer/ArgumentViewerConfig.cmake"
    "D:/Projects/IZG/izg_project/build/libs/ArgumentViewer/ArgumentViewer/ArgumentViewerConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ArgumentViewer/ArgumentViewerTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ArgumentViewer/ArgumentViewerTargets.cmake"
         "D:/Projects/IZG/izg_project/build/libs/ArgumentViewer/CMakeFiles/Export/5dddf4ae052ffa1493c994b9af8669f5/ArgumentViewerTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ArgumentViewer/ArgumentViewerTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/ArgumentViewer/ArgumentViewerTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ArgumentViewer" TYPE FILE FILES "D:/Projects/IZG/izg_project/build/libs/ArgumentViewer/CMakeFiles/Export/5dddf4ae052ffa1493c994b9af8669f5/ArgumentViewerTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ArgumentViewer" TYPE FILE FILES "D:/Projects/IZG/izg_project/build/libs/ArgumentViewer/CMakeFiles/Export/5dddf4ae052ffa1493c994b9af8669f5/ArgumentViewerTargets-debug.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ArgumentViewer" TYPE FILE FILES "D:/Projects/IZG/izg_project/build/libs/ArgumentViewer/CMakeFiles/Export/5dddf4ae052ffa1493c994b9af8669f5/ArgumentViewerTargets-minsizerel.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ArgumentViewer" TYPE FILE FILES "D:/Projects/IZG/izg_project/build/libs/ArgumentViewer/CMakeFiles/Export/5dddf4ae052ffa1493c994b9af8669f5/ArgumentViewerTargets-relwithdebinfo.cmake")
  endif()
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/ArgumentViewer" TYPE FILE FILES "D:/Projects/IZG/izg_project/build/libs/ArgumentViewer/CMakeFiles/Export/5dddf4ae052ffa1493c994b9af8669f5/ArgumentViewerTargets-release.cmake")
  endif()
endif()

