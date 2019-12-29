ADD_LIBRARY(libskeltreventhandler SHARED
  skeltr_dbus_server.c
  skeltr_eventhandler.c
  skeltr_eventhandler_channel_special.c
  skeltr_eventhandler_channel_typing.c
  skeltr_notification_powersave.c
  skeltr_notification_typing.c
)

ADD_DEPENDENCIES(libskeltreventhandler
  generated_headers
  skeltr_dbus_bindings
)

SET_TARGET_PROPERTIES(libskeltreventhandler
  PROPERTIES
    OUTPUT_NAME skeltreventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"SkeltrEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libskeltreventhandler
  libroccat
  libroccatskeltr
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libskeltreventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
