ADD_LIBRARY(libkirogfxplugin SHARED
  kirogfxplugin.c
)

TARGET_LINK_LIBRARIES(libkirogfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libkirogfxplugin
  PROPERTIES
    OUTPUT_NAME kirogfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KiroGfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libkirogfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
