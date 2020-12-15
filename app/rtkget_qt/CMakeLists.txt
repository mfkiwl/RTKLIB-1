set(APP rtkget_qt)

set(SOURCES
  main.cpp
  getmain.cpp
  getoptdlg.cpp
  staoptdlg.cpp
  ../appcmn_qt/aboutdlg.cpp
  ../appcmn_qt/keydlg.cpp
  ../appcmn_qt/viewer.cpp
  ../appcmn_qt/vieweropt.cpp
  ../appcmn_qt/timedlg.cpp

  getmain.ui
  getoptdlg.ui
  staoptdlg.ui
  ../appcmn_qt/aboutdlg.ui
  ../appcmn_qt/keydlg.ui
  ../appcmn_qt/viewer.ui
  ../appcmn_qt/vieweropt.ui
  ../appcmn_qt/timedlg.ui

  rtkget_qt.qrc
)

include(../qtApp.cmake)