add_dbus_bindings(roccat)

INCLUDE_DIRECTORIES(
  ${CMAKE_BINARY_DIR}/roccateventhandler
  ${X11_INCLUDE_DIRS}
)

ADD_EXECUTABLE(roccateventhandler
  roccat_dbus_server.c
  roccat_eventhandler.c
  roccat_eventhandler_application.c
  roccat_window_monitor.c
  roccateventhandler_config.c
)

ADD_DEPENDENCIES(roccateventhandler
  generated_headers
  roccat_dbus_bindings
)

SET_TARGET_PROPERTIES(roccateventhandler
  PROPERTIES
    COMPILE_FLAGS "-fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(roccateventhandler
  libroccat
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
  ${GMODULE2_LIBRARIES}
  ${DBUS-GLIB_LIBRARIES}
  ${X11_LIBRARIES}
)

INSTALL(TARGETS roccateventhandler
  DESTINATION bin
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

INSTALL(FILES roccateventhandler.desktop
  DESTINATION /etc/xdg/autostart
)
