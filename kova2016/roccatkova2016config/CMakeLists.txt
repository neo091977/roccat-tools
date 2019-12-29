ADD_EXECUTABLE(roccatkova2016config
  kova2016_button_combo_box.c
  kova2016_buttons_frame.c
  kova2016_color_frame.c
  kova2016_configuration_dialog.c
  kova2016_cpi_dialog.c
  kova2016_light_effects_frame.c
  kova2016_notification_frame.c
  kova2016_profile_page.c
  kova2016config_window.c
  kova2016config.c
)

TARGET_LINK_LIBRARIES(roccatkova2016config
  libroccatkova2016
  libroccat
  libroccatwidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
)

INSTALL(TARGETS roccatkova2016config
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatkova2016config.desktop
  DESTINATION share/applications
)
