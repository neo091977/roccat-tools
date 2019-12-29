ADD_LIBRARY(libpyraeventhandler SHARED
  pyra_dbus_server.c
  pyra_eventhandler.c
  pyra_eventhandler_channel.c
)

ADD_DEPENDENCIES(libpyraeventhandler
  generated_headers
  pyra_dbus_bindings
)

SET_TARGET_PROPERTIES(libpyraeventhandler
  PROPERTIES
    OUTPUT_NAME pyraeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"PyraEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libpyraeventhandler
  libroccat
  libroccatpyra
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libpyraeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
