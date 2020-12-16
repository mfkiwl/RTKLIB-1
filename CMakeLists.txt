cmake_minimum_required(VERSION 3.19)

set(VERSION_MAJOR 2)
set(VERSION_MINOR 4)
set(VERSION_PATCH 3)

set(REL_TYPE "dev")
project(rtklib VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
add_definitions(-DVER_RTKLIB_EXT="${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
add_definitions(-DPATCH_LEVEL_EXT="${REL_TYPE}")

if(BUILD_QT_APPS)
  project(RTKLIB C CXX)
else(BUILD_QT_APPS)
  project(RTKLIB C)
endif(BUILD_QT_APPS)

#Configurable Options
option(BUILD_QT_APPS "Build QT GUI applications" OFF)

option(D_ENAGLO "GLONASS Support" ON)
if(D_ENGALO)
  add_definitions(-DENAGLO)
endif()

option(D_ENAQZS "QZSS Support" ON)
if(D_ENAQZS)
  add_definitions(-DENAQZS)
endif()

option(D_ENAGAL "Galileo Support" ON)
if(D_ENAGAL)
  add_definitions(-DENAGAL)
endif()

option(D_ENACMP "BeiDou/Compass Support" ON)
if(D_ENACMP)
  add_definitions(-DENACMP)
endif()

option(D_ENAIRN "IRNSS Support" ON)
if(D_ENAIRN)
  add_definitions(-DENAIRN)
endif()

option(D_OUTSTAT_AMB "PPP : Output ambiguity parameters to solution status" OFF)

option(D_IERS_MODEL "Use IERS GMF instead of NMF (Requires gfortran)" ON)
if(D_IERS_MODEL)
  add_definitions(-DIERS_MODEL)
  enable_language(Fortran)
endif()

option(D_EXTLEX "QZSS LEX Support" ON)
if(D_EXTLEX)
  add_definitions(-DEXTLEX)
endif()

option(D_MKL "Intel Math Kernel Library (MKL) Support" OFF)
if(D_MKL)
  add_definitions(-DMKL)
endif()

option(D_LAPACK "Use LAPACK/BLAS" OFF)

option(D_TRACE "Tracing Support" ON)
if(D_TRACE)
  add_definitions(-DTRACE)
endif()


option(D_SHARED "Shared library compatibility" ON)
if(D_SHARED)
  add_definitions(-DSHARED)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

option(D_DWIN32 "Use WIN32 API" OFF)
if(D_DWIN32)
  add_definitions(-DWIN32)
endif()

option(D_WIN_DLL "Generate library as Windows DLL" OFF)
if(D_WIN_DLL)
  add_definitions(-DWINDLL)
endif()

option(D_NOCALLOC "Don't use calloc for zero matrix" OFF)
if(D_NOCALLOC)
  add_definitions(-DNOCALLOC)
endif()

option(D_CPUTIME_IN_GPST "cputime operated in gpst" OFF)
if(D_CPUTIME_IN_GPST)
  add_definitions(-DCPUTIME_IN_GPST)
endif()

add_definitions(-DNFREQ=3)    # Frequencies supported
add_definitions(-DNEXOBS=0)   # Number of extended obs codes
add_definitions(-DMAXOBS=128) # Max number of observations

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Define some TCP options on macos
add_definitions( -D_DARWIN_C_SOURCE=1)

#Common build settings
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