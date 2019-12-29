ADD_LIBRARY(libiskueventhandler SHARED
  isku_dbus_server.c
  isku_eventhandler.c
  isku_eventhandler_channel.c
)

ADD_DEPENDENCIES(libiskueventhandler
  generated_headers
  isku_dbus_bindings
)

SET_TARGET_PROPERTIES(libiskueventhandler
  PROPERTIES
    OUTPUT_NAME iskueventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"IskuEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libiskueventhandler
  libroccat
  libroccatisku
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libiskueventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
