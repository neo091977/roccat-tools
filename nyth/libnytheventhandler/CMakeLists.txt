ADD_LIBRARY(libnytheventhandler SHARED
  nyth_dbus_server.c
  nyth_eventhandler.c
  nyth_eventhandler_channel.c
)

ADD_DEPENDENCIES(libnytheventhandler
  generated_headers
  nyth_dbus_bindings
)

SET_TARGET_PROPERTIES(libnytheventhandler
  PROPERTIES
    OUTPUT_NAME nytheventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"NythEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libnytheventhandler
  libroccat
  libroccatnyth
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libnytheventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
