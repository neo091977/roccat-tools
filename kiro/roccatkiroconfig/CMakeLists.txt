ADD_EXECUTABLE(roccatkiroconfig
  kiro_button_combo_box.c
  kiro_buttons_frame.c
  kiro_color_frame.c
  kiro_configuration_dialog.c
  kiro_dcu_frame.c
  kiro_light_effects_frame.c
  kiro_notification_frame.c
  kiro_profile_page.c
  kiro_sensitivity_frame.c
  kiroconfig_window.c
  kiroconfig.c
)

TARGET_LINK_LIBRARIES(roccatkiroconfig
  libroccatkiro
  libroccat
  libroccatwidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
)

INSTALL(TARGETS roccatkiroconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatkiroconfig.desktop
  DESTINATION share/applications
)
