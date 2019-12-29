ADD_LIBRARY(libryostkleventhandler SHARED
  ryostkl_dbus_server.c
  ryostkl_eventhandler.c
  ryostkl_eventhandler_channel.c
)

ADD_DEPENDENCIES(libryostkleventhandler
  generated_headers
  ryostkl_dbus_bindings
)

SET_TARGET_PROPERTIES(libryostkleventhandler
  PROPERTIES
    OUTPUT_NAME ryostkleventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"RyosTKLEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libryostkleventhandler
  libroccat
  libroccatryostkl
  libroccatryosmk
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libryostkleventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
