ADD_LIBRARY(libtyoneventhandler SHARED
  tyon_dbus_server.c
  tyon_eventhandler.c
  tyon_eventhandler_channel.c
)

ADD_DEPENDENCIES(libtyoneventhandler
  generated_headers
  tyon_dbus_bindings
)

SET_TARGET_PROPERTIES(libtyoneventhandler
  PROPERTIES
    OUTPUT_NAME tyoneventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"TyonEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libtyoneventhandler
  libroccat
  libroccattyon
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libtyoneventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
