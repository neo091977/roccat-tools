ADD_EXECUTABLE(roccatskeltrconfig
  skeltr_capslock_selector.c
  skeltr_color_frame.c
  skeltr_configuration_dialog.c
  skeltr_fn_selector.c
  skeltr_gui_key_relations.c
  skeltr_keyboard_selector.c
  skeltr_key_mask_selector.c
  skeltr_key_combo_box.c
  skeltr_layout_selector.c
  skeltr_light_effects_frame.c
  skeltr_light_event_effect_frame.c
  skeltr_notification_frame.c
  skeltr_primary_selector.c
  skeltr_profile_page.c
  skeltr_sound_feedback_combo_box.c
  skeltr_standard_selector.c
  skeltrconfig_window.c
  skeltrconfig.c
)

TARGET_LINK_LIBRARIES(roccatskeltrconfig
  libroccatskeltr
  libroccat
  libroccatwidget
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GTK_LIBRARIES}
)

INSTALL(TARGETS roccatskeltrconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatskeltrconfig.desktop
  DESTINATION share/applications
)
