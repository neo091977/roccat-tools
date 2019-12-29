ADD_EXECUTABLE(roccattyonconfig
  tyonconfig.c
  tyonconfig_window.c
  tyon_2d_positional.c
  tyon_buttons_frame.c
  tyon_color_frame.c
  tyon_configuration_dialog.c
  tyon_cpi_selector.c
  tyon_dcu_frame.c
  tyon_info_dialog.c
  tyon_key_combo_box.c
  tyon_light_effects_frame.c
  tyon_profile_page.c
  tyon_tcu_calibration_assistant.c
  tyon_tcu_dcu_dialog.c
  tyon_tcu_frame.c
  tyon_xcelerator_calibration_assistant.c
)

TARGET_LINK_LIBRARIES(roccattyonconfig
  libroccattyon
  libroccat
  libroccatwidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
)

INSTALL(TARGETS roccattyonconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccattyonconfig.desktop
  DESTINATION share/applications
)
