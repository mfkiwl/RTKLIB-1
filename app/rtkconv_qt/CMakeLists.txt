set(APP rtkconv_qt)

set(SOURCES
  codeopt.cpp
  convmain.cpp
  convopt.cpp
  rtkconv.cpp
  startdlg.cpp
  ../appcmn_qt/aboutdlg.cpp
  ../appcmn_qt/keydlg.cpp
  ../appcmn_qt/timedlg.cpp
  ../appcmn_qt/viewer.cpp
  ../appcmn_qt/vieweropt.cpp

  codeopt.ui
  convopt.ui
  startdlg.ui
  convmain.ui
  ../appcmn_qt/aboutdlg.ui
  ../appcmn_qt/keydlg.ui
  ../appcmn_qt/timedlg.ui
  ../appcmn_qt/viewer.ui
  ../appcmn_qt/vieweropt.ui

  rtkconv_qt.qrc
)

include(../qtApp.cmake)