#Generic code for all rtklib QT applications
find_package(Qt5 COMPONENTS Core Widgets SerialPort REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PNG libpng16 REQUIRED)

if(APPLE)
  set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/${APP}.icns)
  set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
  add_executable(${APP} MACOSX_BUNDLE ${SOURCES} ${APP_ICON_MACOSX})
else()
  add_executable(${APP} ${SOURCES})
endif(APPLE)

#For portability, link the GUI apps against the static library
target_link_libraries(${APP} Qt5::Widgets Qt5::Core Qt5::SerialPort RTKLib ${PNG_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${EXTRA_LIBS})
target_include_directories(${APP} PRIVATE ${PNG_INCLUDE_DIRS})
target_link_directories(${APP} PRIVATE ${PNG_LIBRARY_DIRS})

if(APPLE)
  install(TARGETS ${APP}
    BUNDLE DESTINATION /Applications
    COMPONENT qt_apps
  )
else()
  install(TARGETS ${APP}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT qt_apps
  )
endif(APPLE)
  #Copy the rtklib shared library and QT libraries into each app.
  #ToDo - Share libraries
  #Adds bloat but it's portable
  #ADD_CUSTOM_COMMAND( TARGET ${APP} COMMAND macdeployqt ARGS ${CMAKE_CURRENT_BINARY_DIR}/${APP}.app )
  # install(CODE
  #   "
  #     include(InstallRequiredSystemLibraries)
  #     include(BundleUtilities)
  #     fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/${APP}.app\"  \"\" \"\")
  #   "
  #   COMPONENT qt_apps
  # )
