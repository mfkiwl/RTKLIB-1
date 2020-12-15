cmake_minimum_required(VERSION 3.19)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

project(rtklib VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

#Default build config
option(BUILD_QT_APPS "Build QT GUI applications" OFF)

if(BUILD_QT_APPS)
  project(RTKLIB C CXX Fortran)
else(BUILD_QT_APPS)
  project(RTKLIB C Fortran)
endif(BUILD_QT_APPS)

set(CMAKE_C_FLAGS "-Wall -O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Define some TCP options on macos
add_definitions( -D_DARWIN_C_SOURCE=1)

#RTKLIB Build Options
add_definitions(-DSHARED
                -DTRACE
                -DENAGLO
                -DENAQZS
                -DENAGAL
                -DENACMP
                -DENAIRN
                -DNFREQ=3
                -DSVR_REUSEADDR
                -DIERS_MODEL
                -DEXTLEX
                -DNEXOBS=2
                -DMAXOBS=128
)

#Shared components
#Common build settings
include_directories(
  src
  src/extensions/glo_ifb
  src/extensions/tdiff_phases
  src/math
  src/math/lsq
)

set(cli_apps
    convbin
    pos2kml
    rnx2rtkp
    rtkrcv
    str2str
)

add_subdirectory(lib)
add_subdirectory(src)

#QT Common Settings
if(BUILD_QT_APPS)
  set(qt_apps
    rtkconv_qt
    rtkget_qt
    rtklaunch_qt
    rtknavi_qt
    rtkplot_qt
    rtkpost_qt
    strsvr_qt
  )
endif(BUILD_QT_APPS)

#Build all executables
add_subdirectory(app)

## Install Configuration

## General CPACK Options
set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}//app/rtknavi_qt/rtknavi_Icon.ico")
set(CPACK_PACKAGE_VENDOR "RTKLIB")
set(CPACK_PACKAGE_NAME "RTKLIB")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RTKLIB")

set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

include(GNUInstallDirs)
include(InstallRequiredSystemLibraries)

## CPACK Config
set(CPACK_COMPONENTS_ALL librtklib cli_apps)

## Set generators depending on host
set(CPACK_GENERATOR "TBZ2" )

if(${APPLE})
  set(CPACK_GENERATOR "DragNDrop")
endif(${APPLE})

## Source
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_src")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /.*build.*
    /\\\\.DS_Store
)

## Install/Packaging Steps
#Permit install of CLI executables directly on unix systems
install(TARGETS ${cli_apps}
  BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT cli_apps
)

#Only package the QT apps if built
if(BUILD_QT_APPS)
  list(APPEND CPACK_COMPONENTS_ALL qt_apps)
endif(BUILD_QT_APPS)

include(CPack)

