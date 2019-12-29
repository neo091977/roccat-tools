ADD_LIBRARY(libluaeventhandler SHARED
  lua_dbus_server.c
  lua_eventhandler.c
)

ADD_DEPENDENCIES(libluaeventhandler
  generated_headers
  lua_dbus_bindings
)

SET_TARGET_PROPERTIES(libluaeventhandler
  PROPERTIES
    OUTPUT_NAME luaeventhandler
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"LuaEventhandler\\\" -fvisibility=hidden"
)

TARGET_LINK_LIBRARIES(libluaeventhandler
  libroccat
  libroccatlua
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${DBUS-GLIB_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

INSTALL(TARGETS libluaeventhandler DESTINATION ${EVENTHANDLER_PLUGIN_DIR})
