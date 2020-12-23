add_library(RTKLib_base OBJECT
  convgpx.c convkml.c convrnx.c
  datum.c download.c
  ephemeris.c erb.c
  geoid.c gis.c
  ionex.c
  lambda.c
  options.c
  pntpos.c postpos.c ppp.c ppp_ar.c ppp_corr.c preceph.c
  qzslex.c rcvraw.c rinex.c rtcm.c rtcm2.c rtcm3.c rtcm3e.c rtkcmn.c rtkpos.c rtksvr.c
  sbas.c solution.c stream.c streamsvr.c
  tides.c tle.c)

add_library(RTKLib_status_output_dummy OBJECT status_output_dummy.c)

add_subdirectory(rcv)
add_subdirectory(math)
add_subdirectory(extensions)

set (LIBRTKLIB_OBJ
  $<TARGET_OBJECTS:RTKLib_base>
  $<TARGET_OBJECTS:RTKLib_math>
  $<TARGET_OBJECTS:RTKLib_math_lsq>
  $<TARGET_OBJECTS:RTKLib_rcv>
  $<TARGET_OBJECTS:RTKLib_tdiff_phases>
  $<TARGET_OBJECTS:RTKLib_glo_ifb>
  $<TARGET_OBJECTS:RTKLib_iers>
  $<TARGET_OBJECTS:RTKLIB_sar_rlm>
)

#This contains dummy function implementations for writing to the
#console in QT applications

add_library(RTKLib SHARED $<TARGET_OBJECTS:RTKLib_status_output_dummy> ${LIBRTKLIB_OBJ})
add_library(RTKLib_qt STATIC ${LIBRTKLIB_OBJ})

set_target_properties(RTKLib PROPERTIES PUBLIC_HEADER "rtklib.h")

install(TARGETS RTKLib
  BUNDLE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  COMPONENT librtklib
)