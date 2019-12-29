ADD_LIBRARY(libkonextdopticaleventhandler SHARED
  konextdoptical_dbus_server.c
  konextdoptical_eventhandler.c
  konextdoptical_eventhandler_channel.c
)

ADD_DEPENDENCIES(libkonextdopticaleventhandler
  generated_headers
  konextdoptical_dbus_bindings
)

SET_TARGET_PROPERTIES(libkonextdopticaleventhandler
  PROPERTIES
    OUTPUT_NAME konextdopticaleventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KoneXTDOpticalEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libkonextdopticaleventhandler
  libroccat
  libroccatkonextdoptical
  libroccatkoneplus
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libkonextdopticaleventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
