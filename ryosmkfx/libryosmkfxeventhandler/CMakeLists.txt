ADD_LIBRARY(libryosmkfxeventhandler SHARED
  ryosmkfx_dbus_server.c
  ryosmkfx_eventhandler.c
  ryosmkfx_eventhandler_channel.c
)

ADD_DEPENDENCIES(libryosmkfxeventhandler
  generated_headers
  ryosmkfx_dbus_bindings
)

SET_TARGET_PROPERTIES(libryosmkfxeventhandler
  PROPERTIES
    OUTPUT_NAME ryosmkfxeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"RyosMKFXEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libryosmkfxeventhandler
  libroccat
  libroccatryosmkfx
  libroccatryosmk
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libryosmkfxeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
