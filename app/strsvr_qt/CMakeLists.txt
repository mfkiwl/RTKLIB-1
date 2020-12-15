set(APP strsvr_qt)

set(SOURCES
  convdlg.cpp
  strsvr.cpp
  svrmain.cpp
  svroptdlg.cpp
  ../appcmn_qt/aboutdlg.cpp
  ../appcmn_qt/tcpoptdlg.cpp
  ../appcmn_qt/serioptdlg.cpp
  ../appcmn_qt/cmdoptdlg.cpp
  ../appcmn_qt/console.cpp
  ../appcmn_qt/fileoptdlg.cpp
  ../appcmn_qt/ftpoptdlg.cpp
  ../appcmn_qt/refdlg.cpp
  ../appcmn_qt/keydlg.cpp

  convdlg.ui
  svrmain.ui
  svroptdlg.ui
  ../appcmn_qt/tcpoptdlg.ui
  ../appcmn_qt/serioptdlg.ui
  ../appcmn_qt/aboutdlg.ui
  ../appcmn_qt/cmdoptdlg.ui
  ../appcmn_qt/console.ui
  ../appcmn_qt/fileoptdlg.ui
  ../appcmn_qt/ftpoptdlg.ui
  ../appcmn_qt/refdlg.ui
  ../appcmn_qt/keydlg.ui

  strsvr_qt.qrc
)

include(../qtApp.cmake)
