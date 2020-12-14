find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PNG libpng16 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(rtkpost_qt
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

target_link_libraries(rtkpost_qt Qt5::Widgets Qt5::Core RTKLib ${PNG_LIBRARIES})
target_include_directories(rtkpost_qt PRIVATE ${PNG_INCLUDE_DIRS})
target_link_directories(rtkpost_qt PRIVATE ${PNG_LIBRARY_DIRS})
