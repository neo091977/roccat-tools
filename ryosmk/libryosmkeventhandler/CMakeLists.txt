ADD_LIBRARY(libryosmkeventhandler SHARED
  ryos_dbus_server.c
  ryos_eventhandler.c
  ryos_eventhandler_channel.c
)

ADD_DEPENDENCIES(libryosmkeventhandler
  generated_headers
  ryosmk_dbus_bindings
)

SET_TARGET_PROPERTIES(libryosmkeventhandler
  PROPERTIES
    OUTPUT_NAME ryosmkeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"RyosMKEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libryosmkeventhandler
  libroccat
  libroccatryosmk
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libryosmkeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
