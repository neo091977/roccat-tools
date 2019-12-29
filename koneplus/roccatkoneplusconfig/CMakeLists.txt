ADD_EXECUTABLE(roccatkoneplusconfig
  koneplus_color_frame.c
  koneplusconfig.c
  koneplusconfig_configuration_dialog.c
  koneplusconfig_profile_page.c
  koneplusconfig_window.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/koneplus/libroccatkonepluswidget
)

TARGET_LINK_LIBRARIES(roccatkoneplusconfig
  libroccatkoneplus
  libroccat
  libroccatwidget
  libroccatkonepluswidget
  ${GTK_LIBRARIES}
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
)

INSTALL(TARGETS roccatkoneplusconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatkoneplusconfig.desktop
  DESTINATION share/applications
)
