ADD_LIBRARY(libnythgfxplugin SHARED
  nythgfxplugin.c
)

TARGET_LINK_LIBRARIES(libnythgfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libnythgfxplugin
  PROPERTIES
    OUTPUT_NAME nythgfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"NythGfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libnythgfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
