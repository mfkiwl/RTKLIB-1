cmake_minimum_required(VERSION 3.19)
project(rtklib VERSION 0.0.0)
project(C Fortran)

#Default build config
#option(BUILD_QT_APPS "Build QT GUI Apps" ON)
set(BUILD_QT_APPS ON)

set(CMAKE_C_FLAGS "-Wall -O3")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Define some TCP options on macos
add_definitions( -D_DARWIN_C_SOURCE=1 -DSHARED)

#RTKLIB Build Options
add_definitions(  -DSHARED
                  -DTRACE
                  -DENAGLO
                  -DENAQZS
                  -DENAGAL
                  -DENACMP
                  -DENAIRN
                  -DNFREQ=3
                  -DSVR_REUSEADDR
                  #-DIERS_MODEL
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

#Build cli/qt apps
add_subdirectory(app)

#Install
include(GNUInstallDirs)
include(InstallRequiredSystemLibraries)

#Install CLI apps directly
install(TARGETS ${cli_apps}
  RUNTIME DESTINATION bin
)


set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RTKLIB")
set(CPACK_PACKAGE_VENDOR "RTKLIB")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
include(CPack)