ADD_LIBRARY(libkonepuremilitarygfxplugin SHARED
  konepuremilitarygfxplugin.c
)

INCLUDE_DIRECTORIES(
  ${DBUS_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES(libkonepuremilitarygfxplugin
  libroccat
  libroccathelper
  ${DBUS_LIBRARIES}
  ${GLIB2_LIBRARIES}
)

SET_TARGET_PROPERTIES(libkonepuremilitarygfxplugin
  PROPERTIES
    OUTPUT_NAME konepuremilitarygfxplugin
    COMPILE_FLAGS "-DG_LOG_DOMAIN=\\\"KonepuremilitaryGfxplugin\\\" -fvisibility=hidden"
)

INSTALL(TARGETS libkonepuremilitarygfxplugin DESTINATION ${GAMINGGEAR0_FX_PLUGIN_DIR})
