ADD_LIBRARY(libkonepuremilitaryeventhandler SHARED
  konepuremilitary_dbus_server.c
  konepuremilitary_eventhandler.c
  konepuremilitary_eventhandler_channel.c
)

ADD_DEPENDENCIES(libkonepuremilitaryeventhandler
  generated_headers
  konepuremilitary_dbus_bindings
)

SET_TARGET_PROPERTIES(libkonepuremilitaryeventhandler
  PROPERTIES
    OUTPUT_NAME konepuremilitaryeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KonePureMilitaryEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libkonepuremilitaryeventhandler
  libroccat
  libroccatkonepuremilitary
  libroccatkoneplus
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libkonepuremilitaryeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
