ADD_LIBRARY(libkiroeventhandler SHARED
  kiro_dbus_server.c
  kiro_eventhandler.c
  kiro_eventhandler_channel.c
)

ADD_DEPENDENCIES(libkiroeventhandler
  generated_headers
  kiro_dbus_bindings
)

SET_TARGET_PROPERTIES(libkiroeventhandler
  PROPERTIES
    OUTPUT_NAME kiroeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KiroEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libkiroeventhandler
  libroccat
  libroccatkiro
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libkiroeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
