find_package(Qt5 COMPONENTS Core Widgets SerialPort REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PNG libpng16 REQUIRED)

add_executable(rtkconv_qt
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

target_link_libraries(rtkconv_qt Qt5::Widgets Qt5::Core Qt5::SerialPort RTKLib ${PNG_LIBRARIES})
target_include_directories(rtkconv_qt PRIVATE ${PNG_INCLUDE_DIRS})
target_link_directories(rtkconv_qt PRIVATE ${PNG_LIBRARY_DIRS})