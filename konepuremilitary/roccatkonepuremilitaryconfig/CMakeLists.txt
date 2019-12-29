ADD_EXECUTABLE(roccatkonepuremilitaryconfig
  konepuremilitaryconfig.c
  konepuremilitaryconfig_configuration_dialog.c
  konepuremilitaryconfig_profile_page.c
  konepuremilitaryconfig_window.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/konepure/libroccatkonepurewidget
  ${CMAKE_SOURCE_DIR}/koneplus/libroccatkonepluswidget
  ${CMAKE_SOURCE_DIR}/konepuremilitary/libroccatkonepuremilitarywidget
)

TARGET_LINK_LIBRARIES(roccatkonepuremilitaryconfig
  libroccatkonepuremilitary
  libroccatkonepuremilitarywidget
  libroccatkonepurewidget
  libroccatkoneplus
  libroccatkonepluswidget
  libroccat
  libroccatwidget
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GTK_LIBRARIES}
)

INSTALL(TARGETS roccatkonepuremilitaryconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatkonepuremilitaryconfig.desktop
  DESTINATION share/applications
)
