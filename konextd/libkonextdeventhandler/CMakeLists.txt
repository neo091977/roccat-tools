ADD_LIBRARY(libkonextdeventhandler SHARED
  konextd_dbus_server.c
  konextd_eventhandler.c
  konextd_eventhandler_channel.c
)

ADD_DEPENDENCIES(libkonextdeventhandler
  generated_headers
  konextd_dbus_bindings
)

SET_TARGET_PROPERTIES(libkonextdeventhandler
  PROPERTIES
    OUTPUT_NAME konextdeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KoneXTDEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libkonextdeventhandler
  libroccat
  libroccatkoneplus
  libroccatkonextd
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libkonextdeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
