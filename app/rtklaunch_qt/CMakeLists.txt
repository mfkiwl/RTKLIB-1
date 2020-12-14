find_package(Qt5 COMPONENTS Core Widgets SerialPort REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PNG libpng16 REQUIRED)

add_executable(rtklaunch_qt
  launchmain.cpp
  main.cpp

  launchmain.ui

  rtklaunch_qt.qrc
)

target_link_libraries(rtklaunch_qt Qt5::Widgets Qt5::Core RTKLib ${PNG_LIBRARIES})
target_include_directories(rtklaunch_qt PRIVATE ${PNG_INCLUDE_DIRS})
target_link_directories(rtklaunch_qt PRIVATE ${PNG_LIBRARY_DIRS})