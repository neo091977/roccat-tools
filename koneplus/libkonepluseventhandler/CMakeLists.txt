ADD_LIBRARY(libkonepluseventhandler SHARED
  koneplus_dbus_server.c
  koneplus_eventhandler.c
  koneplus_eventhandler_channel.c
)

ADD_DEPENDENCIES(libkonepluseventhandler
  generated_headers
  koneplus_dbus_bindings
)

SET_TARGET_PROPERTIES(libkonepluseventhandler
  PROPERTIES
    OUTPUT_NAME konepluseventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Kone[+]Eventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libkonepluseventhandler
  libroccat
  libroccatkoneplus
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libkonepluseventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
