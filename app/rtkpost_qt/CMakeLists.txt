set(APP rtkpost_qt)

set(SOURCES
  extopt.cpp
  kmzconv.cpp
  postmain.cpp
  postopt.cpp
  rtkpost.cpp
  ../appcmn_qt/aboutdlg.cpp
  ../appcmn_qt/keydlg.cpp
  ../appcmn_qt/maskoptdlg.cpp
  ../appcmn_qt/refdlg.cpp
  ../appcmn_qt/viewer.cpp
  ../appcmn_qt/vieweropt.cpp
  ../appcmn_qt/timedlg.cpp
  ../appcmn_qt/utils.cpp

  extopt.ui
  kmzconv.ui
  postmain.ui
  postopt.ui
  ../appcmn_qt/keydlg.ui
  ../appcmn_qt/maskoptdlg.ui
  ../appcmn_qt/refdlg.ui
  ../appcmn_qt/viewer.ui
  ../appcmn_qt/vieweropt.ui
  ../appcmn_qt/aboutdlg.ui
  ../appcmn_qt/timedlg.ui

  rtkpost_qt.qrc
)

include(../qtApp.cmake)