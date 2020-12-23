#Build + install template for cli apps
add_executable(${APP} ${SOURCES})
target_link_libraries(${APP} ${C_BASE_LIBS} RTKLib)

install(TARGETS ${APP}
  BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  COMPONENT cli_apps
)

message(STATUS "Enabled ${APP}")
