ADD_LIBRARY(libsuoraeventhandler SHARED
  suora_dbus_server.c
  suora_eventhandler.c
  suora_eventhandler_channel.c
  suora_notification_typing.c
)

ADD_DEPENDENCIES(libsuoraeventhandler
  generated_headers
  suora_dbus_bindings
)

SET_TARGET_PROPERTIES(libsuoraeventhandler
  PROPERTIES
    OUTPUT_NAME suoraeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"SuoraEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libsuoraeventhandler
  libroccat
  libroccatsuora
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libsuoraeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
