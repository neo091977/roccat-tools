ADD_LIBRARY(libiskufxeventhandler SHARED
  iskufx_dbus_server.c
  iskufx_eventhandler.c
  iskufx_eventhandler_channel.c
)

ADD_DEPENDENCIES(libiskufxeventhandler
  generated_headers
  iskufx_dbus_bindings
)

SET_TARGET_PROPERTIES(libiskufxeventhandler
  PROPERTIES
    OUTPUT_NAME iskufxeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"IskuFXEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libiskufxeventhandler
  libroccat
  libroccatisku
  libroccatiskufx
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libiskufxeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
