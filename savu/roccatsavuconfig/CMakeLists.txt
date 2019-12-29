ADD_EXECUTABLE(roccatsavuconfig
  savuconfig.c
  savuconfig_buttons_frame.c
  savuconfig_configuration_dialog.c
  savuconfig_cpi_selector.c
  savuconfig_key_combo_box.c
  savuconfig_light_frame.c
  savuconfig_profile_page.c
  savuconfig_window.c
)

TARGET_LINK_LIBRARIES(roccatsavuconfig
  libroccatsavu
  libroccat
  libroccatwidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
)

INSTALL(TARGETS roccatsavuconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatsavuconfig.desktop
  DESTINATION share/applications
)
