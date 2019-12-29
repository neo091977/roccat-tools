ADD_EXECUTABLE(roccatiskufxconfig
  iskufxconfig.c
  iskufxconfig_color_frame.c
  iskufxconfig_configuration_dialog.c
  iskufxconfig_profile_page.c
  iskufxconfig_window.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/isku/libroccatiskuwidget
)

TARGET_LINK_LIBRARIES(roccatiskufxconfig
  libroccatiskufx
  libroccatisku
  libroccatiskuwidget
  libroccat
  libroccatwidget
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GTK_LIBRARIES}
)

INSTALL(TARGETS roccatiskufxconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatiskufxconfig.desktop
  DESTINATION share/applications
)
