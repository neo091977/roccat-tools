ADD_EXECUTABLE(roccatkonepureconfig
  konepureconfig.c
  konepureconfig_configuration_dialog.c
  konepureconfig_profile_page.c
  konepureconfig_window.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/koneplus/libroccatkonepluswidget
  ${CMAKE_SOURCE_DIR}/konepure/libroccatkonepurewidget
  ${CMAKE_SOURCE_DIR}/konextd/libroccatkonextdwidget
)

TARGET_LINK_LIBRARIES(roccatkonepureconfig
  libroccatkonepure
  libroccatkonepurewidget
  libroccatkonextdwidget
  libroccatkoneplus
  libroccatkonepluswidget
  libroccat
  libroccatwidget
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GTK_LIBRARIES}
)

INSTALL(TARGETS roccatkonepureconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatkonepureconfig.desktop
  DESTINATION share/applications
)
