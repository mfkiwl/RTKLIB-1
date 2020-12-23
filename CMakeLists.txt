cmake_minimum_required(VERSION 3.17)
include(CheckLanguage)
include(CMakeDependentOption)
find_package(Git REQUIRED)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules/")
include(GetGitRevisionDescription)

###VERSIONING
#############

project(RTKLIB
  VERSION 0.0.0.0
  LANGUAGES C
)

if (NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type selected, defaulting to \"Release\"")
  set(CMAKE_BUILD_TYPE "Release")
endif()

get_git_head_revision(GIT_REFSPEC GIT_SHA1)
git_local_changes(GIT_STATE)

# TODO: Use consistnet versioning/release type fields
set(PATCH_LEVEL_EXT ${CMAKE_BUILD_TYPE})
set(VER_RTKLIB_EXT "${GIT_SHA1}-${GIT_STATE}")
set(CPACK_PACKAGE_VERSION "${GIT_SHA1}-${GIT_STATE}-${CMAKE_BUILD_TYPE}")

#Only allow versioning if the repo state is clean
option(USE_TAG_VERSION "Use tag at current git head to set version" OFF)
if(USE_TAG_VERSION)
  if(GIT_STATE EQUAL "CLEAN")
    git_get_exact_tag(GIT_TAG)
    #Split the version string into groups
    string(REGEX MATCH "([^\.v-]+)" ${GIT_TAG_MATCH} ${GIT_TAG})
    set(VERSION_MAJOR ${CMAKE_MATCH_1})
    set(VERSION_MINOR ${CMAKE_MATCH_2})
    set(VERSION_PATCH ${CMAKE_MATCH_3})
    set(VERSION_TWEAK ${CMAKE_MATCH_4})
    set(VER_RTKLIB_EXT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}.${VERSION_TWEAK}")
    set(CPACK_PACKAGE_VERSION ${CMAKE_VERSION})

  else()
    message( FATAL_ERROR "Can't make tagged release - repo is dirty")
  endif(GIT_STATE)
endif(USE_TAG_VERSION)

add_definitions(-DVER_RTKLIB_EXT="${VER_RTKLIB_EXT}")
add_definitions(-DPATCH_LEVEL_EXT="${PATCH_LEVEL_EXT}")

message(STATUS "Building: ${VER_RTKLIB_EXT}-${PATCH_LEVEL_EXT}")

### CONFIG
##########

option(BUILD_QT_APPS "Build QT GUI applications" OFF)
if(BUILD_QT_APPS)
  check_language(CXX)
  if(CMAKE_CXX_COMPILER)
    enable_language(CXX)
    SET(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    add_definitions(-DQT)
  else()
    message(FATAL_ERROR "No CXX support - Can't build QT Applications")
  endif()
endif(BUILD_QT_APPS)


option(D_ENAGLO "GLONASS Support" ON)
if(D_ENAGLO)
  add_definitions(-DENAGLO)
endif(D_ENAGLO)

option(D_ENAQZS "QZSS Support" ON)
if(D_ENAQZS)
  add_definitions(-DENAQZS)
endif()

option(D_ENAGAL "Galileo Support" ON)
if(D_ENAGAL)
  add_definitions(-DENAGAL)
endif(D_ENAGAL)

option(D_ENACMP "BeiDou/Compass Support" ON)
if(D_ENACMP)
  add_definitions(-DENACMP)
endif(D_ENACMP)

option(D_ENAIRN "IRNSS Support" ON)
if(D_ENAIRN)
  add_definitions(-DENAIRN)
endif(D_ENAIRN)

option(D_ENALEO "LEO Support" ON)
if(D_ENALEO)
  add_definitions(-DENALEO)
endif(D_ENALEO)

option(D_OUTSTAT_AMB "PPP : Output ambiguity parameters to solution status" ON)
if(D_OUTSTAT_AMB)
  add_definitions(-DOUTSTAT_AMB)
endif(D_OUTSTAT_AMB)

option(D_IERS_MODEL "Use IERS GMF instead of NMF (Requires gfortran)" ON)
if(D_IERS_MODEL)
  add_definitions(-DIERS_MODEL)
  enable_language(Fortran)
endif(D_IERS_MODEL)

option(D_EXTLEX "QZSS LEX Support" ON)
if(D_EXTLEX)
  add_definitions(-DEXTLEX)
endif(D_EXTLEX)

option(D_MKL "Intel Math Kernel Library (MKL) Support" OFF)
if(D_MKL)
  add_definitions(-DMKL)
endif(D_MKL)

option(D_LAPACK "Use LAPACK/BLAS" OFF)
if(D_LAPACK)
  add_definitions(-DLAPACK)
endif(D_LAPACK)

option(D_TRACE "Tracing Support" ON)
if(D_TRACE)
  add_definitions(-DTRACE)
endif(D_TRACE)

option(D_SHARED "Shared library compatibility" ON)
if(D_SHARED)
  add_definitions(-DSHARED)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif(D_SHARED)

option(D_DWIN32 "Use WIN32 API" OFF)
if(D_DWIN32)
  add_definitions(-DWIN32)
endif(D_DWIN32)

option(D_WIN_DLL "Generate library as Windows DLL" OFF)
if(D_WIN_DLL)
  add_definitions(-DWINDLL)
endif(D_WIN_DLL)

option(D_NOCALLOC "Don't use calloc for zero matrix" OFF)
if(D_NOCALLOC)
  add_definitions(-DNOCALLOC)
endif(D_NOCALLOC)

option(D_CPUTIME_IN_GPST "cputime operated in gpst" OFF)
if(D_CPUTIME_IN_GPST)
  add_definitions(-DCPUTIME_IN_GPST)
endif(D_CPUTIME_IN_GPST)

# CAUTION: The codebase currently contains hardcoded expectations
# regarding number of frequencies. These settings are the defaults
# in rtklib.h - You have been warned.
add_definitions(-DNFREQ=3)    # Frequencies supported
# This is set to 1 to handle some hard coded assumption in the LEX code
add_definitions(-DNEXOBS=1)   # Number of extended obs codes
add_definitions(-DMAXOBS=64) # Max number of observations

### COMPILE & LINK
##################

set(CMAKE_C_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#Threads isn't always found
set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_WIN32_THREADS_INIT 0)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Check for pthreads support
find_package(Threads REQUIRED)

# Check math funcs available, or link to libm
include(CheckLibraryExists)
message(STATUS "Checking whether libm is required for math")
CHECK_LIBRARY_EXISTS(m sin "" HAVE_LIB_M)
if (HAVE_LIB_M)
    set(EXTRA_LIBS ${EXTRA_LIBS} m)
endif (HAVE_LIB_M)

#Occasionally libstdc++ must be explicitly specified
set(EXTRA_LIBS ${EXTRA_LIBS} stdc++)

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


### PACKAGE
##################

#Add install text to binary packages
install(PROGRAMS "${CMAKE_SOURCE_DIR}/CI_INSTALL.md"  DESTINATION "." COMPONENT bin_archive)

#Copy install text
SET(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
SET(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
SET(CPACK_COMPONENTS_ALL cli_apps qt_apps librtklib bin_archive)


set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_GENERATOR "TBZ2")

set(CPACK_SOURCE_IGNORE_FILExS
  .git/
  build/
)

include(CPack)