ADD_EXECUTABLE(roccatarvoconfig
  arvoconfig.c
  arvoconfig_buttons_frame.c
  arvoconfig_configuration_dialog.c
  arvoconfig_key_combo_box.c
  arvoconfig_key_mask_selector.c
  arvoconfig_mode_selector.c
  arvoconfig_profile_page.c
  arvoconfig_window.c
)

TARGET_LINK_LIBRARIES(roccatarvoconfig
  libroccatarvo
  libroccat
  libroccatwidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
)

INSTALL(TARGETS roccatarvoconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatarvoconfig.desktop
  DESTINATION share/applications
)
