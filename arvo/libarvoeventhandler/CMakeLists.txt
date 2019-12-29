ADD_LIBRARY(libarvoeventhandler SHARED
  arvo_dbus_server.c
  arvo_eventhandler.c
  arvo_eventhandler_channel.c
)

ADD_DEPENDENCIES(libarvoeventhandler
  generated_headers
  arvo_dbus_bindings
)

SET_TARGET_PROPERTIES(libarvoeventhandler
  PROPERTIES
    OUTPUT_NAME arvoeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"ArvoEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libarvoeventhandler
  libroccat
  libroccatarvo
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libarvoeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
