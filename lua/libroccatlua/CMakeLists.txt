ADD_LIBRARY(libroccatlua SHARED
  lua_config.c
  lua_control.c
  lua_cpi.c
  lua_polling_rate.c
  lua_rmp_accessors.c
  lua_rmp_file.c
  lua_rmp_high_level.c
  lua.c
)

ADD_DEPENDENCIES(libroccatlua
  generated_headers
  lua_dbus_bindings
)

TARGET_LINK_LIBRARIES(libroccatlua
  libroccat
  libroccathelper
  ${GAMINGGEAR0_LIBRARY}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libroccatlua
  PROPERTIES
    VERSION ${V_MAJOR}.${V_MINOR}.${V_PATCH}
    SOVERSION ${V_MAJOR}
    OUTPUT_NAME roccatlua
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Lua\\\""
)

INSTALL(TARGETS libroccatlua DESTINATION ${LIBDIR})
