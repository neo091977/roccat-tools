ADD_LIBRARY(libkonepuregfxplugin SHARED
  konepuregfxplugin.c
)

INCLUDE_DIRECTORIES(
  ${DBUS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(libkonepuregfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libkonepuregfxplugin
  PROPERTIES
    OUTPUT_NAME konepuregfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KonepureGfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libkonepuregfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
