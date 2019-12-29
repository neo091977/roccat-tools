ADD_LIBRARY(libkonepureeventhandler SHARED
  konepure_dbus_server.c
  konepure_eventhandler.c
  konepure_eventhandler_channel.c
)

ADD_DEPENDENCIES(libkonepureeventhandler
  generated_headers
  konepure_dbus_bindings
)

SET_TARGET_PROPERTIES(libkonepureeventhandler
  PROPERTIES
    OUTPUT_NAME konepureeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KonePureEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libkonepureeventhandler
  libroccat
  libroccatkoneplus
  libroccatkonepure
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libkonepureeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
