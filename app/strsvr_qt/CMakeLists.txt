find_package(Qt5 COMPONENTS Core Widgets SerialPort REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PNG libpng16 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(strsvr_qt
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

target_link_libraries(strsvr_qt Qt5::Widgets Qt5::Core Qt5::SerialPort RTKLib ${PNG_LIBRARIES})
target_include_directories(strsvr_qt PRIVATE ${PNG_INCLUDE_DIRS})
target_link_directories(strsvr_qt PRIVATE ${PNG_LIBRARY_DIRS})
