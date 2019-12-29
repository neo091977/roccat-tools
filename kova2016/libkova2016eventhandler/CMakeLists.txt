ADD_LIBRARY(libkova2016eventhandler SHARED
  kova2016_dbus_server.c
  kova2016_eventhandler.c
  kova2016_eventhandler_channel.c
)

ADD_DEPENDENCIES(libkova2016eventhandler
  generated_headers
  kova2016_dbus_bindings
)

SET_TARGET_PROPERTIES(libkova2016eventhandler
  PROPERTIES
    OUTPUT_NAME kova2016eventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Kova2016Eventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libkova2016eventhandler
  libroccat
  libroccatkova2016
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libkova2016eventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
