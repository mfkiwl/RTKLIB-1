#Build + install template for cli apps
add_executable(${APP} ${SOURCES})
target_link_libraries(${APP} RTKLib ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS ${APP}
  BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT cli_apps
)