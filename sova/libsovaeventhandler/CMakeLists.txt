ADD_LIBRARY(libsovaeventhandler SHARED
  sova_dbus_server.c
  sova_eventhandler.c
  sova_eventhandler_channel_special.c
  sova_eventhandler_channel_typing.c
  sova_notification_powersave.c
  sova_notification_typing.c
)

ADD_DEPENDENCIES(libsovaeventhandler
  generated_headers
  sova_dbus_bindings
)

SET_TARGET_PROPERTIES(libsovaeventhandler
  PROPERTIES
    OUTPUT_NAME sovaeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"SovaEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libsovaeventhandler
  libroccat
  libroccatsova
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libsovaeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
