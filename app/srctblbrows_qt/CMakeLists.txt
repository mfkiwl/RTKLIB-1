find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PNG libpng16 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(srctblbrows_qt
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

target_link_libraries(srctblbrows_qt Qt5::Widgets Qt5::Core RTKLib ${PNG_LIBRARIES})
target_include_directories(srctblbrows_qt PRIVATE ${PNG_INCLUDE_DIRS})
target_link_directories(srctblbrows_qt PRIVATE ${PNG_LIBRARY_DIRS})
