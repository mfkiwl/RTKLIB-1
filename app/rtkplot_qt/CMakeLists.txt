find_package(Qt5 COMPONENTS Core Widgets SerialPort REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PNG libpng16 REQUIRED)

add_executable(rtkplot_qt
  ../appcmn_qt/aboutdlg.cpp
  conndlg.cpp
  geview.cpp
  mapdlg.cpp
  plotcmn.cpp
  plotdata.cpp
  plotdraw.cpp
  plotinfo.cpp
  plotmain.cpp
  plotopt.cpp
  pntdlg.cpp
  rtkplot.cpp
  satdlg.cpp
  skydlg.cpp
  ../appcmn_qt/refdlg.cpp
  ../appcmn_qt/viewer.cpp
  ../appcmn_qt/vieweropt.cpp
  ../appcmn_qt/cmdoptdlg.cpp
  ../appcmn_qt/fileoptdlg.cpp
  ../appcmn_qt/serioptdlg.cpp
  ../appcmn_qt/tcpoptdlg.cpp
  ../appcmn_qt/keydlg.cpp
  ../appcmn_qt/graph.cpp
  ../appcmn_qt/console.cpp
  ../appcmn_qt/tspandlg.cpp
  fileseldlg.cpp
  ../appcmn_qt/gmview.cpp
  vmapdlg.cpp

  ../appcmn_qt/aboutdlg.ui
  conndlg.ui
  geview.ui
  mapdlg.ui
  plotmain.ui
  plotopt.ui
  pntdlg.ui
  satdlg.ui
  skydlg.ui
  ../appcmn_qt/refdlg.ui
  ../appcmn_qt/viewer.ui
  ../appcmn_qt/vieweropt.ui
  ../appcmn_qt/cmdoptdlg.ui
  ../appcmn_qt/fileoptdlg.ui
  ../appcmn_qt/serioptdlg.ui
  ../appcmn_qt/tcpoptdlg.ui
  ../appcmn_qt/keydlg.ui
  ../appcmn_qt/console.ui
  ../appcmn_qt/tspandlg.ui
  fileseldlg.ui
  ../appcmn_qt/gmview.ui
  vmapdlg.ui

  rtkplot_qt.qrc

)

target_link_libraries(rtkplot_qt Qt5::Widgets Qt5::Core Qt5::SerialPort RTKLib ${PNG_LIBRARIES})
target_include_directories(rtkplot_qt PRIVATE ${PNG_INCLUDE_DIRS})
target_link_directories(rtkplot_qt PRIVATE ${PNG_LIBRARY_DIRS})
