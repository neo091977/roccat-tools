ADD_LIBRARY(libsavueventhandler SHARED
  savu_dbus_server.c
  savu_eventhandler.c
  savu_eventhandler_channel.c
)

ADD_DEPENDENCIES(libsavueventhandler
  generated_headers
  savu_dbus_bindings
)

SET_TARGET_PROPERTIES(libsavueventhandler
  PROPERTIES
    OUTPUT_NAME savueventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"SavuEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libsavueventhandler
  libroccat
  libroccatsavu
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libsavueventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
