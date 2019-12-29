ADD_EXECUTABLE(roccatpyraconfig
  pyraconfig.c
  pyraconfig_buttons_frame.c
  pyraconfig_configuration_dialog.c
  pyraconfig_cpi_selector.c
  pyraconfig_key_combo_box.c
  pyraconfig_light_frame.c
  pyraconfig_profile_page.c
  pyraconfig_sensitivity_frame.c
  pyraconfig_window.c
)

TARGET_LINK_LIBRARIES(roccatpyraconfig
  libroccatpyra
  libroccat
  libroccatwidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
)

INSTALL(TARGETS roccatpyraconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatpyraconfig.desktop
  DESTINATION share/applications
)
