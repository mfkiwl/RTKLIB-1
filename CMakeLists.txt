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
    add_definitions(-DQT5)
    #Occasionally libstdc++ must be explicitly specified
    set(CXX_BASE_LIBS stdc++)
  else()
    message(FATAL_ERROR "No CXX support - Can't build QT Applications")
  endif()
endif(BUILD_QT_APPS)


option(ENAGLO "GLONASS Support" ON)
if(ENAGLO)
  add_definitions(-DENAGLO)
endif(ENAGLO)

option(ENAQZS "QZSS Support" ON)
if(ENAQZS)
  add_definitions(-DENAQZS)
endif()

option(ENAGAL "Galileo Support" ON)
if(ENAGAL)
  add_definitions(-DENAGAL)
endif(ENAGAL)

option(ENACMP "BeiDou/Compass Support" ON)
if(ENACMP)
  add_definitions(-DENACMP)
endif(ENACMP)

option(ENAIRN "IRNSS Support" ON)
if(ENAIRN)
  add_definitions(-DENAIRN)
endif(ENAIRN)

option(ENALEO "LEO Support" ON)
if(ENALEO)
  add_definitions(-DENALEO)
endif(ENALEO)

option(OUTSTAT_AMB "PPP : Output ambiguity parameters to solution status" ON)
if(OUTSTAT_AMB)
  add_definitions(-DOUTSTAT_AMB)
endif(OUTSTAT_AMB)

option(IERS_MODEL "Use IERS GMF instead of NMF (Requires gfortran)" ON)
if(IERS_MODEL)
  add_definitions(-DIERS_MODEL)
  enable_language(Fortran)
endif(IERS_MODEL)

option(EXTLEX "QZSS LEX Support" ON)
if(EXTLEX)
  add_definitions(-DEXTLEX)
endif(EXTLEX)

option(MKL "Intel Math Kernel Library (MKL) Support" OFF)
if(MKL)
  add_definitions(-DMKL)
endif(MKL)

option(LAPACK "Use LAPACK/BLAS" OFF)
if(LAPACK)
  add_definitions(-DLAPACK)
  find_package(BLAS)
  set(C_BASE_LIBS ${BLAS_LIBRARIES})
endif(LAPACK)

option(TRACE "Tracing Support" ON)
if(TRACE)
  add_definitions(-DTRACE)
endif(TRACE)

option(SHARED "Shared library compatibility" ON)
if(SHARED)
  add_definitions(-DSHARED)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif(SHARED)

option(DWIN32 "Use WIN32 API" OFF)
if(DWIN32)
  add_definitions(-DWIN32)
endif(DWIN32)

option(WIN_DLL "Generate library as Windows DLL" OFF)
if(WIN_DLL)
  add_definitions(-DWINDLL)
endif(WIN_DLL)

option(NOCALLOC "Don't use calloc for zero matrix" OFF)
if(NOCALLOC)
  add_definitions(-DNOCALLOC)
endif(NOCALLOC)

option(CPUTIME_IN_GPST "cputime operated in gpst" OFF)
if(CPUTIME_IN_GPST)
  add_definitions(-DCPUTIME_IN_GPST)
endif(CPUTIME_IN_GPST)

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
list(APPEND C_BASE_LIBS ${CMAKE_THREAD_LIBS_INIT})

# Check math funcs available, or link to libm
include(CheckLibraryExists)
message(STATUS "Checking whether libm is required for math")
CHECK_LIBRARY_EXISTS(m sin "" HAVE_LIB_M)
if (HAVE_LIB_M)
    list(APPEND C_BASE_LIBS m)
endif (HAVE_LIB_M)

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