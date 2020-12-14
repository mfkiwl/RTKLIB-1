find_package(Qt5 COMPONENTS Core Widgets SerialPort REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PNG libpng16 REQUIRED)

add_executable(rtkget_qt
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

target_link_libraries(rtkget_qt Qt5::Widgets Qt5::Core RTKLib ${PNG_LIBRARIES})
target_include_directories(rtkget_qt PRIVATE ${PNG_INCLUDE_DIRS})
target_link_directories(rtkget_qt PRIVATE ${PNG_LIBRARY_DIRS})