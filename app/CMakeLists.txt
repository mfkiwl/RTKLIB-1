#cli apps
add_subdirectory(convbin)
add_subdirectory(pos2kml)
add_subdirectory(rtkrcv)
add_subdirectory(rnx2rtkp)
add_subdirectory(str2str)

if(BUILD_QT_APPS)

  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)
  set(CMAKE_AUTORCC ON)

  add_subdirectory(rtkconv_qt)
  add_subdirectory(rtkget_qt)
  add_subdirectory(rtklaunch_qt)
  add_subdirectory(rtknavi_qt)
  add_subdirectory(rtkplot_qt)
  add_subdirectory(rtkpost_qt)
  add_subdirectory(srctblbrows_qt)
  add_subdirectory(strsvr_qt)
endif(BUILD_QT_APPS)

