ADD_EXECUTABLE(roccatkonextdconfig
  konextdconfig.c
  konextdconfig_configuration_dialog.c
  konextdconfig_profile_page.c
  konextdconfig_window.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/koneplus/libroccatkonepluswidget
  ${CMAKE_SOURCE_DIR}/konextd/libroccatkonextdwidget
)

TARGET_LINK_LIBRARIES(roccatkonextdconfig
  libroccatkonextd
  libroccatkonextdwidget
  libroccatkoneplus
  libroccatkonepluswidget
  libroccat
  libroccatwidget
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GTK_LIBRARIES}
)

INSTALL(TARGETS roccatkonextdconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatkonextdconfig.desktop
  DESTINATION share/applications
)
