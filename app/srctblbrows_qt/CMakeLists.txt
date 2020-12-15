set(APP srctblbrows_qt)

set(SOURCES
  browsmain.cpp
  srctblbrows.cpp
  staoptdlg.cpp
  ../appcmn_qt/aboutdlg.cpp
  ../appcmn_qt/gmview.cpp

  browsmain.ui
  staoptdlg.ui
  ../appcmn_qt/aboutdlg.ui
  ../appcmn_qt/gmview.ui

  srctblbrows_qt.qrc
)

include(../qtApp.cmake)