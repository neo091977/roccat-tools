ADD_EXECUTABLE(roccatluaconfig
  luaconfig.c
  luaconfig_buttons_frame.c
  luaconfig_configuration_dialog.c
  luaconfig_cpi_selector.c
  luaconfig_handedness_selector.c
  luaconfig_key_combo_box.c
  luaconfig_light_frame.c
  luaconfig_polling_rate_frame.c
  luaconfig_profile_page.c
  luaconfig_surface_check.c
  luaconfig_window.c
)

TARGET_LINK_LIBRARIES(roccatluaconfig
  libroccatlua
  libroccat
  libroccatwidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
)

INSTALL(TARGETS roccatluaconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatluaconfig.desktop
  DESTINATION share/applications
)
