ADD_EXECUTABLE(roccatryostklconfig
  ryostklconfig.c
  ryostklconfig_blink.c
  ryostklconfig_configuration_dialog.c
  ryostklconfig_effect_selector.c
  ryostklconfig_illumination_frame.c
  ryostklconfig_key_illumination_selector.c
  ryostklconfig_keyboard_selector.c
  ryostklconfig_layer_illumination_dialog.c
  ryostklconfig_led_feedback_frame.c
  ryostklconfig_light_frame.c
  ryostklconfig_notification_frame.c
  ryostklconfig_pro_dimness_selector.c
  ryostklconfig_profile_page.c
  ryostklconfig_window.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/ryosmk/libroccatryosmkwidget
)

TARGET_LINK_LIBRARIES(roccatryostklconfig
  libroccatryostkl
  libroccatryosmk
  libroccatryosmkwidget
  libroccat
  libroccatwidget
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GTK_LIBRARIES}
)

INSTALL(TARGETS roccatryostklconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatryostklconfig.desktop
  DESTINATION share/applications
)
