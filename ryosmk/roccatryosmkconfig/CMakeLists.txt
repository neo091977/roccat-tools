ADD_EXECUTABLE(roccatryosmkconfig
  ryosconfig.c
  ryosconfig_blink.c
  ryosconfig_effect_selector.c
  ryosconfig_illumination_frame.c
  ryosconfig_key_illumination_selector.c
  ryosconfig_keyboard_selector.c
  ryosconfig_layer_illumination_dialog.c
  ryosconfig_led_feedback_frame.c
  ryosconfig_light_frame.c
  ryosconfig_pro_dimness_selector.c
  ryosconfig_profile_page.c
  ryosconfig_window.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/ryosmk/libroccatryosmkwidget
)

TARGET_LINK_LIBRARIES(roccatryosmkconfig
  libroccatryosmk
  libroccatryosmkwidget
  libroccat
  libroccatwidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
)

INSTALL(TARGETS roccatryosmkconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatryosmkconfig.desktop
  DESTINATION share/applications
)
