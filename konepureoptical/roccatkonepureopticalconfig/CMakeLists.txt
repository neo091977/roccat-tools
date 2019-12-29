ADD_EXECUTABLE(roccatkonepureopticalconfig
  konepureopticalconfig.c
  konepureopticalconfig_configuration_dialog.c
  konepureopticalconfig_cpi_scale.c
  konepureopticalconfig_cpi_selector.c
  konepureopticalconfig_profile_page.c
  konepureopticalconfig_single_cpi_selector.c
  konepureopticalconfig_window.c
)

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/konepure/libroccatkonepurewidget
  ${CMAKE_SOURCE_DIR}/konextd/libroccatkonextdwidget
  ${CMAKE_SOURCE_DIR}/koneplus/libroccatkonepluswidget
)

TARGET_LINK_LIBRARIES(roccatkonepureopticalconfig
  libroccatkonepureoptical
  libroccatkonepurewidget
  libroccatkoneplus
  libroccatkonepluswidget
  libroccat
  libroccatwidget
  ${GAMINGGEAR0_WIDGET_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GTK_LIBRARIES}
)

INSTALL(TARGETS roccatkonepureopticalconfig
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccatkonepureopticalconfig.desktop
  DESTINATION share/applications
)
