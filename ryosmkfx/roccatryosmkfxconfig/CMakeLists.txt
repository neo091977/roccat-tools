ADD_EXECUTABLE(roccatryosmkfxconfig
  ryosmkfxconfig.c
  ryosmkfxconfig_window.c
  ryosmkfx_capslock_selector.c
  ryosmkfx_configuration_dialog.c
  ryosmkfx_dimness_selector.c
  ryosmkfx_effect_gui.c
  ryosmkfx_illumination_frame.c
  ryosmkfx_key_combo_box.c
  ryosmkfx_key_illumination_selector.c
  ryosmkfx_keyboard_selector.c
  ryosmkfx_layer_editor_dialog.c
  ryosmkfx_led_feedback_frame.c
  ryosmkfx_light_frame.c
  ryosmkfx_light_effects_frame.c
  ryosmkfx_light_effects_emulator.c
  ryosmkfx_profile_page.c
  ryosmkfx_standard_selector.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/ryosmk/libroccatryosmkwidget
)

TARGET_LINK_LIBRARIES(roccatryosmkfxconfig
  libroccatryosmkfx
  libroccatryosmk
  libroccatryosmkwidget
  libroccat
  libroccatwidget
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GTK_LIBRARIES}
  ${GTHREAD2_LIBRARIES}
)

INSTALL(TARGETS roccatryosmkfxconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatryosmkfxconfig.desktop
  DESTINATION share/applications
)
