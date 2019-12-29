ADD_EXECUTABLE(roccatkonextdopticalconfig
  konextdopticalconfig.c
  konextdopticalconfig_configuration_dialog.c
  konextdopticalconfig_profile_page.c
  konextdopticalconfig_window.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/koneplus/libroccatkonepluswidget
  ${CMAKE_SOURCE_DIR}/konextd/libroccatkonextdwidget
  ${CMAKE_SOURCE_DIR}/konextdoptical/libroccatkonextdopticalwidget
  ${CMAKE_SOURCE_DIR}/konepuremilitary/libroccatkonepuremilitarywidget
)

TARGET_LINK_LIBRARIES(roccatkonextdopticalconfig
  libroccatkonextdoptical
  libroccatkonepuremilitary
  libroccatkonepuremilitarywidget
  libroccatkoneplus
  libroccatkonepluswidget
  libroccatkonextdwidget
  libroccat
  libroccatwidget
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GTK_LIBRARIES}
)

INSTALL(TARGETS roccatkonextdopticalconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatkonextdopticalconfig.desktop
  DESTINATION share/applications
)
