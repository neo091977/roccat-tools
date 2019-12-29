ADD_LIBRARY(libtyongfxplugin SHARED
  tyongfxplugin.c
)

TARGET_LINK_LIBRARIES(libtyongfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libtyongfxplugin
  PROPERTIES
    OUTPUT_NAME tyongfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"TyonGfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libtyongfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
