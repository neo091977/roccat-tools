ADD_EXECUTABLE(roccatkoneconfig
  koneconfig.c
  koneconfig_buttons_frame.c
  koneconfig_color_frame.c
  koneconfig_configuration_dialog.c
  koneconfig_cpi_selector.c
  koneconfig_info_dialog.c
  koneconfig_key_combo_box.c
  koneconfig_light_effects_frame.c
  koneconfig_polling_rate_frame.c
  koneconfig_profile_page.c
  koneconfig_sensitivity_frame.c
  koneconfig_shortcut_dialog.c
  koneconfig_window.c
)

TARGET_LINK_LIBRARIES(roccatkoneconfig
  libroccatkone
  libroccat
  libroccatwidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
)

INSTALL(TARGETS roccatkoneconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatkoneconfig.desktop
  DESTINATION share/applications
)
