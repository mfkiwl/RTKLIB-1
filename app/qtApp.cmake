#Generic code for all rtklib QT applications

#Additional QT UI Headers
include_directories(../appcmn_qt)

#find_package(PkgConfig REQUIRED)
find_package(PNG REQUIRED)
find_package(Qt5 COMPONENTS Core Widgets SerialPort REQUIRED)

#For portability, link the GUI apps against a static RTKLIB library
#Note that the shared library contains dummy functions used
#to print progress to the QT window 'console' during processing

set(APP_LIBS  ${C_BASE_LIBS} ${CXX_BASE_LIBS}
              ${PNG_LIBRARIES} Qt5::Widgets Qt5::Core Qt5::SerialPort
              RTKLib_qt
)
set (APP_INCLUDES ${PNG_INCLUDE_DIRS})
set (APP_DEFINES ${PNG_DEFINITIONS})

add_definitions(${APP_DEFINES})

if(APPLE)
  set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/${APP}.icns)
  set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
  add_executable(${APP} ${SOURCES} ${APP_ICON_MACOSX})
  SET_TARGET_PROPERTIES(${APP} PROPERTIES MACOSX_BUNDLE TRUE)
else()
  add_executable(${APP} ${SOURCES})
endif(APPLE)

target_link_libraries(${APP} ${APP_LIBS})
target_include_directories(${APP} PRIVATE ${APP_INCLUDES})

if(APPLE)
  install(TARGETS ${APP}
    #BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
    DESTINATION .
    #BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
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

message(STATUS "Enabled ${APP}")

