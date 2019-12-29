ADD_EXECUTABLE(roccatnythconfig
  nyth_button_combo_box.c
  nyth_color_frame.c
  nyth_configuration_dialog.c
  nyth_dcu_frame.c
  nyth_light_effects_frame.c
  nyth_notification_frame.c
  nyth_profile_page.c
  nyth_side_buttons_frame.c
  nyth_tcu_calibration_assistant.c
  nyth_tcu_dcu_dialog.c
  nyth_tcu_frame.c
  nyth_top_buttons_frame.c
  nythconfig_window.c
  nythconfig.c
)

TARGET_LINK_LIBRARIES(roccatnythconfig
  libroccatnyth
  libroccat
  libroccatwidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
)

INSTALL(TARGETS roccatnythconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatnythconfig.desktop
  DESTINATION share/applications
)
