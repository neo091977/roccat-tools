ADD_LIBRARY(libkonepureopticaleventhandler SHARED
  konepureoptical_dbus_server.c
  konepureoptical_eventhandler.c
  konepureoptical_eventhandler_channel.c
)

ADD_DEPENDENCIES(libkonepureopticaleventhandler
  generated_headers
  konepureoptical_dbus_bindings
)

SET_TARGET_PROPERTIES(libkonepureopticaleventhandler
  PROPERTIES
    OUTPUT_NAME konepureopticaleventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KonePureOpticalEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libkonepureopticaleventhandler
  libroccat
  libroccatkonepureoptical
  libroccatkoneplus
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libkonepureopticaleventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
