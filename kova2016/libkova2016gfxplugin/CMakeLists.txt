ADD_LIBRARY(libkova2016gfxplugin SHARED
  kova2016gfxplugin.c
)

TARGET_LINK_LIBRARIES(libkova2016gfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libkova2016gfxplugin
  PROPERTIES
    OUTPUT_NAME kova2016gfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"Kova2016Gfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libkova2016gfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
