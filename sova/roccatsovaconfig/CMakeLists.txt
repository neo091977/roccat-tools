ADD_EXECUTABLE(roccatsovaconfig
  sova_capslock_selector.c
  sova_configuration_dialog.c
  sova_gui_key_relations.c
  sova_key_combo_box.c
  sova_key_mask_selector.c
  sova_keyboard_selector.c
  sova_layout_selector.c
  sova_light_effects_frame.c
  sova_light_event_effect_frame.c
  sova_notification_frame.c
  sova_primary_selector.c
  sova_profile_page.c
  sova_sound_feedback_combo_box.c
  sova_standard_selector.c
  sovaconfig.c
  sovaconfig_window.c
)

TARGET_LINK_LIBRARIES(roccatsovaconfig
  libroccatsova
  libroccat
  libroccatwidget
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GTK_LIBRARIES}
)

INSTALL(TARGETS roccatsovaconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatsovaconfig.desktop
  DESTINATION share/applications
)
