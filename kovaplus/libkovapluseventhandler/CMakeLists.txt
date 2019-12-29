ADD_LIBRARY(libkovapluseventhandler SHARED
  kovaplus_dbus_server.c
  kovaplus_eventhandler.c
  kovaplus_eventhandler_channel.c
)

ADD_DEPENDENCIES(libkovapluseventhandler
  generated_headers
  kovaplus_dbus_bindings
)

SET_TARGET_PROPERTIES(libkovapluseventhandler
  PROPERTIES
    OUTPUT_NAME kovapluseventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Kova[+]Eventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libkovapluseventhandler
  libroccat
  libroccatkovaplus
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libkovapluseventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
