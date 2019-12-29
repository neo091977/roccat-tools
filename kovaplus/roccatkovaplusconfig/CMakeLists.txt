ADD_EXECUTABLE(roccatkovaplusconfig
  kovaplusconfig.c
  kovaplusconfig_buttons_frame.c
  kovaplusconfig_configuration_dialog.c
  kovaplusconfig_cpi_selector.c
  kovaplusconfig_key_combo_box.c
  kovaplusconfig_light_frame.c
  kovaplusconfig_profile_page.c
  kovaplusconfig_sensitivity_frame.c
  kovaplusconfig_window.c
)

TARGET_LINK_LIBRARIES(roccatkovaplusconfig
  libroccatkovaplus
  libroccat
  libroccatwidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
)

INSTALL(TARGETS roccatkovaplusconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatkovaplusconfig.desktop
  DESTINATION share/applications
)
