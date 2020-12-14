cmake_minimum_required(VERSION 3.19)
project(rtklib VERSION 0.0.0)
project(C Fortran)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -O3")

# Define some TCP options on macos
add_definitions( -D_DARWIN_C_SOURCE=1 -DSHARED)

#Internal definitions to support generating ashared library
add_definitions(-DSHARED)

#RTKLIB Build Options
add_definitions(  -DTRACE
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


include_directories(
  src
  src/extensions/glo_ifb
  src/extensions/tdiff_phases
  src/math
  src/math/lsq
)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(app)

#Install
include(GNUInstallDirs)

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "RTKLIB")
set(CPACK_PACKAGE_VENDOR "RTKLIB")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
include(CPack)