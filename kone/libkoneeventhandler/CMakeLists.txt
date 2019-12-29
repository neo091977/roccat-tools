ADD_LIBRARY(libkoneeventhandler SHARED
  kone_dbus_server.c
  kone_eventhandler.c
  kone_eventhandler_channel.c
)

ADD_DEPENDENCIES(libkoneeventhandler
  generated_headers
  kone_dbus_bindings
)

SET_TARGET_PROPERTIES(libkoneeventhandler
  PROPERTIES
    OUTPUT_NAME koneeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KoneEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libkoneeventhandler
  libroccat
  libroccathelper
  libroccatkone
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libkoneeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
