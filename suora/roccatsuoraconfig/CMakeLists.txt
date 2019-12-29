ADD_EXECUTABLE(roccatsuoraconfig
  suora_gui_key_relations.c
  suora_illumination_frame.c
  suora_key_combo_box.c
  suora_key_selector.c
  suora_keyboard_selector.c
  suora_profile_page.c
  suora_sound_feedback_frame.c
  suoraconfig.c
  suoraconfig_window.c
)

TARGET_LINK_LIBRARIES(roccatsuoraconfig
  libroccatsuora
  libroccat
  libroccatwidget
  ${GAMINGGEAR0_LIBRARY}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GTK_LIBRARIES}
)

INSTALL(TARGETS roccatsuoraconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatsuoraconfig.desktop
  DESTINATION share/applications
)
