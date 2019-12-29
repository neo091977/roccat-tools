ADD_EXECUTABLE(roccatiskuconfig
  iskuconfig.c
  iskuconfig_configuration_dialog.c
  iskuconfig_profile_page.c
  iskuconfig_window.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/isku/libroccatiskuwidget
)

TARGET_LINK_LIBRARIES(roccatiskuconfig
  libroccatisku
  libroccat
  libroccatwidget
  libroccatiskuwidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
)

INSTALL(TARGETS roccatiskuconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatiskuconfig.desktop
  DESTINATION share/applications
)
