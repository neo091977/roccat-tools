ADD_LIBRARY(libryosmkfxgfxplugin SHARED
  ryosmkfxgfxplugin.c
)

TARGET_LINK_LIBRARIES(libryosmkfxgfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libryosmkfxgfxplugin
  PROPERTIES
    OUTPUT_NAME ryosmkfxgfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"RyosmkfxGfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libryosmkfxgfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
