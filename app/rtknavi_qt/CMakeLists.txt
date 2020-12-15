set(APP rtknavi_qt)

set(SOURCES
  ../appcmn_qt/aboutdlg.cpp
  ../appcmn_qt/cmdoptdlg.cpp
  ../appcmn_qt/fileoptdlg.cpp
  ../appcmn_qt/ftpoptdlg.cpp
  ../appcmn_qt/keydlg.cpp
  ../appcmn_qt/maskoptdlg.cpp
  ../appcmn_qt/refdlg.cpp
  ../appcmn_qt/serioptdlg.cpp
  ../appcmn_qt/tcpoptdlg.cpp
  ../appcmn_qt/viewer.cpp
  ../appcmn_qt/vieweropt.cpp
  instrdlg.cpp
  logstrdlg.cpp
  main.cpp
  mondlg.cpp
  navimain.cpp
  naviopt.cpp
  outstrdlg.cpp
  rcvoptdlg.cpp
  markdlg.cpp
  ../appcmn_qt/graph.cpp

  instrdlg.ui
  logstrdlg.ui
  navimain.ui
  naviopt.ui
  mondlg.ui
  outstrdlg.ui
  rcvoptdlg.ui
  ../appcmn_qt/aboutdlg.ui
  ../appcmn_qt/cmdoptdlg.ui
  ../appcmn_qt/fileoptdlg.ui
  ../appcmn_qt/ftpoptdlg.ui
  ../appcmn_qt/keydlg.ui
  ../appcmn_qt/maskoptdlg.ui
  ../appcmn_qt/refdlg.ui
  ../appcmn_qt/serioptdlg.ui
  ../appcmn_qt/viewer.ui
  ../appcmn_qt/vieweropt.ui
  ../appcmn_qt/tcpoptdlg.ui
  markdlg.ui

  rtknavi_qt.qrc

)

include(../qtApp.cmake)